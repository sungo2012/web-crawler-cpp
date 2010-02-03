/*
 * TinyDNSResolver.cpp
 *
 *  Created on: Jun 2, 2009
 *      Author: luo
 */

#include "TinyDNSResolver.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>

const char* TinyDNSResolver::DNS_ADDR[DNS_SERVERS] = {"202.97.224.69", "202.97.224.68"};
const int TinyDNSResolver::DNS_PORT = 53;

TinyDNSResolver::TinyDNSResolver() {
	bzero(dnscache._domain, sizeof(dnscache._domain));
	bzero(dnscache._ip, sizeof(dnscache._ip));
}

TinyDNSResolver::~TinyDNSResolver() {

}
bool TinyDNSResolver::resolve(const char* domain, char* ip, unsigned short id) {

	bool result = false;

	if (strcmp(domain, dnscache._domain) == 0) {
		//hit dns cache
		if (dnscache._ip[0] == '\0')
			return false;
		strncpy(ip, dnscache._ip, 16);
		return true;

	} else {
		DNS_PKG_HEADER_PTR nphp = new DNS_PKG_HEADER;
		DNS_PKG_QUERY_PTR dkqp = new DNS_PKG_QUERY;

		unsigned char dnsBuff[1024];
		unsigned char dnsRecv[2048];
		bzero(dnsBuff, 1024);
		bzero(dnsRecv, 2048);

		int send_size = pkgDNS(domain, nphp, dkqp, dnsBuff, id);//fulfil dns package
		if(send_size == -1)
			return false;

		int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd < 0) {
			return false;
		}

		struct sockaddr_in server_addr;
		unsigned int pkg_len = 0;
		bzero(&server_addr, sizeof(struct sockaddr_in));


		int server_id  = id % DNS_SERVERS;
		server_addr.sin_family = AF_INET;
		//在寝室用［1］,[1]是学校dns服务器
		inet_aton(DNS_ADDR[server_id], &server_addr.sin_addr);
		server_addr.sin_port = htons(DNS_PORT);

		pkg_len = sizeof(server_addr);

		int in_len = sendto(sockfd, dnsBuff, sizeof(dnsBuff), 0,
				(struct sockaddr*) &server_addr, pkg_len);
		if (in_len < 0) {
//			std::cout << "tiny dns:ERROR sendto" << strerror(errno)<<std::endl;
			close(sockfd);
			return false;
		}

		int recv_len = 0;
		// if time is out , try again (3 times totally)
//		for (int i = 0; i < 3 && recv_len == 0; i++)
			recv_len = recvfromTimeOut(sockfd, 0, 5000000);//wait 5 seconds

		if (recv_len == 0 || recv_len < 0) {
			// if in_len == 0 stands for timeout, if -1 stands for error
			strncpy(dnscache._domain, domain, 256);
			dnscache._ip[0] = '\0';
			close(sockfd);
			return false;
		}
		else {
			//select returns fd it must be sockfd, because only sockfd is selected;
			recv_len = recvfrom(sockfd, dnsRecv, sizeof(dnsRecv), 0,
					(struct sockaddr*) &server_addr, &pkg_len);

			if (recv_len < 0) {
//				std::cout << "tiny dns:ERROR recvfrom" << std::endl;
				strncpy(dnscache._domain, domain, 256);
				dnscache._ip[0] = '\0';
				close(sockfd);
				return false;
			}

			result = recvAnalyse(dnsRecv, recv_len, send_size, ip);
			if (result) {
				strncpy(dnscache._domain, domain, 256);
				if(strcmp(ip, "125.211.213.133") == 0){
					result = false;
					dnscache._ip[0] = '\0';

				}
				else{
					strncpy(dnscache._ip, ip, 16);
				}
			} else {
				strncpy(dnscache._domain, domain, 256);
				dnscache._ip[0] = '\0';
			}
		}

		close(sockfd);
		return result;
	}
}

int TinyDNSResolver::pkgDNS(const char* domain, DNS_PKG_HEADER_PTR& nphp,
		DNS_PKG_QUERY_PTR& dkqp, unsigned char* dnsBuff, unsigned short id) {
	char tmpBuf[256];
	bzero(tmpBuf, 256);
	int domainLen = strlen( domain);
	if(domainLen <= 0)
		return -1;
	memcpy(tmpBuf, domain, domainLen);
	dkqp->dns_name = new unsigned char[domainLen + 2* sizeof (unsigned char)];
	bzero(dkqp->dns_name, domainLen+2* sizeof (unsigned char));

	char* tok = NULL;
	tok = strtok(tmpBuf, ".");
	unsigned char dot = '\0';
	int offset = 0;

	while(tok != NULL) {
		dot = (unsigned char)strlen(tok);
		memcpy(dkqp->dns_name +offset, &dot, sizeof(unsigned char));
		offset += sizeof(unsigned char);
		memcpy(dkqp->dns_name +offset, tok, strlen(tok));
		offset += strlen(tok);

		tok = strtok(NULL, ".");
	}
	dkqp->dns_name[domainLen+ 2*sizeof(unsigned char) -1] = 0x00;

	nphp->id = htons(id);//dns transaction id, given randomly
			nphp->flag = htons(0x0100); //dns standard query;
			nphp->questions = htons(0x0001); //num of questions;
			nphp->answerRRs = htons(0x0000);
			nphp->authorityRRs = htons(0x0000);
			nphp->additionalRRs = htons(0x0000);

			dkqp->dns_type = htons(0x0001); //Type	: A
			dkqp->dns_class = htons(0x0001); //Class : IN


			memcpy(dnsBuff, (unsigned char*)nphp, sizeof(DNS_PKG_HEADER));
			memcpy(dnsBuff+sizeof(DNS_PKG_HEADER), (unsigned char*)dkqp->dns_name,domainLen+2*sizeof(unsigned char));
	memcpy(dnsBuff+sizeof(DNS_PKG_HEADER)+(domainLen+2*sizeof(unsigned char)), (unsigned char*)&dkqp->dns_type, sizeof(unsigned short));
	memcpy(dnsBuff+sizeof(DNS_PKG_HEADER)+(domainLen+2*sizeof(unsigned char))+sizeof(unsigned short), (unsigned char*)&dkqp->dns_class, sizeof(unsigned short));
	delete[] dkqp->dns_name;


	return sizeof(DNS_PKG_HEADER)+(domainLen+2*sizeof(unsigned char))+sizeof(unsigned short)+sizeof(unsigned short);
}

bool TinyDNSResolver::recvAnalyse(unsigned char* buf, size_t buf_size,
		size_t send_size, char* ip) {

	bool result = false;
	unsigned char* p = buf;

	p += 2;//dns id

	unsigned short flag =ntohs(*((unsigned short*) p));// p[0] * 0x100 + p[1];
	if (flag != 0x8180) {
//		std::cout << "not a \"standard query response no error\"! "
//				<< std::endl;
		return false;
	}
	p += 2;//dns flag

	p += 2;//dns questions

	unsigned short answerRRs = ntohs(*((unsigned short*) p));//p[0] * 0x100 + p[1];//dns answer RRs

	p = buf + send_size;//p point to Answers

	unsigned short type;
	unsigned short dataLen;
	for (int i = 0; i < answerRRs; i++) {
		p += 2;//Name
		type = ntohs(*((unsigned short*) p));//p[0] * 0x100 + p[1];
		p += 2;//Type;
		if (type == 0x0001) {
			p += 2;//Class
			p += 4;//TTL
			p += 2;//Data Length
			strncpy(ip, inet_ntoa(*(struct in_addr*) p), 16);
			result = true;
			break;
		}
		p += 2;//Class
		p += 4;//TTL
		dataLen = ntohs(*((unsigned short*) p));//p[0] * 0x100 + p[1];
		p += 2;//Data Length
		p += dataLen;//data
	}

	return result;
}

int TinyDNSResolver::recvfromTimeOut(int socket, long sec, long usec) {

	// Setup timeval variable
	timeval timeout;
	timeout.tv_sec = sec;
	timeout.tv_usec = usec;
	// Setup fd_set structure
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(socket, &fds);
	// Return value:
	// -1: error occured
	// 0: timed out
	// >0: data ready to be read
	return select(FD_SETSIZE, &fds, 0, 0, &timeout);

}

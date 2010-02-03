/*
 * TinyDNSResolver.h
 *
 *  Created on: Jun 2, 2009
 *      Author: luo
 */

#ifndef TINYDNSRESOLVER_H_
#define TINYDNSRESOLVER_H_

#include "../../global.h"
#include <iostream>


typedef struct {
	unsigned short id;
	unsigned short flag;
	unsigned short questions;
	unsigned short answerRRs;
	unsigned short authorityRRs;
	unsigned short additionalRRs;

} DNS_PKG_HEADER, *DNS_PKG_HEADER_PTR;

typedef struct {
	unsigned char * dns_name;
	unsigned short dns_type;
	unsigned short dns_class;
} DNS_PKG_QUERY, *DNS_PKG_QUERY_PTR;

typedef struct{
	unsigned short dns_name;
	unsigned short dns_type;
	unsigned short dns_class;
	unsigned short dns_ttl;
	unsigned char* data;
}DNS_RESPONSE_ANSWER, *DNS_RESPONSE_ANSWER_PTR;

class TinyDNSResolver {

public:
	TinyDNSResolver();
	~TinyDNSResolver();
	bool resolve(const char*, char* ip, unsigned short id);
private:

	typedef struct {
		char _domain[256];
		char _ip[16];
	} DNSCACHE;

	int pkgDNS(const char*, DNS_PKG_HEADER_PTR&, DNS_PKG_QUERY_PTR&, unsigned char*,unsigned short);
	bool recvAnalyse(unsigned char*, size_t, size_t, char*);
	/*
	 * recvfromTimeOut func is copied from :
	 * http://blog.phamansinh.com/2008/08/10/recvfrom-with-time-out-checking/
	 *
	 * I think its name is not very good, it just used select to realize time control
	 * and I have to use recvfrom again after it returns not 0 or -1
	 *
	 * I will follow GPL, though this is not required by the author
	 */
	int recvfromTimeOut(int socket, long sec, long usec);

	DNSCACHE dnscache;
	enum{DNS_SERVERS= 2	};
	static const char*  DNS_ADDR[DNS_SERVERS];

	static const int DNS_PORT;

};

#endif /* TINYDNSRESOLVER_H_ */

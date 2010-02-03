///*
// * HttpModule.cpp
// *
// *  Created on: Apr 6, 2009
// *      Author: luo
// */
//
//#include "HttpModule.h"
//
//#include "../../util/rio/rio.h"
//#include "../../global.h"
//
//HttpModule::HttpModule() {
//
//}
//
//HttpModule::~HttpModule() {
//
//}
//bool HttpModule::fetch(struct page_link pl, char* buffer, char* bigbuf, int tid) {
//
//	using namespace std;
//
//	char * domain = pl._host_addr;
//	char ip[16] = { '\0' };
//	int nbytes = 0;
//	char request[2048] = { '\0' };
//
//	if (debug)
//		cout << "domain:" << domain << endl;
//
//	if (!tr.resolve(domain, ip, tid)) {
//		if(debug)		cout << " dns resolve failed domain : "<<domain << endl;
//		return false;
//	}
//	if (debug)
//		cout << "ip:" << ip << endl;
//
//	/* 客户程序开始建立 sockfd描述符 */
//	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)/*建立SOCKET连接*/
//	{
//		fprintf(stderr, "Socket Error:%s\a\n", strerror(errno));
//		return false;
//	}
//
//	/* 客户程序填充服务端的资料 */
//	bzero(&server_addr, sizeof(server_addr));
//	server_addr.sin_family = AF_INET;
//	server_addr.sin_port = htons(pl._portnumber);
//	struct in_addr my_in_addr;
//	my_in_addr.s_addr = inet_addr(ip);
//	server_addr.sin_addr = my_in_addr;
//
//	/* 客户程序发起连接请求 */
//	if (connect(sockfd, (struct sockaddr *) (&server_addr),
//			sizeof(struct sockaddr)) == -1)/*连接网站*/
//	{
//		fprintf(stderr, "Connect Error:%s\a\n", strerror(errno));
//		close(sockfd);
//		return false;
//	}
//
//	sprintf(
//			request,
//			"GET /%s HTTP/1.1\r\nAccept: */*\r\nAccept-Language: zh-cn\r\n\
//User-Agent: Mozilla/4.0 \r\n\
//Host: %s:%d\r\nConnection: Close\r\n\r\n",
//			pl._host_file, pl._host_addr, pl._portnumber);
//	if (debug) {
//		cout << request << endl;
//	}
//	/*发送http请求request*/
//	nbytes = rio_writen(sockfd, request, strlen(request));
//	if (nbytes == -1) {
//		cout << "error occurs while sending request to server" << endl;
//		close(sockfd);
//		return false;
//	}
//	if (debug) {
//		cout << strlen(request) << " bytes send ok!" << endl;
//		cout << "\nThe following is the response header:" << endl;
//	}
//
//	/* 连接成功了，接收http响应，response */
//	//接受响应头信息
//	int i = 0;
//	int beg = 0;
//	while ((nbytes = read(sockfd, &buffer[beg], 1)) == 1) {
//		if (i < 3) {
//			if (buffer[beg] == '\r' || buffer[beg] == '\n')
//				i++;
//			else
//				i = 0;
//			if (debug) {
//				printf("%c", buffer[beg]);/*把http头信息打印在屏幕上*/
//			}
//			beg++;
//		} else if (i == 3) {//http头信息结束
//			break;
//		}
//	}
//
//	if (nbytes < 0) {//error occurs
//		cout<<"哎呀，破服务器你想害我？！"<<endl;
//		close(sockfd);
//		return false;
//	}
//	//接受http主体信息 只读入512k，如果大于512k丢弃
//	cout<<"3"<<endl;
//	nbytes = rio_readn(sockfd, bigbuf, MAX_BUFFER);
//	cout<<"4"<<endl;
//	if (nbytes == 0 || nbytes == -1) {
//		if (debug)
//			cout << "error occurs while receiving from server now nbytes is"
//					<< nbytes << " host: " << pl._host_addr << "  file: "
//					<< pl._host_file << "  port : " << pl._portnumber << endl;
//		close(sockfd);
//		return false;
//	} else if (nbytes >= MAX_BUFFER) {
//		if (debug)
//			cout << "!file is too large" << endl;
//		close(sockfd);
//		return false;
//	}
//
//	bigbuf[MAX_BUFFER] = '\0';
//	close(sockfd);
//	return true;
//}

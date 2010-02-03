/*
 * HttpModule.h
 *
 *  Created on: Apr 6, 2009
 *      Author: luo
 */

#ifndef HTTPMODULE_H_
#define HTTPMODULE_H_

#include "../../global.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
#include "../DNS_Resolver/TinyDNSResolver.h"
#include "../../util/thread_print/thread_print.h"
#include "../../util/rio/rio.h"
#include <time.h>
#include <sys/time.h>

class HttpModule {
public:
	HttpModule(int);
	~HttpModule();
	/*
	 * fetch创建子线程时调用pthread_detach由操作系统负责回收自线程
	 * 终止后的资源
	 * fetch通过条件等待30秒如果超时则调用pthread_cancel终止子线程
	 * 未超时则条件等待被唤醒继续执行
	 */
	bool fetch(struct page_link pl, char* buffer, char* bigbuf, int tid);

	/*
	 * 因为c＋＋函数符号表的问题，我不得不用一个全局的包装方法来包装
	 * threadtofetch郁闷阿
	 * 详见http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.7
	 * http://cboard.cprogramming.com/cplusplus-programming/113981-passing-class-member-function-pthread_create.html
	 * http://www.cpptalk.net/how-to-pass-a-class-member-function-to-pthreadcreate-vt44306.html
	 * 3篇文章
	 * 全局的封装方法为
	 void* threadtofetch_global(void*)

	 template<class T, void(T::*mem_fn)()>
	 void* LaunchMemberFunction(void* obj)
	 {
	 (static_cast<T*>(p)->*mem_fn)(obj);
	 return 0;
	 }
	 *
	 */
	void* threadtofetch(void*);

	char* getIP();

private:
	int tid;
	struct sockaddr_in server_addr;
	int sockfd;
	TinyDNSResolver tr;
//	DnsResolver dns;
	enum {
		NOTRETURN,//线程还没有返回结果
		RETURN_TRUE,//线程工作结束并返回true
		RETURN_FALSE//线程工作结束返回false
	};

	pthread_mutex_t mutex_fetch;
	pthread_cond_t cond_fetch;
#ifdef DEBUG
	Thread_print tp;//线程打印到文件
#endif

	char ip[16];
};


//HttpModule类中用到的结构
struct threadPara {
	int tid;
	struct page_link pl;
	char* buffer;
	char* bigbuf;
	int flag;
	/* flag用于标识线程是否已经开始执行，避免开出的线程未获得机会运行
	 * 主线程就已经结束
	 * 通过对flag标记的判断可以保证子线程真的已经运行
	 * (p_flag)的值是枚举中的一个
	 */
};
struct Wrapper_t {
	struct threadPara* tp;
	HttpModule* hm;
};
void* thread_LaunchMemberFunction(void*);
void alrm_handler(int);
#endif /* HTTPMODULE_H_ */

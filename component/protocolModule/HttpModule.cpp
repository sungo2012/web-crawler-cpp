/*
 * HttpModule.cpp
 *
 *  Created on: Apr 6, 2009
 *      Author: luo
 */

#include "HttpModule.h"

#include "../../util/rio/rio.h"
#include "../../global.h"

#include <signal.h>

HttpModule::HttpModule(int id) :
	tid(id)
#ifdef DEBUG
	, tp(id)
#endif
{

	pthread_mutex_init(&mutex_fetch, NULL);
	pthread_cond_init(&cond_fetch, NULL);

	memset(ip, 0, sizeof(ip));
}

HttpModule::~HttpModule() {

	pthread_mutex_destroy(&mutex_fetch);
	pthread_cond_destroy(&cond_fetch);
}
char* HttpModule::getIP(){
	return ip;
}

bool HttpModule::fetch(struct page_link pl, char* buffer, char* bigbuf, int tid) {

	using namespace std;

	pthread_t thread;
	struct threadPara thread_para;
	int res = 0;
	struct timespec m_time;

	thread_para.pl = pl;
	thread_para.buffer = buffer;
	thread_para.bigbuf = bigbuf;
	thread_para.flag = NOTRETURN;
	thread_para.tid = tid;

	struct Wrapper_t wt;
	wt.tp = &thread_para;
	wt.hm = this;

	//建立信号捕捉
	struct sigaction actions;
	memset(&actions, 0, sizeof(actions));
	sigemptyset(&actions.sa_mask);
	actions.sa_flags = 0;
	actions.sa_handler = alrm_handler;
	int sigrc = sigaction(SIGALRM, &actions, NULL);
	if (sigrc) {
		printf("sigaction error!\n");
		exit(-1);
	}

	/* 客户程序开始建立 sockfd描述符 */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)/*建立SOCKET连接*/
	{
		fprintf(stderr, "Socket Error:%s\a\n", strerror(errno));

		return false;
	}

	int rc = pthread_create(&thread, NULL, thread_LaunchMemberFunction,
			(void*) (&wt));
	pthread_detach(thread);//系统负责回收线程资源
	if (rc) {
		perror("error: fetch :pthread_create()");
		close(sockfd);
		return false;
	}
	m_time.tv_sec = time(NULL) + 60;
	m_time.tv_nsec = 0;

	pthread_mutex_lock(&mutex_fetch);
	while (thread_para.flag == NOTRETURN && res != ETIMEDOUT) {
#ifdef DEBUG
tp.print("＠抓取线程进入等待！\n");
#endif

		res = pthread_cond_timedwait(&cond_fetch, &mutex_fetch,
				(const struct timespec *) &m_time);

#ifdef DEBUG
tp.print("tid: %d ＠抓取线程结束等待！\n", tid);
#endif
	}
	pthread_mutex_unlock(&mutex_fetch);

	if (thread_para.flag == RETURN_TRUE) {
#ifdef DEBUG
tp.print("抓取线程成功返回true\n");
#endif
		close(sockfd);
		return true;
	} else if (thread_para.flag == RETURN_FALSE) {
#ifdef DEBUG
tp.print("抓取线程返回false\n");
#endif
		close(sockfd);
		return false;
	} else if (res == ETIMEDOUT) {//如果抓取线程超时
#ifdef DEBUG
if ((res = pthread_kill(thread, SIGALRM)) == 0) {
	tp.print("＠抓取线程超时,pthread_cancle success!\n");

} else {
	tp.print("＠抓取线程超时,pthread_cancle failed!\n");

}
#endif

		close(sockfd);
		return false;
	} else {

#ifdef DEBUG
tp.print(
"error:fetch 遇到未预料情况 将返回false thread_para.flap: %d  res: %d\n",thread_para.flag, res);
#endif
		close(sockfd);
		return false;
	}

}

void* HttpModule::threadtofetch(void* p_attr) {

	using namespace std;

	struct threadPara * pt = (struct threadPara *) p_attr;

	char * domain = pt->pl._host_addr;

	int nbytes = 0;
	char request[2048] = { '\0' };
#ifdef DEBUG
tp.print("domain:%s\n", domain);
#endif

	if (!tr.resolve(domain, ip, pt->tid)) {
#ifdef DEBUG
tp.print(" dns resolve failed domain : %s\n", domain);
#endif
		pt->flag = RETURN_FALSE;
		pthread_cond_signal(&cond_fetch);
		return NULL;
	}
#ifdef DEBUG
tp.print("ip: %s\n", ip);
#endif
	/* 客户程序填充服务端的资料 */
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(pt->pl._portnumber);
	struct in_addr my_in_addr;
	my_in_addr.s_addr = inet_addr(ip);
	server_addr.sin_addr = my_in_addr;

	/* 客户程序发起连接请求 */
	if (connect(sockfd, (struct sockaddr *) (&server_addr),
			sizeof(struct sockaddr)) == -1)/*连接网站*/
	{
#ifdef DEBUG
tp.print("Connect Error:%s\n", strerror(errno));
#endif
		pt->flag = RETURN_FALSE;
		pthread_cond_signal(&cond_fetch);
		return NULL;
	}

	sprintf(
			request,
			"GET /%s HTTP/1.1\r\nAccept: */*\r\nAccept-Language: zh-cn\r\n\
User-Agent: Mozilla/4.0 \r\n\
Host: %s:%d\r\nConnection: Close\r\n\r\n",
			pt->pl._host_file, pt->pl._host_addr, pt->pl._portnumber);
#ifdef DEBUG
tp.print("%s\n", request);
#endif

	/*发送http请求request*/
	nbytes = rio_writen(sockfd, request, strlen(request));
	if (nbytes == -1) {
#ifdef DEBUG
tp.print("Connect Error:%s\n", strerror(errno));
#endif
		pt->flag = RETURN_FALSE;
		pthread_cond_signal(&cond_fetch);
		return NULL;
	}
#ifdef DEBUG
tp.print("%d bytes send ok!\n", strlen(request));
tp.print("\nThe following is the response header:\n");
#endif


	/* 连接成功了，接收http响应，response */
	//接受响应头信息
	int i = 0;
	int beg = 0;
	while ((nbytes = read(sockfd, &(pt->buffer[beg]), 1)) == 1) {
		if (i < 3) {
			if (pt->buffer[beg] == '\r' || pt->buffer[beg] == '\n')
				i++;
			else
				i = 0;
#ifdef DEBUG
tp.print("%c", pt->buffer[beg]);/*把http头信息打印在屏幕上*/
#endif

			beg++;
			if (beg >= (DMAX_HEAD - 1)) {//http头过长放弃掉
				pt->flag = RETURN_FALSE;
				pthread_cond_signal(&cond_fetch);
				return NULL;
			}
		} else if (i == 3) {//http头信息结束
			break;
		}
	}
	if (nbytes < 0) {//error occurs
#ifdef DEBUG
tp.print("哎呀，破服务器你想害我？！\n");
#endif

		pt->flag = RETURN_FALSE;
		pthread_cond_signal(&cond_fetch);
		return NULL;
	}
	//接受http主体信息 只读入512k，如果大于512k丢弃, 区分是否为chunked交由不同函数处理。

        if(strstr(pt->buffer,"Transfer-Encoding: chunked")!=NULL)
            nbytes = rio_readn_chunked(sockfd,pt->bigbuf, MAX_BUFFER);
        else
	    nbytes = rio_readn(sockfd, pt->bigbuf, MAX_BUFFER);

	if (nbytes == 0 || nbytes == -1) {
#ifdef DEBUG
tp.print("error occurs while receiving from server now nbytes is%d, host: %s,   file: %s   port : %d\n",nbytes, pt->pl._host_addr, pt->pl._host_file,pt->pl._portnumber);
#endif

		pt->bigbuf[MAX_BUFFER] = '\0';
		pt->flag = RETURN_FALSE;
		pthread_cond_signal(&cond_fetch);
		return NULL;
	} else if (nbytes >= MAX_BUFFER) {
#ifdef DEBUG
tp.print("!file is too large\n");
#endif
		pt->bigbuf[MAX_BUFFER] = '\0';
		pt->flag = RETURN_FALSE;
		pthread_cond_signal(&cond_fetch);
		return NULL;
	}

	pt->bigbuf[MAX_BUFFER] = '\0';
	pt->flag = RETURN_TRUE;
	pthread_cond_signal(&cond_fetch);
	return NULL;
}

void* thread_LaunchMemberFunction(void*obj) {

	struct Wrapper_t* wt = (Wrapper_t*) obj;

	(reinterpret_cast<HttpModule *> (wt->hm))->threadtofetch(wt->tp);
	pthread_exit(NULL);
}

void alrm_handler(int signo) {

	pthread_exit(0);
}

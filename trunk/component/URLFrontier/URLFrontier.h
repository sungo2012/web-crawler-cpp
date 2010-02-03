/*
 * URLFrontier.h
 *
 *  Created on: Apr 1, 2009
 *      Author: luo
 *
 *      the default version of  URL frontier is actually implemented by a collection
 *      of distinct FIFO subqueues.There are two important aspects to how URLs are added
 *      to and removed from these queues. First, there is one FIFO subqueue per worker thread.
 *      That is, each worker thread removes URLs from exactly one of the FIFO subqueues.
 * 		Second, when a new URL is added, the FIFO subqueue in which it is placed is determined
 * 		by the URL’s canonical host name Together, these two points imply that at most one worker
 * 		thread will download documents from a given Web server. This design prevents Mercator from
 * 		overloading a Web server, which could otherwise become a bottleneck of the crawl.
 *
 *		In actual World Wide Web crawls, the size of the crawl’s frontier numbers in the hundreds of millions of URLs.
 *		Hence, the majority of the URLs must be stored on disk. To amortize the cost of reading from and writing to disk,
 *		our FIFO subqueue implementation maintains fixed-size enqueue and dequeue buffers in memory; our current
 *		our implementation uses buffers that can hold 800 URLs each.
 *
 *		Q:when a new URL is added, the FIFO subqueue in which it is placed is determined by the URL’s canonical host name
 *		A：全局所有URLFrontier,用一个内部tid表明所属线程，每个URLFrontier自己保存一个队列，内部实现锁机制，URL用MOD方法分配到
 *		URLFrontier中最多维护800个连接如果大于800，写入600个到文件中，如果队列为空向文件中取400
 *		为了减少io操作，在每个队列文件的最前面用2进制写一个long类型，用来表示当前的偏移量（从第几个结构开始读），这样省去了删除文件的麻烦
 */

#ifndef URLFRONTIER_H_
#define URLFRONTIER_H_

#include "../../global.h"
#include "../../util/thread_print/thread_print.h"
#include <pthread.h>
#include <queue>
#include <fstream>
#include <string.h>

class URLFrontier {
private:
	//id of thread the class URLFrontier belongs to
	int tid;

//	std::fstream dataFile;

	std::fstream file_out;

	std::fstream file_in;

	//offset of current struct from the file
	long long cur;
	//num of struct in file
	long long end;

	pthread_mutex_t qlock;
	pthread_mutex_t flock;

	std::queue<struct page_link> url_queue;
	//for queue
	void getLock();
	void releaseLock();
	//for file
	void getFileLock();
	void releaseFileLock();
	bool write2file();
	bool getFromFile();
#ifdef DEBUG
	Thread_print tp;
#endif
public:

	URLFrontier(int id);
	~URLFrontier();

	bool is_empty();
	//add url to queue
	bool addURL(struct page_link*);
	//read the first one in the queue, or in file as the queue is empty
	bool readURL(struct page_link*);
	// is there object avaliable
	bool available();

};

#endif /* URLFRONTIER_H_ */

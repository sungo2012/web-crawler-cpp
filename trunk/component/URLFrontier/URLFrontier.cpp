/*
 * URLFrontier.cpp
 *
 *  Created on: Apr 1, 2009
 *      Author: luo
 */
#include "URLFrontier.h"

#include <string.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

URLFrontier::URLFrontier(int id) :
	tid(id)
#ifdef DEBUG
	, tp(id) 
#endif
{

	string filename("./QueueFiles/file");
	filename += integer2string(tid) + ".url";

	cur = 0LL;
	end = 0LL;

	//	dataFile.open(filename.c_str(), ios_base::out | ios_base::trunc
	//			| ios_base::binary);
	//	if (!dataFile.is_open()) {
	//		cout << "error: cant open " + filename + " to write in constructor"
	//				<< endl;
	//		cout << strerror(errno) << endl;
	//	}
	//	//dataFile.write(reinterpret_cast<char *> (&cur), sizeof(long)) << flush;
	//
	//	dataFile.close();

	file_out.open(filename.c_str(), ios_base::out | ios_base::trunc);
	if (!file_out.is_open()) {
		printf("urlfroniter can't open filename = %s\n", filename.c_str());
		exit(-1);
	}

	file_in.open(filename.c_str(), ios_base::in);
	if (!file_in.is_open()) {
		printf("urlfrontier can't open filename = %s\n", filename.c_str());
		exit(-1);
	}

	pthread_mutex_init(&qlock, NULL);
	pthread_mutex_init(&flock, NULL);

}

URLFrontier::~URLFrontier() {

	pthread_mutex_destroy(&qlock);
	pthread_mutex_destroy(&flock);

}
//private::
void URLFrontier::getLock(void) {

	pthread_mutex_lock(&qlock);

}

void URLFrontier::releaseLock() {

	pthread_mutex_unlock(&qlock);
}

void URLFrontier::getFileLock() {

	pthread_mutex_lock(&flock);
}
void URLFrontier::releaseFileLock() {

	pthread_mutex_unlock(&flock);
}

bool URLFrontier::write2file() {
	using namespace std;

	getFileLock();

	std::string filename("./QueueFiles/file");
	filename += integer2string(tid) + ".url";
#ifdef DEBUG
tp.print("%s\n", filename.c_str());
#endif

	getLock();
	if (url_queue.size() >= (unsigned int) MAX_URL) {

		for (int i = 0; i < NUM_URL2FILE; i++) {

			struct page_link first = url_queue.front();

			file_out.write(reinterpret_cast<char *> (&first),
					sizeof(struct page_link));
			end++;

			url_queue.pop();

		}
		file_out.flush();

	} else {
		//do nothing
	}
	releaseLock();

	releaseFileLock();
	return true;
}

bool URLFrontier::getFromFile() {

	//long offset = 0;

	getFileLock();
	if (end - cur == 0) {//文件中没有可读url
		releaseFileLock();
		return false;
	}
	long long cal = cur;

	//dataFile.read(reinterpret_cast<char*> (&offset), sizeof(long));

	getLock();
	if (url_queue.empty() == true) {
		file_in.seekg(sizeof(struct page_link) * cur, ios::beg);//跳过已经读出的
		struct page_link* tmppl = new struct page_link;
		//从文件中读指定个数的url到内存中
		//如果文件中没有足够数量的url，那么读出所有文件中的url
		for (int i = 0; i < NUM_URL2MEMERY && file_in.read(
				reinterpret_cast<char*> (tmppl), sizeof(struct page_link)); i++) {

			url_queue.push(*tmppl);

			cur++;

		}
		delete tmppl;

	} else {//队列不为空直接返回
		//do nothing
	}
	releaseLock();

	//dataFile.seekp(0, ios_base::beg);
	//dataFile.write(reinterpret_cast<char*> (&offset), sizeof(long));
	file_in.flush();

	releaseFileLock();

	if (cur - cal > 0)//证明确实从文件中读取了结构体
		return true;
	else
		//文件已经为空
		return false;
}

//public::
bool URLFrontier::addURL(struct page_link* pl) {

	if (url_queue.size() >= (unsigned int) MAX_URL) {
#ifdef DEBUG
tp.print("@@url has to write to file");
#endif

		write2file();
	}

	getLock();
	url_queue.push(*pl);
	releaseLock();
#ifdef DEBUG
tp.print("report:add url===================================================================================url in queue:%d\n",url_queue.size());
#endif

	return true;
}

bool URLFrontier::readURL(struct page_link* result) {

	struct page_link tmp;
#ifdef DEBUG
tp.print("<<<<<<<<<<<<<<<<>>>>cur%d>>>>>>>>>>>>>>>>>\n",cur);
tp.print("<<<<<<<<<<<<<<<<>>>>end%d>>>>>>>>>>>>>>>>>\n",end);
#endif

	if (!url_queue.empty()) {

		getLock();
		if (!url_queue.empty()) {//队列不为空
			tmp = url_queue.front();
			strncpy(result->_host_addr, tmp._host_addr, 255);
			result->_host_addr[255] = '\0';
			strncpy(result->_host_file, tmp._host_file, 1023);
			result->_host_file[1023] = '\0';
			result->_portnumber = tmp._portnumber;
			url_queue.pop();

			releaseLock();
			return true;
		} else {//此时队列已经为空

			releaseLock();
			return false;
		}

	} else {

		if (getFromFile()) {

			getLock();
			tmp = url_queue.front();

			strncpy(result->_host_addr, tmp._host_addr, 255);
			result->_host_addr[255] = '\0';
			strncpy(result->_host_file, tmp._host_file, 1023);
			result->_host_file[1023] = '\0';
			result->_portnumber = tmp._portnumber;
			url_queue.pop();
			releaseLock();
#ifdef DEBUG
tp.print("report:read url===================================================================================url in queue:%d\n",url_queue.size());
#endif

		} else {
			return false;
		}
	}

	return true;
}

bool URLFrontier::is_empty() {
	return url_queue.empty();
}

bool URLFrontier::available() {

	if (!url_queue.empty())
		return true;
	else {//queue is empty
		if (end - cur > 0)
			return true;
		else if (end - cur == 0)
			return false;
		else if (end - cur < 0) {
			cerr << "error:end - cur < 0 in frontier" << endl;
			exit(-1);
		}
	}
}

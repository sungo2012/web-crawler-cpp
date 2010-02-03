/*
 * global.h
 *
 *  Created on: Apr 1, 2009
 *      Author: luo
 *
 *      定义可爱的全局变量
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_
#include <sstream>
#include <vector>

#include "./util/RabinHash/rabinhash64.h"


#define DEBUG

struct page_link {
	char _host_addr[256];
	char _host_file[1024];
	int _portnumber;
};

struct url_hash {
	long long host_hash;
	long long file_hash;
};



//number of max thread
extern const int MAX_THREAD;

//max urls in queue
extern const int MAX_URL;
//num url to file
extern const int NUM_URL2FILE;
//if queue is empty ,num url from file to memery
extern const int NUM_URL2MEMERY;
//max buffer to hold the file that is received from server
extern const int MAX_BUFFER;
extern const int MAX_BUFFERPlus1;
//512k
#define DMAX_BUFFER  (512*1024-1)
#define DMAX_BUFFERPlus1  (512*1024)
//max buffer to hold the http head;
extern const int MAX_HEAD;
extern const int MAX_HEADPlus1;
#define DMAX_HEAD 512
//max doc hash the ContentSeen hold per thread
extern const int MAX_DOC;
//hash seed
extern const int HASH_SEED;
//how many items LRU cache hold
extern const int MAX_LRU;

extern RabinHashFunction64 rabin_global;

//debug?
extern bool debug;

//int to string
std::string integer2string(const int n);
//you must free yourself
char* newStrChangeC(const char*p, char from, char to, int n);

void ToLowerCase(char * str);

void initURLFriontier();

void initURLContentSeen();

void initURLSeen();

//这个函数和urlseen 的long long hashurl(struct page_link*);等效
long long hash_site(struct page_link*);

extern int bughunterc;
extern int bughunterf;

extern const char* amp; //康维鹏提出的bug：＆amp;转移成＆


extern int pagecounter ;
extern pthread_mutex_t mutex_counter;

//线程分配锁
extern pthread_mutex_t mutex_assign_thread;
extern int assignmode;
extern int assign_counter ;


#endif /* GLOBAL_H_ */

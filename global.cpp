/*
 * global.cpp
 *
 *  Created on: Apr 1, 2009
 *      Author: luo
 */

#include "global.h"
#include <iostream>
#include <string.h>
#include <ctype.h>

using namespace std;

const int MAX_THREAD = 1;

const int MAX_URL = 1000;

const int NUM_URL2FILE = 900;

const int NUM_URL2MEMERY = 100;

const int MAX_BUFFER = 512* 1024 - 1 ;
const int MAX_BUFFERPlus1 = MAX_BUFFER + 1;

const int MAX_HEAD = 512 - 1;
const int MAX_HEADPlus1 = MAX_HEAD + 1;
const int MAX_DOC = 800;

const int HASH_SEED = 1;

const int MAX_LRU = 150;

bool debug = false;

const char* amp="&amp;";


int pagecounter = 0;
pthread_mutex_t mutex_counter;

//线程分配锁
pthread_mutex_t mutex_assign_thread;
int assignmode = 0, assign_counter = 0;

std::string integer2string(const int n) {

	//	std::stringstream newstr(std::stringstream::in | std::stringstream::out);
	std::stringstream newstr;
	if (!(newstr << n)) {
		return "";
	}
	return newstr.str();

}

char* newStrChangeC(const char*p, char from, char to, int n) {
	using namespace std;
	int i;
	const char* tmp;
	char* result = new char[n];
	memset(result, 0, n);

	for (tmp = p, i = 0; (*tmp) && i < n; tmp++, i++) {

		if (*tmp == from)
			result[i] = to;
		else
			result[i] = *tmp;

	}

	return result;
}

/********************************************
 功能：把字符串转换为全小写
 ********************************************/
void ToLowerCase(char * str) {
	int i;
	i = 0;
	while (str[i] != '\0') {
		str[i] = tolower(str[i]);
		i++;
	}
}

int bughunterc = 0;
int bughunterf = 0;

RabinHashFunction64 rabin_global(HASH_SEED);
//这个函数和urlseen 的long long hashurl(struct page_link*);等效
long long hash_site(struct page_link* ppl) {



	long long v, v1, v2;
	v1 = rabin_global.hash(ppl->_host_addr, 256);
	v2 = rabin_global.hash(ppl->_host_file, 1024);

	v = (v1 << 32) + v2;
	return v;
}




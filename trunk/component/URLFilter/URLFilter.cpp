/*
 * URLFilter.cpp
 *
 *  Created on: Apr 9, 2009
 *      Author: luo
 */

#include "URLFilter.h"
#include <string.h>

#include <stdlib.h>
#include <errno.h>

URLFilter::URLFilter(int id) :
	filename("./URLFilter/urlfilter.txt"), re_counter(0), tid(id)
#ifdef DEBUG
	, tp(id)
#endif
{

	memset(_url, 0, sizeof(_url));
	memset(regex, 0, sizeof(regex));

	fstream fp;
	fp.open(filename.c_str(), ios_base::in);
	if (!fp.is_open()) {
		printf("filefileter::compilereg error: %d\n", strerror(errno));
		exit(-1);
	}

	size_t len; /* store error message length */
	char errbuf[128]; /* store error message */
	int err;
/*
 * 一次性把过滤文件的正则表达式编译好
 */
	for (int i = 0; i < 20 && fp.getline(regex, 1024); i++) {
		//		cout << regex << endl;
		re_counter++;
		bzero(&re_f[i], sizeof(regex_t));
		err = regcomp(&re_f[i], regex, REG_EXTENDED | REG_ICASE);

		if (err) {
			len = regerror(err, &re_f[i], errbuf, sizeof(errbuf));
			fprintf(stderr, "urlfilter:error: regcomp: %s\n", errbuf);

			fp.close();

			exit(-1);
		}
	}

	fp.close();

}

URLFilter::~URLFilter() {

	for (int i = 0; i < re_counter; i++) {
		regfree(&re_f[i]);
	}

}


bool URLFilter::fill_url(struct page_link *plPtr) {

	if (plPtr == NULL)
		return false;

	char* p = _url;
	memset(_url, 0, sizeof(_url));

	strncpy(_url, plPtr->_host_addr, 256);
	p += strlen(plPtr->_host_addr);
	*p = ':';

	strcat(_url, integer2string(plPtr->_portnumber).c_str());
	p += strlen(integer2string(plPtr->_portnumber).c_str()) + 1;
	*p = '/';
	strncat(_url, plPtr->_host_file, 1024);
	_url[2047]= '\0';
	return true;

}
/*
 * true 表示被过滤
 * false 表示未被过滤
 */
bool URLFilter::filter(struct page_link* plPtr) {

	if (!fill_url(plPtr)) {

		return true;
	}

//	size_t len; /* store error message length */
	regmatch_t subs[10]; /* store matched string position */
//	char matched[1024]; /* store matched strings */

	int err;
		if(debug)printf("tid %d re_counter %d\n", tid, re_counter);
	for (int i = 0; i < re_counter; i++) {
		err = regexec(&re_f[i], _url, (size_t) 10, subs, 0);
		if (err != REG_NOMATCH) {
#ifdef DEBUG
			tp.print("\nOK, has matched ...\n\n");
#endif

			return false;
		} else if(err == REG_NOMATCH){

#ifdef DEBUG
tp.print("URLFilter:url:%s passed ...\n\n", _url);
#endif


		}else{//正则表达式匹配出错丢弃网址
#ifdef DEBUG
tp.print("正则表达式匹配出错丢弃网址\n");
tp.print("错误代码是%d\n", err);
#endif


			return true;
		}

	}
#ifdef DEBUG
tp.print("filter return\n");
#endif

	return true;
}

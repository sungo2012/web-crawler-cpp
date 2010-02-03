/*
 * URLFilter.h
 *
 *  Created on: Apr 9, 2009
 *      Author: luo
 */

#ifndef URLFILTER_H_
#define URLFILTER_H_

#include "../../global.h"
#include "../../util/thread_print/thread_print.h"
#include <fstream>
#include <iostream>
#include <string>
#include <regex.h>
using namespace std;
class URLFilter {

public:
	URLFilter(int);
	~URLFilter();
	bool filter(struct page_link*);//true 表示连接被过滤， false表示通过了过滤

private:
	enum{MAX_FILTER=20};
	string  filename;
	char         _url[2048];
	char         regex[1024];
	int			 re_counter;
	regex_t re_f[MAX_FILTER];
	bool fill_url(struct page_link*);

	int tid;
#ifdef DEBUG
	Thread_print tp;
#endif

};

#endif /* URLFILTER_H_ */

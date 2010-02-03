/*
 * LinkExtractor.h
 *
 *  Created on: Apr 9, 2009
 *      Author: luo
 */

#ifndef LINKEXTRACTOR_H_
#define LINKEXTRACTOR_H_
#include <regex.h>
#include "../../global.h"

class LinkExtractor {

public:

	LinkExtractor();
	~LinkExtractor();
	bool initLinkExtractor(struct page_link* ,char*, int);//初始化
	bool clearself();//针对每一个网页提取完所有超链接后必须释放
	bool getLink(struct page_link* pl) ;//提取一个超链接并封装到结构中
	bool URLanalyser(struct page_link*, char*);//针对一个给定的绝对url封装到结构中

private:

	char* currPtr;//指向存放网页的内存
	int num;//内存大小
	struct page_link* currPage_link;
	bool fixURL(struct page_link*plptr, char*url);//针对一个url（可能是相对的也可能是绝对的），正确封装到结构中
	inline int min(int a , int b){return a>b? b: a;}

	regex_t regex;
};

#endif /* LINKEXTRACTOR_H_ */

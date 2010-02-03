/*
 * LinkExtractor.cpp
 *
 *  Created on: Apr 9, 2009
 *      Author: luo
 */

#include "LinkExtractor.h"
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <cstdio>

LinkExtractor::LinkExtractor() {

	char errbuf[128]; //* store error message
	int err;
	int len;

	char pattern[] =
			"<a\\s+href=[\"|\']?([^>\"\' ]+)[\"|\']?\\s*[^>]*>([^>]+)</a>";//超连接的正则表达式

	bzero(&regex, sizeof(regex_t));
	err = regcomp(&regex, pattern, REG_EXTENDED | REG_ICASE);//compile regex

	if (err) {
		len = regerror(err, &regex, errbuf, sizeof(errbuf));
		fprintf(stderr, "error: regcomp: %s\n", errbuf);
		exit(-1);
	}
}

LinkExtractor::~LinkExtractor() {
	regfree(&regex);
}

bool LinkExtractor::initLinkExtractor(struct page_link* plPtr, char* p,
		int size) {

	currPtr = p;
	num = size;
	currPage_link = plPtr;

	return true;

}

bool LinkExtractor::clearself() {

	currPtr = NULL;
	num = 0;
	currPage_link = NULL;

	return true;

}
bool LinkExtractor::URLanalyser(struct page_link* plPtr, char* src) {

	char* pA;
	char* pB;

	if (!(*src))
		return false;
	ToLowerCase(src);
	if ((pA = strstr(src, "javascript")) != NULL) {
		return false;
	}
	pA = src;
	memset(plPtr, 0, sizeof(struct page_link));

	if (!strncmp(pA, "http://", strlen("http://")))
		pA = src + strlen("http://"); //remove "http://" from pA
	else if (!strncmp(pA, "https://", strlen("https://")))
		pA = src + strlen("https://"); //remove "https://" from pA

	pB = strchr(pA, '/');
	if (pB) {
		memcpy(plPtr->_host_addr, pA, min((strlen(pA) - strlen(pB)), 255));

		if (pB + 1) {
			memcpy(plPtr->_host_file, pB + 1, strlen(pB) - 1);
			plPtr->_host_file[strlen(pB) - 1] = 0;
		} else {
			*(plPtr->_host_file) = '\0';
		}
	} else {
		memcpy(plPtr->_host_addr, pA, min(strlen(pA), 255));

		*(plPtr->_host_file) = '\0';
	}

	if (pB && ((strlen(pA) - strlen(pB)) < 256))
		plPtr->_host_addr[strlen(pA) - strlen(pB)] = 0;
	else if (strlen(pA) < 256)
		plPtr->_host_addr[strlen(pA)] = 0;
	else
		plPtr->_host_addr[255] = '\0';

	pB = pA;
	pA = strchr(plPtr->_host_addr, ':');
	if (pA) {
		plPtr->_portnumber = atoi(pA + 1);
		*pA = 0; //去除在web数组中的端口号保留主机名
	} else
		plPtr->_portnumber = 80;

	return true;

}

bool LinkExtractor::getLink(struct page_link* pl) {

	size_t len; //* store error message length

	regmatch_t subs[10]; //* store matched string position
	char matched[1024]; //* store matched strings

	int err;

	err = regexec(&regex, currPtr, (size_t) 10, subs, 0);//execute regex


	if (err == REG_NOMATCH) {

		return false;
	} else if (err) {

		return false;
	}
	//只有err == 0才能进入到下面
	if (subs[1].rm_so == -1) {

		return false;
	}
	len = subs[1].rm_eo - subs[1].rm_so;
	if (len > 1023)
		len = 1023;
	memset(matched, 0, sizeof(matched));
	memcpy(matched, currPtr + subs[1].rm_so, len);
	matched[len] = '\0';//现在match数组已经填充了匹配正则表达式的URL
	currPtr = currPtr + subs[0].rm_eo;

	return fixURL(pl, matched);

}

bool LinkExtractor::fixURL(struct page_link* plPtr, char* url) {
	struct page_link* pltmp = new struct page_link;

	strncpy(pltmp->_host_addr, currPage_link->_host_addr, 256);
	strncpy(pltmp->_host_file, currPage_link->_host_file, 1024);
	pltmp->_portnumber = currPage_link->_portnumber;

	char* urlPtr = url;
	char* tmp;
	if ((*urlPtr) != '.') {
		if ((!strncmp(urlPtr, "http://", strlen("http://"))) || (!strncmp(
				urlPtr, "https://", strlen("https://")))) {//认为是绝对url
			delete pltmp;
			return URLanalyser(plPtr, url);
		} else if ((*urlPtr) == '/') {
			urlPtr += 1;
			strncpy(plPtr->_host_addr, pltmp->_host_addr, 255);
			plPtr->_host_addr[255] = '\0';
			strncpy(plPtr->_host_file, urlPtr, 1023);
			plPtr ->_host_file[1023] = '\0';
			plPtr->_portnumber = pltmp->_portnumber;
			delete pltmp;
			return true;
		} else {//同上
			strncpy(plPtr->_host_addr, pltmp->_host_addr, 255);
			plPtr->_host_addr[255] = '\0';
			strncpy(plPtr->_host_file, urlPtr, 1023);
			plPtr ->_host_file[1023] = '\0';
			plPtr->_portnumber = pltmp->_portnumber;
			delete pltmp;
			return true;
		}
	}
	while ((*urlPtr) == '.') {
		urlPtr += 1;
		//遇到"./"
		if ((*urlPtr) == '/') {
			urlPtr += 1;
			tmp = strrchr(pltmp->_host_file, '/');
			if (tmp != NULL) {
				tmp += 1;
				//把/后面的文件名清0
				for (char* tp = tmp; *tp; tp++) {
					*tp = 0;
				}
			} else {
				tmp = pltmp->_host_file;
				//把后面的文件名清0
				for (char* tp = tmp; *tp; tp++) {
					*tp = 0;
				}
			}
		}
		//遇到"../"
		else if ((*urlPtr) == '.' && (*(urlPtr + 1)) == '/') {
			urlPtr += 2;
			//找到倒数第二个／
			tmp = strrchr(pltmp->_host_file, '/');
			if (tmp != NULL) {
				*tmp = 0;
				tmp += 1;
				//把/后面的文件名清0
				for (char* tp = tmp; *tp; tp++) {
					*tp = 0;
				}
				tmp = strrchr(pltmp->_host_file, '/');
			} else {
				delete pltmp;
				return false;
			}
			if (tmp == NULL) {
				tmp = pltmp->_host_file;
				//把后面的文件名清0
				for (char* tp = tmp; *tp; tp++) {
					*tp = 0;
				}
			} else {
				tmp += 1;
				//把/后面的文件名清0
				for (char* tp = tmp; *tp; tp++) {
					*tp = 0;
				}
			}
		}
		//遇到不可预料情况
		else {
			delete pltmp;
			return false;
		}
	}
	//处理完./或../后处理余下部分
	strcpy(plPtr->_host_addr, pltmp->_host_addr);
	char* tmpchar = new char[1024];
	memset(tmpchar, 0, 1024);
	strncpy(tmpchar, pltmp->_host_file, 1023);
	int nnn = 1023 - strlen(pltmp->_host_file);
	if (nnn < 0) {
		delete[] tmpchar;
		delete pltmp;
		return false;
	}
	strncat(tmpchar, urlPtr, nnn);
        /*
         * 处理康维朋提出的bug：
         * 不能正常处理&amp;转义为＆的情况
         */
        char* p_amp = tmpchar;
        while((p_amp=strstr(p_amp, amp)) != NULL)
        {
            std::copy(p_amp+strlen(amp), p_amp+strlen(p_amp)+1, p_amp+1);// p_amp+strlen(p_amp)+1是为了保持\0, p_amp+1是为了保存原有的&
            p_amp += strlen(amp);
        }


        strncpy(plPtr->_host_file, tmpchar, 1023);
        plPtr->_portnumber = pltmp->_portnumber;
        delete[] tmpchar;
        delete pltmp;
        return true;
}

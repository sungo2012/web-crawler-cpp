/*
 * Basic_LRU.cpp
 *
 *  Created on: Jun 6, 2009
 *      Author: luo
 */

#include "Basic_LRU.h"

Basic_LRU::Basic_LRU(int num_cache) :
	max_cache(num_cache) {

	head_pointer = NULL;
	tail_pointer = NULL;
	curr_num = 0;
}

Basic_LRU::~Basic_LRU() {
	PCACHE_PAGE p = head_pointer;
	PCACHE_PAGE pp = NULL;

	if (head_pointer == NULL)
		return;
	while (p != tail_pointer) {
		pp = p;
		p = p->next;
		delete pp;
	}
	if (p == tail_pointer)
		delete p;

}
PCACHE_PAGE Basic_LRU::find_cache(long long int key)const {

//	PCACHE_PAGE curr = head_pointer;
//	PCACHE_PAGE p = NULL;
//
//	if (curr == NULL)
//		return NULL;
//
//	while (curr != tail_pointer) {
//		if (curr->kv.key == key) {
//			p = curr;
//			break;
//		}
//		curr = curr->next;
//	}
//
//	if (curr == tail_pointer) {
//		if (curr->kv.key == key) {
//			p = curr;
//		}
//	}
//
//	return p;

	PCACHE_PAGE curr = tail_pointer;
	PCACHE_PAGE p    = NULL;

	if(curr == NULL)
		return NULL;

	while(curr != head_pointer){
		if(curr->kv.key == key){
			p = curr;
			break;
		}
		curr = curr->prev;
	}

	if(curr == head_pointer){
		if(curr->kv.key == key){
			p = curr;
		}
	}

	return p;
}
void Basic_LRU::adjust_cache(PCACHE_PAGE p) {

	if(p == tail_pointer){
		return;
	}
	else if (p != head_pointer) {//p isnot either head_pointer or tail_pointer
		PCACHE_PAGE pprev = p->prev;
		PCACHE_PAGE pnext = p->next;

		pprev->next = pnext;
		pnext->prev = pprev;

		tail_pointer->next = p;
		p->next = NULL;
		p->prev = tail_pointer;
		tail_pointer = p;
	}
	else if(p == head_pointer)// p is headpointer and not tailpointer
	{
		PCACHE_PAGE pnext = p->next;

		pnext->prev = NULL;
		head_pointer = pnext;

		tail_pointer ->next = p;
		p->prev = tail_pointer;
		p->next = NULL;
		tail_pointer = p;

	}

}

bool Basic_LRU::hit_cache(long long int key) {

//	std::cout<<"hit_cache begin"<<std::endl;
	PCACHE_PAGE p = find_cache(key);
//	std::cout<<"find_cache over"<<std::endl;
	if (p == NULL) {//key does not exist in lru cache;
		return false;
	} else {//key exits in lru cache
		adjust_cache(p);
		return true;
	}
}
/*************************************************
 * 如果cache数量已满，返回被淘汰页面的key
 * 否则返回－1
 *
 */
long long int Basic_LRU::insert_cache(long long int k, int v) {

	if (curr_num == max_cache) {
		PCACHE_PAGE p = new CACHE_PAGE;
		p->kv.key = k;
		p->kv.value = v;
		p->next = NULL;
		p->prev = NULL;

		PCACHE_PAGE out_page = head_pointer;
		head_pointer = head_pointer->next;
		head_pointer->prev = NULL;

		tail_pointer->next = p;
		p->prev = tail_pointer;

		tail_pointer = p;
//		tail_pointer->next = NULL;

		long long int out = out_page->kv.key;
		delete out_page;

		return out;

	} else {
		PCACHE_PAGE p = new CACHE_PAGE;
		p->kv.key = k;
		p->kv.value = v;
		p->prev = NULL;
		p->next = NULL;

		if (curr_num == 0) {
			head_pointer = p;
			tail_pointer = p;
		} else {
			p->prev = tail_pointer;
			tail_pointer->next = p;
			tail_pointer = p;
		}
		curr_num++;

		return -1;

	}
}
void Basic_LRU::test() {
	std::cout << "void test() " << std::endl;

	PCACHE_PAGE p = head_pointer;
	if (!p) {
		std::cout << "lru is empty" << std::endl;
		return;
	} else {
		while (p != tail_pointer) {
			std::cout << "key: " << p->kv.key << " value: " << p->kv.value
					<< std::endl;
			p = p->next;
		}
		if (p == tail_pointer) {
			std::cout << "key: " << p->kv.key << " value: " << p->kv.value
					<< std::endl;
		}
	}

}
int Basic_LRU::getNum(){
	return curr_num;
}

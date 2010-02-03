/*
 * Basic_LRU.h
 *
 *  Created on: Jun 6, 2009
 *      Author: luo
 */

#ifndef BASIC_LRU_H_
#define BASIC_LRU_H_
#include <iostream>
typedef struct{
	long long int key;
	int value;
}LRU_KV,*PLRU_KV;

typedef struct page{
	LRU_KV kv;
	struct page* prev;
	struct page* next;
}CACHE_PAGE, *PCACHE_PAGE;
class Basic_LRU {
public:
	Basic_LRU(int num_cache);
	~Basic_LRU();
	bool hit_cache(long long int key);
	long long int insert_cache(long long int k, int v);
	int getNum();
	void test();

private:
	PCACHE_PAGE head_pointer;
	PCACHE_PAGE tail_pointer;
	const int max_cache;
	int curr_num;

	void adjust_cache(PCACHE_PAGE);
	PCACHE_PAGE find_cache(long long int key)const;

};

#endif /* BASIC_LRU_H_ */

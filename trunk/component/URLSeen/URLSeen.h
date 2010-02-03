/*
 * URLSeen.h
 *
 *  Created on: Apr 9, 2009
 *      Author: luo
 */

#ifndef URLSEEN_H_
#define URLSEEN_H_

#include "../../util/btree-f/btree.h"
#include "../../util/lru/Basic_LRU.h"
#include "../../util/thread_print/thread_print.h"
#include "../../util/rwlock_fcfs/pthread/rwlock_fcfs.h"
#include "../../global.h"
#include "../../util/RabinHash/rabinhash64.h"

class URLSeen {

private:
	int tid;

//	int cache_counter;

	std::string filename;

	BTTableClass btree_writer;/*,btree_reader;*/

	long Root; // fake pointer to the root node
	long NumNodes; // number of nodes in the B-tree
	long NumItems; // number of bytes per node
	NodeType nodetype;

//	BTTableClass btree_reader;

	RabinHashFunction64 rabin;

	Basic_LRU LRUCacher;
#ifdef DEBUG
	Thread_print tp;
#endif
//	pthread_rwlock_fcfs_t* rwlock_cache, *rwlock_btree;


	pthread_rwlock_t /*p_rwlock_cache,*/ p_rwlock_btree;

	pthread_mutex_t cache_lock;

//	pthread_mutex_t flag_lock;
//	int adjust_flag;


	bool check_cache(const long long);
	bool check_file(const long long);

//	bool getLock_r_cache();
	bool getLock_r_file();
//	bool getLock_w_cache();
	bool getLock_w_file();

//	bool releaseLock_cache();
	bool releaseLock_file();


public:

	URLSeen(int id);
	~URLSeen();

	long long hashurl(struct page_link*);
	long long hashurl(struct page_link*,  long long);


	bool check(long long);

	bool add(struct page_link*);
	bool add(long long);
};

#endif /* URLSEEN_H_ */

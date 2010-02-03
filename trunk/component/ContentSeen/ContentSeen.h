/*
 * ContentSeen.h
 *
 *  Created on: Apr 7, 2009
 *      Author: luo
 */

#ifndef CONTENTSEEN_H_
#define CONTENTSEEN_H_

#include "../../util/btree-m/avl_ex1.cpp"//没有遵守代码规范
#include "../../util/btree-f/btree.h"
#include "../../util/rwlock_fcfs/pthread/rwlock_fcfs.h"
#include "../../global.h"
#include "../../util/RabinHash/rabinhash64.h"
#include <string>

using namespace std;

class ContentSeen {

private:
	int tid;

	string filename;

	env btree_m;

	BTTableClass btree_f_write;

	long Root; // fake pointer to the root node
	long NumNodes; // number of nodes in the B-tree
	long NumItems; // number of bytes per node
	NodeType nodetype;

//	BTTableClass btree_f_read;

//	pthread_rwlock_fcfs_t* rwlock_m;

	pthread_rwlock_t p_rwlock_m;

//	pthread_rwlock_fcfs_t* rwlock_f;

	pthread_rwlock_t p_rwlock_f;

	int m_counter;

	RabinHashFunction64 rabin;

	bool write2file();
	bool check_m(long long hash);
	bool check_f(long long hash);
	bool getLock_m_r();
	bool getLock_f_r();
	bool getLock_m_w();
	bool getLock_f_w();
	bool releaseLock_m();
	bool releaseLock_f();

public:


	ContentSeen(int id);
	~ContentSeen();

	long long hash(char*, int);

	bool check(long long);

	bool add(long long );
};

#endif /* CONTENTSEEN_H_ */

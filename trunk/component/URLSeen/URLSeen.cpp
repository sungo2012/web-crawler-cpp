/*
 * URLSeen.cpp
 *
 *  Created on: Apr 9, 2009
 *      Author: luo
 */

#include "URLSeen.h"
#include <pthread.h>
#include <stdlib.h>
#include "../../util/rwlock_fcfs/pthread/rwlock_fcfs.h"

using namespace std;
/*********************************************************
 * 构造函数初始化是按在类中的定义顺序初始化的，因为先定义a,所以先初始
 * 化a,跟构造函数列表中数据出现的先后顺序无关。
 */
URLSeen::URLSeen(int id) :
	tid(id), filename(string("./URLSet/") + integer2string(id) + ".lru"),
			btree_writer('w', filename.c_str()), /*btree_reader('r',
			 filename.c_str()),*/rabin(HASH_SEED), LRUCacher(MAX_LRU)
#ifdef DEBUG
			, tp(id)
#endif
{

	//			rwlock_cache = pthread_rwlock_fcfs_alloc();
	//			rwlock_btree = pthread_rwlock_fcfs_alloc();
	bzero(&nodetype, sizeof(NodeType));
	NumNodes = 0L;
	NumItems = 0L;
	Root = 0L;

//	pthread_rwlock_init(&p_rwlock_cache, NULL);
	pthread_rwlock_init(&p_rwlock_btree, NULL);

	pthread_mutex_init(&cache_lock, NULL);
//	pthread_mutex_init(&flag_lock, NULL);
//	adjust_flag = 0;

}

URLSeen::~URLSeen() {

}

//private:
//bool URLSeen::getLock_r_cache() {
//
//	//			pthread_rwlock_fcfs_gain_read(rwlock_cache);
//
//	pthread_rwlock_rdlock(&p_rwlock_cache);
//	return true;
//}
//bool URLSeen::getLock_w_cache() {
//
//	//			pthread_rwlock_fcfs_gain_write(rwlock_cache);
//
//	pthread_rwlock_wrlock(&p_rwlock_cache);
//	return true;
//}
bool URLSeen::getLock_r_file() {

	//			pthread_rwlock_fcfs_gain_read(rwlock_btree);

	pthread_rwlock_rdlock(&p_rwlock_btree);
	return true;
}
bool URLSeen::getLock_w_file() {

	//			pthread_rwlock_fcfs_gain_write(rwlock_btree);

	pthread_rwlock_wrlock(&p_rwlock_btree);
	return true;
}

//bool URLSeen::releaseLock_cache() {
//
//	//			pthread_rwlock_fcfs_release(rwlock_cache);
//
//	pthread_rwlock_unlock(&p_rwlock_cache);
//	return true;
//}
bool URLSeen::releaseLock_file() {

	//			pthread_rwlock_fcfs_release(rwlock_btree);

	pthread_rwlock_unlock(&p_rwlock_btree);
	return true;
}

bool URLSeen::check_cache(long long hash) {

//	getLock_r_cache();

	pthread_mutex_lock(&cache_lock);

#ifdef DEBUG
		tp.print("LRU:test begin \n");
#endif

	bool res = LRUCacher.hit_cache(hash);

	if (res == false) {
#ifdef DEBUG
tp.print("LRU:not found \n");
#endif

//		releaseLock_cache();
		pthread_mutex_unlock(&cache_lock);
		return false;
	}

	else {

		pthread_mutex_unlock(&cache_lock);
//		releaseLock_cache();//释放读锁，请求写锁

#ifdef DEBUG
tp.print("LRU: Congratulations Hit It!\n");
#endif

		return true;
	}

}

bool URLSeen::check_file(long long hash) {

	getLock_r_file();
	if (NumItems == 0) {
		releaseLock_file();
		//文件还没有任何写入
		return false;
	}

	bool result = false;
	BTTableClass r_btree('r', filename.c_str());
	ItemType item;

	r_btree.setCurrentNode(nodetype);
	r_btree.setNumItems(NumItems);
	r_btree.setNumNodes(NumNodes);
	r_btree.setRoot(Root);

#ifdef DEBUG
tp.print("check_file before makesure \n");
tp.print("check_file makesure over\n");
#endif

	if (r_btree.Retrieve(hash, item))
		result = true;
	else
		result = false;
#ifdef DEBUG
if (result)
	tp.print("check_file retrieve over, and return true\n");
else
	tp.print("check_file retrieve over, and return false\n");
#endif

	releaseLock_file();

	return result;

}
long long URLSeen::hashurl(struct page_link* pl) {

	long long v, v1, v2;
	v1 = rabin.hash(pl->_host_addr, 256);
	v2 = rabin.hash(pl->_host_file, 1024);

	v = (v1 << 32) + v2;
	return v;
}
long long URLSeen::hashurl(struct page_link* pl, long long hash) {

	long long v, v1, v2;
	v1 = hash;
	v2 = rabin.hash(pl->_host_file, 1024);

	v = (v1 << 32) + v2;
#ifdef DEBUG
		tp.print("urlseen hash done\n");
#endif
	return v;
}

bool URLSeen::add(long long hash) {
	long long out;

	pthread_mutex_lock(&cache_lock);
	//如果内存中lru未被填满
	if (LRUCacher.getNum() < MAX_LRU) {
#ifdef DEBUG
tp.print("内存中lru未被填满\n");
#endif

		out = LRUCacher.insert_cache(hash, 1);

		pthread_mutex_unlock(&cache_lock);

	}
	//内存lru列表被填满
	else {
#ifdef DEBUG
tp.print("!内存lru列表被填满\n");
#endif

		//向btree中直接插入，如果btree中存在插入失败


		ItemType item;

		out = LRUCacher.insert_cache(hash, 1);

#ifdef DEBUG
tp.print(" !key 被写入文件\n");
tp.print("tid:%d 0\n", tid);
#endif

		pthread_mutex_unlock(&cache_lock);

#ifdef DEBUG
tp.print("realease cache lock and want file lock\n");
#endif

		getLock_w_file();
		item.KeyField = out;
		item.DataField = 1;
#ifdef DEBUG
tp.print("tid:%d 1\n", tid);
#endif

		if (btree_writer.Insert(item) == true) {
			btree_writer.makeSure();
#ifdef DEBUG
tp.print("tid:%d 2\n", tid);
#endif

			btree_writer.getPCurrentNode(nodetype);//传引用

			NumItems = nodetype.Branch[0];
			NumNodes = nodetype.Branch[1];
			Root = nodetype.Branch[2];

		}
#ifdef DEBUG
tp.print("tid:%d insert over realease file lock\n", tid);
#endif

		releaseLock_file();
	}
	return true;
}

bool URLSeen::add(struct page_link * plptr) {

	long long v = hashurl(plptr);

	return add(v);
}

bool URLSeen::check(long long pl) {

	bool result = false;
	long long hash = pl;
#ifdef DEBUG
tp.print("going to urlseencheck\n");
#endif

	if (check_cache(hash)) {
		result = true;

	} else {

		if (check_file(hash)) {

			long long out;

#ifdef DEBUG
tp.print("在urlseen文件中找\n");
#endif

			pthread_mutex_lock(&cache_lock);
			out = LRUCacher.insert_cache(hash, 1);
			pthread_mutex_unlock(&cache_lock);

			//把被lru淘汰的页面加入文件，如果文件中已存在导致插入失败即不会插入
			ItemType item;
			getLock_w_file();
			item.KeyField = out;
			item.DataField = 1;

			if (btree_writer.Insert(item) == true) {
				btree_writer.makeSure();

				btree_writer.getPCurrentNode(nodetype);

				NumItems = nodetype.Branch[0];
				NumNodes = nodetype.Branch[1];
				Root = nodetype.Branch[2];
			}
			releaseLock_file();

			result = true;
		}
	}

	return result;
}

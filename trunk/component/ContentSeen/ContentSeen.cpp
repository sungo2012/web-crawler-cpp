/*
 * ContentSeen.cpp
 *
 *  Created on: Apr 7, 2009
 *      Author: luo
 */

#include "./ContentSeen.h"
#include <stdlib.h>

ContentSeen::ContentSeen(int id) :
	tid(id), filename(string("./DocFPs/") + integer2string(id) + ".d"),
			btree_f_write('w', filename.c_str()),
			/*btree_f_read('r', filename.c_str()) ,*/ rabin(HASH_SEED){

	//Note:顺序不能颠倒

	//	rwlock_m = pthread_rwlock_fcfs_alloc();
	//	rwlock_f = pthread_rwlock_fcfs_alloc();
	bzero(&nodetype, sizeof(NodeType));
	NumNodes = 0L;
	NumItems = 0L;
	Root = 0L;

	pthread_rwlock_init(&p_rwlock_m, NULL);
	pthread_rwlock_init(&p_rwlock_f, NULL);

	m_counter = 0;

}

ContentSeen::~ContentSeen() {

}

//private:
bool ContentSeen::getLock_m_r() {

	//	pthread_rwlock_fcfs_gain_read(rwlock_m);

	pthread_rwlock_rdlock(&p_rwlock_m);
	return true;

}
bool ContentSeen::getLock_m_w() {

	//	pthread_rwlock_fcfs_gain_write(rwlock_m);

	pthread_rwlock_wrlock(&p_rwlock_m);
	return true;

}
bool ContentSeen::getLock_f_r() {

	//	pthread_rwlock_fcfs_gain_read(rwlock_f);

	pthread_rwlock_rdlock(&p_rwlock_f);
	return true;

}
bool ContentSeen::getLock_f_w() {

	//	pthread_rwlock_fcfs_gain_write(rwlock_f);

	pthread_rwlock_wrlock(&p_rwlock_f);
	return true;
}

bool ContentSeen::releaseLock_m() {

	//	pthread_rwlock_fcfs_release(rwlock_m);

	pthread_rwlock_unlock(&p_rwlock_m);
	return true;

}
bool ContentSeen::releaseLock_f() {

	//	pthread_rwlock_fcfs_release(rwlock_f);

	pthread_rwlock_unlock(&p_rwlock_f);
	return true;
}

bool ContentSeen::write2file() {

	getLock_m_w();
	getLock_f_w();

	if (m_counter >= MAX_DOC) {
		if (debug)
			cout << tid << ": ContentSeen trying to write to file..." << endl;

		queue<KV> q;
		btree_m.clearself(q);
		if (debug)
			cout << tid << ": ContentSeen 已经将btree_m内容导入到队列q中..." << endl;

		while (!q.empty()) {

			ItemType item;
			item.KeyField = q.front().name;
			item.DataField = q.front().value;

			btree_f_write.Insert(item);
			btree_f_write.makeSure();

			btree_f_write.getPCurrentNode(nodetype);

			NumItems = nodetype.Branch[0];
			NumNodes = nodetype.Branch[1];
			Root = nodetype.Branch[2];

			q.pop();
		}
		//		btree_f_write.makeSure();
		m_counter = 0;
		if (debug) {
			cout
					<< "report:===================================================================================now doc to 0:"
					<< m_counter << endl;
		}
	}

	releaseLock_f();
	releaseLock_m();

	return true;
}

bool ContentSeen::check_m(long long hash) {
	getLock_m_r();

	int in = btree_m.get(hash);

	releaseLock_m();

	return (in ? true : false);
}

bool ContentSeen::check_f(long long hash) {

	bool result = false;
	getLock_f_r();
	if (NumItems == 0) {
		releaseLock_f();
		//没有向文件中写入任何内容
		return false;
	}
		BTTableClass r_btree('r', filename.c_str());
	ItemType item;
	//	r_btree.makeSure();

	r_btree.setCurrentNode(nodetype);
	r_btree.setNumItems(NumItems);
	r_btree.setNumNodes(NumNodes);
	r_btree.setRoot(Root);

	if (r_btree.Retrieve(hash, item))
		result = true;
	else
		result = false;

	releaseLock_f();

	return result;
}
//public
long long ContentSeen::hash(char* doc, int size) {

	return (rabin.hash(doc, size));

}

bool ContentSeen::check(long long hash) {

	if (check_m(hash)) {
		if (debug)
			cout << "tid:" << tid << "contentseen check_m ok" << endl;
		return true;

	} else {
		if (debug)
			cout << "tid:" << tid << "contentseen check_m failed" << endl;
		if (check_f(hash)) {
			cout << "tid:" << tid << "contentseen check_f ok" << endl;
			return true;
		}
	}
	if (debug)
		cout << "tid:" << tid << "contentseen check failed" << endl;
	return false;
}
bool ContentSeen::add(long long hash) {

	if (m_counter >= MAX_DOC) {
		write2file();
	}
	getLock_m_w();
	btree_m.set(hash, 1);
	m_counter++;
	releaseLock_m();
	if (debug)
		cout
				<< "report:===================================================================================doc:"
				<< m_counter << endl;

	return true;
}

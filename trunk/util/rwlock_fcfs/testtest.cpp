/*
 * testtest.cpp
 *
 * this is test case for rwlock_fcfs
 *  Created on: Apr 12, 2009
 *      Author: luo
 */
/*

#include <iostream>
#include "./pthread/rwlock_fcfs.h"

using namespace std;

const int MAX_THREAD = 100;
class A{
private:
	pthread_rwlock_fcfs_t* lock;
public :
	A();
	void getRead();
	void getWrite();
	void release();
	void say();
};
A::A(){
	 lock = pthread_rwlock_fcfs_alloc();
}
void A::say(){
		cout<<"in class A"<<endl;
}
void A::getRead(){
	pthread_rwlock_fcfs_gain_read(lock);
}
void A::getWrite(){
	pthread_rwlock_fcfs_gain_write(lock);
}
void A::release(){
	pthread_rwlock_fcfs_release(lock);
}

A object;

void * thread_work(void* attr){

	int tid = (int)attr;



	while(1){

		object.getRead();
		object.say();
		cout<<"tid: "<<tid<<" read locking"<<endl;
		object.release();
		object.getWrite();
		cout<<"tid: "<<tid<<" write locking"<<endl;
		object.release();
		sleep(1);
	}
}

int main() {
	pthread_t threads[MAX_THREAD];
	int rc;
	void* status;

	for (int t = 0; t < MAX_THREAD; t++) {

		rc = pthread_create(&threads[t], NULL, thread_work, (void*) t);
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			return -1;
		}
	}

	for (int t = 0; t < MAX_THREAD; t++) {
			rc = pthread_join(threads[t], &status);
			if (rc) {
				printf("ERROR; return code from pthread_join() is %d\n", rc);
				return -1;
			}
			printf("Main: completed join with thread %ld having a status of %ld\n",
					t, (long)status);
		}

	return 0;

}*/


#ifndef _THREAD_PRINT_
#define _THREAD_PRINT_

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <fstream>

using namespace std;

class Thread_print
{
private:
	int tid;
	string int2string(int);
	FILE* fp;
public:
	Thread_print(int);
	~Thread_print();
	void print(const char*, ...);

};




#endif

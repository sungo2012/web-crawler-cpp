#include "thread_print.h"
#include <stdarg.h>
#include <sstream>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

Thread_print::Thread_print(int id):tid(id)
{
	string filename = int2string(tid)+".txt";

	if((fp = fopen(filename.c_str(), "a+")) == NULL)
	{
		printf("thread_print open failed\n");
		exit(1);
	}

}
Thread_print::~Thread_print()
{
	fclose(fp);
}

void Thread_print::print(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int v = fseek(fp, 0, SEEK_END);
	if(v){
		errno = 0;
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@fseekerror %s\n", strerror(errno));
	}
	vfprintf(fp, fmt, args);

	fflush(fp);

	va_end(args);

	return;
}

string Thread_print::int2string(int n)
{
	stringstream newstr(stringstream::in | stringstream::out);
	newstr << n;
	return newstr.str();
}

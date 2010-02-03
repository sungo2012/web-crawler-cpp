/*#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <cstdio>
using namespace std;

void del(string name) {

	string workdir = "/home/luolei/code/crawler/";
	string currdir;
	string cmd;

	currdir = workdir + name;
	cout << currdir << endl;
	if (access(currdir.c_str(), 0)) {
		cerr << "can not read dir: " << currdir <<" reason:"<<strerror(errno)<< endl;
		exit(-1);
	}
	chdir(currdir.c_str());
	cmd = " rm  * ";
	cout << "system : " << cmd << endl;
	system(cmd.c_str());

}

string integer2string(const int n) {

	stringstream newstr(stringstream::in | stringstream::out);
	newstr << n;
	return newstr.str();

}
int main() {

	del("DocFPs/");
	del("HTMLFiles/");
	del("QueueFiles/");
	del("URLSet/");

	for(int i=0; i<100; i++){
		string name("Header/");
		name = name + integer2string(i) + "/";

		del(name);
	}
	return 0;
}*/

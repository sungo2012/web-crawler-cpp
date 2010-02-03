/*
 * ris.cpp
 *
 *  Created on: Apr 7, 2009
 *      Author: luo
 */

#include "./ris.h"

RIS::RIS(){

	memset(fileBuffer, 0, sizeof(fileBuffer));
	curptr = fileBuffer;
}
RIS::~RIS(){

}

bool RIS::fillBuffer(char* b, const int size ){
	memcpy(fileBuffer, b, size);
	fileBuffer[MAX_BUFFER] = '\0';
	return true;
}

void RIS::rewindPtr(){
	curptr = fileBuffer;
}

char* RIS::getPtr(){
	rewindPtr();
	return curptr;
}
void RIS::clearself(){
	memset(fileBuffer, 0, DMAX_BUFFER);
	memset(headBuffer, 0, DMAX_HEAD);
	curptr = fileBuffer;
}
char* RIS::getHead(){
	return headBuffer;
}

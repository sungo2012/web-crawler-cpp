/*
 * ris.h
 *
 *  Created on: Apr 7, 2009
 *      Author: luo
 *
 *      ris = rewind input stream;
 *
 *      他是一个在线程内部被重复使用的一个对象，他接收协议模块传入的数据，然
 *      后依次分发给处理模块。
 *		模块在内存维护一个32K的空间用来缓冲文档
 */

#ifndef RIS_H_
#define RIS_H_

#include "../../global.h"
#include <string.h>

class RIS {
private:
	char fileBuffer[DMAX_BUFFER+1];
	char headBuffer[DMAX_HEAD];
	char* curptr;
public:
	RIS();
	~RIS();
	bool fillBuffer(char* b, const int size = MAX_BUFFER);
	void rewindPtr(void);
	void clearself();
	char* getPtr();
	char* getHead();

};

#endif /* RIS_H_ */

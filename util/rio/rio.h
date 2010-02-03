/*
 * rio.h
 *
 *  Created on: Apr 7, 2009
 *      Author: luo
 *
 *      详见《深入理解计算机系统（修订版）》
 */

#ifndef RIO_H_
#define RIO_H_

#include <unistd.h>
#include <errno.h>

ssize_t rio_readn (int fd, void* usrbuf, size_t n);
ssize_t rio_writen(int fd, void* usrbuf, size_t n);
ssize_t rio_readn_chunked(int fd, void*usrbuf, size_t n);

#endif /* RIO_H_ */

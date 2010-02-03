/*
 * rio.cpp
 *
 *  Created on: Apr 7, 2009
 *      Author: luo
 */

#include "./rio.h"
#include "../../global.h"
#include<iostream>
#include <string.h>
#include <netinet/in.h>
using namespace std;

ssize_t rio_readn(int fd, void* usrbuf, size_t n) {

	size_t nleft = n;
	ssize_t nread;
	char* bufp = (char*) usrbuf;

	while (nleft > 0) {

		if ((nread = read(fd, bufp, nleft)) < 0) {
			if (errno== EINTR){
				if(debug)std::cout << "error原因：" << strerror(errno) << std::endl;
				nread = 0;}
			else{
				if(debug)std::cout << "error原因：" << strerror(errno) << std::endl;
				return -1;}
		} else if (nread == 0) {
			break;
		}
		nleft -= nread;
		bufp += nread;
	}

	return (n - nleft);
}

ssize_t rio_readn_chunked(int fd, void*usrbuf, size_t n){

    unsigned char tmp[4]={'0'};
    int nread;
    int nleft = n;
    unsigned char * p = (unsigned char*)usrbuf;
    int chunksize;

    int rn;
    char c;
    unsigned char* ptmp = tmp;
    for(int i=0; i<4; i++, ptmp++)
    {   if(read(fd, ptmp, 1)<0)return -1;
         if(*ptmp==';'||*ptmp=='\r')break;
    }
    if(*ptmp == '\r')
    {
        *ptmp = '\0';
        sscanf((char*)tmp, "%x", &chunksize);
        //读取剩余的\n
        if(read(fd, tmp, 1)<0||*tmp!='\n')return -1;
    }
    else//包括两种情况，＊ptmp＝＝‘;'或者i＝＝4
    {
        if(*ptmp == ';') *ptmp = '\0';
        sscanf((char*)tmp, "%x", &chunksize);

        unsigned char c;
        int rn=0;
        read(fd, &c, 1);
        while(rn != 2)
        {
            if(c == '\r')
                rn = 1;
            else if(c == '\n')
                rn += 1;
            else
                rn = 0;
            if(rn!=2)read(fd,&c, 1);
        }
    }
#ifdef DEBUG
    cout << "CHUCK传输 初始size " << chunksize << endl;
#endif


    while(chunksize != 0)
    {
        if ((nread = rio_readn(fd, p, chunksize)) <= 0) {
            return -1;
        }

        p += nread;
        nleft -= nread;

        //读chunk边界符号\r\n 
        rn = 0;
        read(fd, &c, 1);
        while(rn != 2)
        {
            if(c == '\r')
                rn = 1;
            else if(c == '\n')
                rn += 1;
            else
                rn = 0;
            if(rn!=2)read(fd, &c, 1);
        }

        ptmp = tmp;
        for(int i=0 ; i<4; i++, ptmp++)
        {
            if(read(fd, ptmp, 1)<0)return -1;
            if(*ptmp==';'||*ptmp=='\r')break;
        }
        if(ptmp[0] == '\r')
        {
            *ptmp = '\0';
            sscanf((char*)tmp, "%x", &chunksize);
            //读取剩余的\n
            if(read(fd, tmp, 1)<0)return -1;
            if(*tmp!='\n')return -1;
        }
        else//包括两种情况，＊ptmp＝＝‘;'或者i＝＝4
        {
            if(*ptmp == ';') *ptmp = '\0';
            sscanf((char*)tmp, "%x", &chunksize);
            unsigned char c;
            int rn=0;
            read(fd, &c, 1);
            while(rn != 2)
            {
                if(c == '\r')
                    rn = 1;
                else if(c == '\n')
                    rn += 1;
                else
                { rn = 0;printf("ooo %c\n", c);}
                if(rn!=2)read(fd,&c, 1);
            }
        }
#ifdef DEBUG
cout << "CHUCK size " << chunksize << endl;
#endif



    }

    return (n-nleft);
}

ssize_t rio_writen(int fd, void* usrbuf, size_t n) {

    size_t nleft = n;
    ssize_t nwriten;
    char* bufp = (char*) usrbuf;

    while (nleft > 0) {
        if ((nwriten = write(fd, bufp, nleft)) <= 0) {
            if (errno== EINTR)
                nwriten = 0;
            else {
                std::cout << "error原因：" << strerror(errno) << std::endl;
                return -1;
            }
        }
        nleft -= nwriten;
        bufp += nwriten;

    }

    return n;
}

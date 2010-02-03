/*
 * Writer.h
 *
 *  Created on: Apr 8, 2009
 *      Author: luo
 *	这个类用来把RIS中被确认为非重复页面写入文件中
 */

#ifndef WRITER_H_
#define WRITER_H_

#include "../../global.h"
#include "../../util/thread_print/thread_print.h"
#include "../../component/ris/ris.h"
#include <string>
#include <ctime>
using namespace std;
class Writer {
    public:
        Writer(int);
        virtual ~Writer();
        bool writeLocal(int tid, struct page_link* pl, char * p_ip, RIS& ris, const char * vaildstr="200 OK", int size = 16);
        inline int min(int a, int b) {
            if (a <= b)
                return a;
            else
                return b;
        }
    private:
        string file;
        //一下的变量为writeHeader函数提供时间
        static const char * writer_wday[7];
        static const char * writer_month[12];

        char stime[128];

        time_t timep;
        struct tm *p;

        int tid;
#ifdef DEBUG
        Thread_print tp;
#endif

        bool WriteIt(struct page_link pl, char* p, int size = MAX_BUFFER);

        //如果网页中有指定字符串（如200 ok）返回true，否则返回false
        bool writeHeader(int tid, char* buf, struct page_link *p_pl, char* p_ip, int size = 16);//writeHeader must be used after WriteIt, because WriteIt function call fill_filename funticon to fill filename which is used by writeheader

        void fill_filename(struct page_link pl);
};


#endif /* WRITER_H_ */

#ifndef _CRAWLER_H
#define _CRAWLER_H


#include <iostream>
#include <vector>
#include <pthread.h>
#include <stdlib.h>
#include <new>
#include <time.h>
#include "global.h"
#include "component/URLFrontier/URLFrontier.h"
#include "component/ContentSeen/ContentSeen.h"
#include "component/URLSeen/URLSeen.h"
#include "util/RabinHash/rabinhash64.h"
#include "util/thread_print/thread_print.h"
#include "component/LinkExtractor/LinkExtractor.h"
#include "component/protocolModule/HttpModule.h"
#include "component/URLFilter/URLFilter.h"
#include "component/ris/ris.h"
#include "component/Writer/Writer.h"
#include "component/protocolModule/protocolBaseModule.h"

#include <signal.h>
using namespace std;


extern vector<URLFrontier*> urlfrontier_vector;
extern vector<ContentSeen*> contentseen_vector;
extern vector<URLSeen*> urlseen_vector;


//初始化全局的处理函数这种初始化vector的方法低效，不过幸好只在开时候初始化一次
void initURLFrontier() {

    for (int i = 0; i < MAX_THREAD; i++) {
        cout << "init urlfrontier( " << i << " )" << endl;
        URLFrontier* puf = new URLFrontier(i);
        urlfrontier_vector.push_back(puf);
    }
}

void initContentSeen() {
    for (int i = 0; i < MAX_THREAD; i++) {
        cout << "init contentseen (" << i << ")" << endl;
        ContentSeen* pcs = new ContentSeen(i);
        contentseen_vector.push_back(pcs);
    }
}

void initUrlSeen() {
    for (int i = 0; i < MAX_THREAD; i++) {
        cout << "init urlseen(" << i << ")" << endl;
        URLSeen* pus = new URLSeen(i);
        urlseen_vector.push_back(pus);
    }
}
//线程工作函数
void * thread_work(void* attr) {

    int tid = (int) attr;
    struct page_link* p_pl = new struct page_link;
    struct page_link* holder_pl = new struct page_link;

    RabinHashFunction64 rabin(HASH_SEED);
    long long urlhash, urlmod;

    //每个线程私有变量
    RIS ris;
    HttpModule module(tid);
    LinkExtractor le;
    URLFilter filter(tid);
    Writer writer(tid);
#ifdef DEBUG
    Thread_print tp(tid);
#endif
    char* p_ip;

    while (1) {
        if (urlfrontier_vector[tid]->available()) {
            if (!urlfrontier_vector[tid]->readURL(p_pl)) {
                continue;
            }
#ifdef DEBUG
            tp.print("=========mainthread:going to fetch\n");
#endif
            //从当前链接中取出相应头和HML内容放入ris中
            if (!module.fetch(*p_pl, ris.getHead(), ris.getPtr(), tid)) {
                ris.clearself();
                //如果失败继续返回继续从队列中取出链接
                continue;
            }

            p_ip = module.getIP();

            ris.rewindPtr();
            //			如果需要contentseen功能可以取消下面的注释
            //			hash = rabin.hash(ris.getPtr(), MAX_BUFFER);
            //			mod = hash % MAX_THREAD;
            //			if (mod < 0) {
            //				mod = 0 - mod;
            //			}

            //     //如果已经存在则不需要下载

            //			if (contentseen_vector[mod]->check(hash)) {
            //				ris.clearself();
            //				continue;
            //			}
            //
            //			contentseen_vector[mod]->add(hash);
            //			if (debug) {
            //				cout << "tid:" << tid << " " << "需要下载" << endl;
            //			}
            //			ris.rewindPtr();
#ifdef DEBUG
            tp.print("=========mainthread:going to write\n");
#endif
            if(!writer.writeLocal(tid, p_pl, p_ip, ris))
            {
                ris.clearself();
                //如果失败继续返回继续从队列中取出链接
                continue;
            }

            pthread_mutex_lock(&mutex_counter);
            pagecounter++;
            printf("%d################################%d#############\n", tid,pagecounter);
            pthread_mutex_unlock(&mutex_counter);
            ris.rewindPtr();
            /*
             * 因为从网页中提取的超链接可能会有很多如../或./这样的链接，
             * 因此若要分析出这些链接究竟指向那个网址，保存下当前网页的地址
             * 并基于当前地址分析变得必须，所以每次从一个网页开始提取链接时候
             * 必须进行一次初始化工作。
             */
            le.initLinkExtractor(p_pl, ris.getPtr(), MAX_BUFFER);
            memset(holder_pl, 0, sizeof(struct page_link));

            while (le.getLink(holder_pl)) {
#ifdef DEBUG
                tp.print("=========mainthread:got holder_pl\n");
#endif
                /**********************************************************
                 *应对新的需要－－只针对性的爬几个网站，不能在用以前的线程 分配函数直接
                 *采用随机分配策略,
                 *实验证明随机分配策略导致cpu几乎满载，改为顺序添加策略
                 *即把每取出的20个超链接依次放入队列1，2，3.。。。中，
                 *这种策略的坏处是大量线程向同一服务器请求资源
                 urlhash = rabin.hash(holder_pl->_host_addr, 256);
                 urlmod = urlhash % MAX_THREAD;
                 if (urlmod < 0) {
                 urlmod = 0 - urlmod;
                 }
                 */
#ifdef DEBUG
                tp.print("=========mainthread:going to filter\n");
#endif
                if (filter.filter(holder_pl)) {//未名中
#ifdef DEBUG
                    tp.print("passed addr:%s\nfile:%s\n", holder_pl->_host_addr,holder_pl->_host_file);
#endif

                    memset(holder_pl, 0, sizeof(struct page_link));

                    continue;
                }
#ifdef DEBUG
                tp.print("addr:%s\nfile:%s\n", holder_pl->_host_addr,holder_pl->_host_file);
#endif

                /*
                 *
                //根据当前网址的主机名字哈西出该由哪个来进行urlseen处理
                urlhash = rabin.hash(holder_pl->_host_addr, 256);
                 *
                 *这种获取urlhash的方式是以前高效的爬互联网的线程分配方式，
                 *现在的需求是只爬几个站点，所以这种高效的方式不得不被
                 *抛弃了，唉！
                 *
                 *注意：！现在 的urlhash是根据当前网址哈希出的值，不在是根据服务器名哈希出的值
                 */
                urlhash = hash_site(holder_pl);
                urlmod = urlhash % MAX_THREAD;
                if (urlmod < 0) {
                    urlmod = 0 - urlmod;
                }

                //哈西出当前网址的值
#ifdef DEBUG
                tp.print("=========mainthread:going to urlseencheck%d\n",urlmod);
#endif

                if (urlseen_vector[urlmod]->check(urlhash)) {
                    memset(holder_pl, 0, sizeof(struct page_link));
                    continue;
                }
#ifdef DEBUG
                tp.print("=========mainthread:going to urlseen_add %d\n",urlmod );
#endif

                //把当前url加入列表标志为已经下载，这个url加入下载列表
                urlseen_vector[urlmod]->add(urlhash);

#ifdef DEBUG
                tp.print("=========mainthread:going to frontier add %d\n",urlmod);
#endif
                urlfrontier_vector[urlmod]->addURL(holder_pl);

                memset(holder_pl, 0, sizeof(struct page_link));
            }
            //对一个网址的url提取完毕后，作清理工作
            le.clearself();

            ris.clearself();

        }

        else {//function available return false
            cout << "not available" << endl;
            sleep(1);
        }
    }

    return NULL;
}

//function to call if operator new can't allocate enough memory
void noMoreMemory() {
    cerr << "error:Unable to satisfy request for memory\n";
    abort();
}
/*
 * 信号处理函数用于gprof的调试
 */
void sighandler(int s) {
    printf("sighandler recieve a %d signal\n", s);
    exit(0);
}

int crawler()
{
    set_new_handler(noMoreMemory);
    signal(SIGUSR1, sighandler);

    pthread_t threads[MAX_THREAD];
    int rc;
    void* status;
    pthread_mutex_init(&mutex_counter, NULL);
    pthread_mutex_init(&mutex_assign_thread, NULL);

    cout << "开始进行初始化..." << endl;
    cout << "========================" << endl;
    initUrlSeen();
    initURLFrontier();
    initContentSeen();
    cout << "========================" << endl;
    cout << "开始读url种子文件..." << endl;

    RabinHashFunction64 rabin(HASH_SEED);
    long long hash;
    int mod;
    LinkExtractor le;
    struct page_link main_pl;

    fstream seedfile;
    char httpbuffer[1280];
    memset(httpbuffer, 0, 1280);
    seedfile.open("./urlseed.txt", ios_base::in);
    if (!seedfile.is_open()) {
        cout << "error:canot open urlseed.txt";
        exit(1);
    }

    while (seedfile.getline(httpbuffer, 1280)) {
        cout << "file:" << httpbuffer << endl;
        hash = rabin.hash(httpbuffer, 1280);
        httpbuffer[1279] = '\0';
        mod = hash % MAX_THREAD;
        if (mod < 0) {
            mod = 0 - mod;
        }
        if (le.URLanalyser(&main_pl, httpbuffer)) {
            urlfrontier_vector[mod]->addURL(&main_pl);
            cout << "urlfrontier(" << mod << ") addurl" << endl;
            hash = urlseen_vector[mod]->hashurl(&main_pl);
            urlseen_vector[mod]->add(hash);
        } else {
            continue;
        }
    }
    seedfile.close();
    cout << "＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝" << endl;
    cout << "读url种子文件完成..." << endl;


    cout << "＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝" << endl;
    cout << "main开始创建线程" << endl;


    //	thread_work((void*)0);
    for (int t = 0; t < MAX_THREAD; t++) {

        rc = pthread_create(&threads[t], NULL, thread_work, (void*) t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    cout << "＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝" << endl;
    cout << "main开始创建线程完成" << endl;


    //等待线程结束才能终止主程序，实际上线程永远不可能以这种形式退出，必须以信号形式退出。
    while(1)sleep(100);
    for (int t = 0; t < MAX_THREAD; t++) {
        rc = pthread_join(threads[t], &status);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: completed join with thread %ld having a status of %ld\n",
                t, (long int) status);
    }
    pthread_mutex_destroy(&mutex_counter);

    return 0;
}

#endif//crawler.h

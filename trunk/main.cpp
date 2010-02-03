#include "component/URLFrontier/URLFrontier.h" 
#include "component/ContentSeen/ContentSeen.h"
#include "component/URLSeen/URLSeen.h"
#include "crawler.h"
#include "fresh.h"

vector<URLFrontier*> urlfrontier_vector;
vector<ContentSeen*> contentseen_vector;
vector<URLSeen*> urlseen_vector;

void printusage()
{
    using namespace std;
    
    cout << "crawler usage:" << endl;
    cout << "crawler --fetch or crawler --fresh" << endl;
}

int main(int args, char* argv[])
{
    if(args == 2&&strcmp("--fetch", argv[1])==0)
        crawler();
    else if(args == 2&&strcmp("--fresh", argv[1])==0)
        fresh();
    else
        printusage();

    return 0;
}

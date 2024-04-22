#include "BMTTcyc.h"
#include <psapi.h>
#include<iostream>

using namespace std;

static std::atomic<int> getids;//辅助线程获取任务
int getid[16]={0};//辅助线程获取任务
float percent=0.1;//金额差值比

vector<int> scc[200000];
atomic<int> mun(0);
int big =INT_MAX;

int mode ;
int longstack = 100000;//设定模拟数组长度

graph::graph(int x,int y,int t)
{
    vexnum = x;
    arcnum = y;
    mode = t;

    vertscc = new vNode[x+1];//邻接表结构
    stackt = new arcNode[x+1];//tarjan模拟递归
    dfn = new int[x+1];//tarjan辅助数组
    low = new int[x+1];//tarjan辅助数组
    ins = new int[x+1];//tarjan辅助数组
    color = new int [x+1];//tarjan辅助数组
    stacks = new int [x+1];//tarjan辅助数组
    visited = new int [x+1];//标记是否访问


    timeblock = new int[x+1];;//阻塞数组
    Blist = new vNode[x+1];//阻塞列表
    Bstack = new sarcNode[mode];//模拟解锁递归
    arcNode *o = new arcNode;
    o->arctime = -1;
//    o->arcmoney = -1;


    //初始化
    for(int i=0;i<mode;i++){
        Bstack[i].next = new arcNode[longstack];
        Bstack[i].next[0].next = NULL;
        for(int g = 1;g<=100;g++ ){
            Bstack[i].next[g].next = NULL;
        }

    }
    for(int i=0;i<=x;i++) {
        vertscc[i].firstarc = NULL;
        stackt[i].next=NULL;
        Blist[i].firstarc=NULL;
        timeblock[i]=big;
        ins[i]=dfn[i]=low[i]=visited[i]=0;
    }

    stackttop=-1;
    cnt=0;
    ststop=-1;
    stmp=0;//Tarjan
}
graph::~graph(){
    delete vertscc;
    delete stackt;
    delete dfn;
    delete low;
    delete visited;
    delete ins;
    delete color;
    delete stacks;
    delete Blist;
    delete timeblock;

}

// 函数用于获取当前进程的内存使用量
SIZE_T GetProcessMemoryUsage() {
    HANDLE hProcess = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}

bool graph::absa(int a,int b){
    int k = a>b?a-b:b-a;
    float pps = (float)k /(float)a;
    if(pps<percent) return true;
    else return false;
}

//构建邻接表结构
void graph::makescc(int a, int b,int c){
    arcNode* p = new arcNode ;
    p->arcdata=b;
    p->arctime = c;
    //头插入创建节点。
    p->next = vertscc[a].firstarc;
    vertscc[a].firstarc = p;
}

void graph::getdi(int i){//辅助线程获取任务
    getid[getids++] = GetCurrentThreadId();
    chrono::milliseconds dure(1);//休眠一下时间，确保线程都创建成功，并排序线程号
    this_thread::sleep_for(dure);
}

int graph::finits(int i){
    for(int l=0;l<mode;l++){
        if(i == getid[l]) return l;
    }
    return -1;
}

void graph::missfind(int i){
    findallcyc(i);
}

void graph::findallcyc(int i){
    int p = scc[i].size();
    int id = GetCurrentThreadId();
    int a = finits(id);//根据线程号，使用对应的模拟递归栈
    arcNode *q;
    for(int j=0;j<p;j++){
        int index = scc[i][j];
        int Btop = 0,cyctime =0;
//        bool flags = false;
        unlock(index,&Btop,Bstack[a].next,big);
//        recurFindcycles(index,0,index,-1,&Btop,Bstack[a].next,&cyctime,flags);
        recurFindcycles(index,index,-1,&Btop,Bstack[a].next,&cyctime);
    }
}

//bool graph::recurFindcycles(int index,int getmoney,int getvex,int gettime,int* Btop,arcNode* Bstack,int* cyctime,bool fs){
bool graph::recurFindcycles(int index,int getvex,int gettime,int* Btop,arcNode* Bstack,int* cyctime){
    arcNode *p; bool cyctfound = false;// 标记当前搜索是否成环
    int cycnowtime = 0; //记录当前的解锁时间
    p = vertscc[getvex].firstarc;
    while(p!=nullptr){
        int k = p->arcdata; int t = p->arctime;
        if(t > gettime){
            bool nowfound = false;
            bool cycflag = false;// 获取回溯的成环信息
//            bool nowf = false;//标记当前路径是否满足金额约束
//            if(fs || !absa(getmoney,m)){
//                nowf = true;
//            }
            if(k==index){
                nowfound = true;
//                if(!nowf) {
                mun++;
                   cout<<mun<<" ";
//                }
                if(t>cycnowtime){  //
                    cycnowtime = t;
                }
            }else if(!visited[k] && t<timeblock[k]){
                // cout<<k<<"xun ";
                visited[k]=1; timeblock[k] = t;
//                cycflag = recurFindcycles(index,m,k,t,Btop,Bstack, cyctime,nowf);
                cycflag = recurFindcycles(index,k,t,Btop,Bstack, cyctime);
                visited[k] =0;
            }
            if(nowfound||cycflag){
                cyctfound = true;
                if(cycflag){
                    *cyctime = *cyctime>cycnowtime?*cyctime:cycnowtime;
                    cycnowtime = *cyctime;
                }else{
                    *cyctime = cycnowtime;
                }
            }else{
                makeBlist(k,getvex,t);

            }
        }
        p = p->next;
    }
    if(cyctfound){
        unlock(getvex,Btop,Bstack,*cyctime);
    }
    return cyctfound;
}


//给Blist压入节点。与邻接表类似
void graph::makeBlist(int a,int b,int c){
    // cout<<a<<"="<<b<<" ";
    arcNode*q = Blist[a].firstarc;
    while(q!= nullptr){
        if(q->arcdata == b){
            if(q->arctime<c){
                q->arctime = c;
            }
            break;
        }
        q = q->next;
    }
    if(q==nullptr){
        arcNode* p = new arcNode ;
        p->arcdata=b;
        p->arctime =c;
        //头插入创建节点。
        p->next = Blist[a].firstarc;
        Blist[a].firstarc = p;
    }
}

//解锁完后删除所有节点。
void graph::deleBlist(int a,int time){
    arcNode* p;arcNode* q;arcNode* dums;
    dums = new arcNode;
    p = Blist[a].firstarc;
    Blist[a].firstarc = NULL;
    dums->next=p;
    q = dums;
    while(p!=NULL){
        if(p->arctime<time){
            q->next = p->next;
            delete p;
            p = q->next;
        }else{
            q = p;
            p = p->next;
        }
    }
    q->next=NULL;
    Blist[a].firstarc=dums->next;
    delete dums;
}
//非递归的形式实现联级解锁
void graph::unlock(int u,int* Btop,arcNode* Bstack,int overtime){
    Bstack[++(*Btop)].arcdata=u;
    while(*Btop!=0){
        u = Bstack[*Btop].arcdata;
        // cout<<endl;
        // cout<<"解"<<u<<" ";
        timeblock[u]=overtime>timeblock[u]?overtime:timeblock[u];
        arcNode* r;
        // cout<<*Btop<<" ";
        if(Bstack[*Btop].next!=NULL){
            r = Bstack[*Btop].next->next;
            // cout<<"1 ";
        }else{
            r = Blist[u].firstarc;
            // cout<<"2 ";
        }
        while (r!=NULL)
        {
            int e = r->arcdata;
            // if(e==2){
            //     cout<<r->arctime<<"=="<<timeblock[2]<<" "<<overtime<<" ";
            // }
            // cout<<e<<"解吗？ ";
            if(overtime>timeblock[e]){
                // cout<<e<<"压入 ";
                Bstack[*Btop].next= r;
                Bstack[++(*Btop)].arcdata= e;
                break;
            }else{
                r=r->next;
            }
        }
        if(r==NULL){
            Bstack[*Btop].next=NULL;
            // cout<<"删？ ";
            deleBlist(Bstack[(*Btop)--].arcdata,overtime);
        }
    }
}


//遍历求环。
void graph::findallscc(){
    for(int i=1;i<=vexnum;i++){
        if(!dfn[i]) tarjan(i); //从没有访问过的节点开始。
    }
}


void graph::tarjan(int a){
    int b,c,d;arcNode * p,*q;
    stackt[++stackttop].arcdata = a;
    dfn[a] = low[a] = ++stmp;
    stacks[++ststop] = a;
    ins[a] = 1;
    while(stackttop>=0){
        b = stackt[stackttop].arcdata;
        // cout<<" 外t = "<<b<<endl;
        if(stackt[stackttop].next== NULL){
            p = vertscc[b].firstarc;
        }else{
            p = stackt[stackttop].next->next;
        }
        while(p!=NULL){
            c = p->arcdata;
            // cout<<"for边内 = "<<c<<endl;
            if(dfn[c]==0){
                //  cout<<"dfn未访问 = "<<c<<endl;
                dfn[c] = low[c] = ++stmp;
                stacks[++ststop] = c;
                ins[c] = 1;
                stackt[stackttop++].next = p;
                stackt[stackttop].arcdata = c;
                break;
            }
            p = p->next;
        }
        if(b == stackt[stackttop].arcdata){
            q = vertscc[b].firstarc;
            // cout<<" t == s.top()= "<<b<<endl;
            while(q!= NULL){
                d = q->arcdata;
                // cout<<"for推内 = "<<d<<endl;
                if(dfn[d]>dfn[b]){
                    low[b] = min(low[b],low[d]);
                }else if(ins[d]==1){
                    low[b] = min(low[b],low[d]);
                }
                q = q->next;
            }
            if(dfn[b] == low[b]){
                // cout<<" dfn[t] == low[t] = "<<b<<endl;
                ++cnt; int y;
                do{
                    y = stacks[ststop--];
                    ins[y]= 0;
                    //  cout<<"while内 = "<<y<<endl;
                    color[y] = cnt;
                    scc[cnt].push_back(y);
                }while(b != y);
            }
            stackt[stackttop--].next = NULL;
        }
    }
}

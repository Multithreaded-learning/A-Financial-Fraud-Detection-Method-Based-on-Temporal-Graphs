#include "JMTTcyc.h"
#include <psapi.h>
#include<iostream>

using namespace std;

static std::atomic<int> getids;//辅助线程获取任务
int getid[16]={0};//辅助线程获取任务
float percent=0.1;//金额差值比
vector<int> scc[200000];
atomic<int> mun(0);
int mode ;

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
    block = new int[x+1];//阻塞数组
    Blist = new vNode[x+1];//阻塞列表
    stack = new snode[mode];//模拟搜索递归
    Bstack = new sarcNode[mode];//模拟解锁递归
    arcNode *o = new arcNode;
    o->arctime = -1;
//    o->arcmoney = -1;
    //初始化
    for(int i=0;i<mode;i++){
        stack[i].next = new node[101];
        Bstack[i].next = new arcNode[101];
        stack[i].next[0].next = o;
        stack[i].next[0].arctime = 0;
        Bstack[i].next[0].next = NULL;
        for(int g = 1;g<=100;g++ ){
            stack[i].next[g].next = NULL;
            stack[i].next[g].flag = false;
            stack[i].next[g].arctime = 0;
            Bstack[i].next[g].next = NULL;
        }

    }
    for(int i=0;i<=x;i++) {
        vertscc[i].firstarc = NULL;
        stackt[i].next=NULL;
        Blist[i].firstarc=NULL;
        block[i]=0;
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
    delete block;

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

//构建邻接表结构
//void graph::makescc(int a, int b,int c,int d){
void graph::makescc(int a, int b,int c){
    arcNode* p = new arcNode ;
    p->arcdata=b;
    p->arctime = c;
//    p->arcmoney = d;
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
    int p = scc[i].size(), top1 = 0, Btop = 0,index,a;
    int id = GetCurrentThreadId();
    a = finits(id);//根据线程号，使用对应的模拟递归栈
    for(int j=0;j<p;j++){
        index = scc[i][j];
        Findcycles(index,index,stack[a].next,top1,&Btop,Bstack[a].next);
        visited[index]=1;
    }
}


bool graph::absa(int a,int b){//判断当前差值比例是否满足要求
    int k = a>b?a-b:b-a;
    float pps = (float)k /(float)a;
    if(pps<percent) return true;
    else return false;
}


//主要的找环算法。
void graph::Findcycles(int start,int cur,node* stack,int top1,int* Btop,arcNode* Bstack){
    stack[++top1].arcdata=cur;//压入栈。
    int l = -1;  //用于判断当前节点发出的边有无符合时序和金额的
    block[cur]=1;
    while(top1!=0){
        cur=stack[top1].arcdata;
        arcNode* p;
        if(stack[top1].next== NULL){
            p = vertscc[cur].firstarc;
        }else{
            p = stack[top1].next->next;
        }
        while(p!=NULL){
            int  e = p->arcdata;
            if(e==start){///找到路径
                stack[top1].flag=true;//标记存在环
//                if(stack[top1-1].next->arctime<p->arctime&&absa(stack[top1-1].next->arcmoney,p->arcmoney)){有金额限制
                if(stack[top1-1].next->arctime<p->arctime){
                    //最后符合时序和金额 则是所找的时序环 保存信息并计算数量
                    mun++;
////                    cout<<mun<<" ";
//               保存并输出环路
//                    for(int l = 1;l<=top1;l++){
//                        if(summs[stack[l].arcdata]==0) summs[stack[l].arcdata]++;
//                        // for(int l = 1;l<=top;l++){
//                        //     cout<<stack[l].arcdata<<"-";
//                        // }
//                    }
                }
            }else if(!block[e]){//节点为被阻塞
                l = top1;
//                if(stack[top1-1].next->arctime<p->arctime&&absa(stack[top1-1].next->arcmoney,p->arcmoney)){
                if(stack[top1-1].next->arctime<p->arctime){
                    //满足时序和金额要求
                    stack[top1++].next=p;//压入栈
                    stack[top1].arcdata=e;
                    block[e]=1;
                    break;//退出 继续新节点的搜索
                }
            }
            p=p->next;
        }
        if(l ==top1){//如果当前节点发出的边都不符合时序和金额的要求 则默认能成环
            stack[top1].flag=true;
        }
        if(p==NULL){
            if(stack[top1].flag){//若标记为成环
                unlock(cur,Btop,Bstack);//则解锁
            }else{
                arcNode* h = vertscc[cur].firstarc;int k;
                while(h!=NULL){
                    k=h->arcdata;
                    if(!visited[k]){
                        makeBlist(k,cur);  //不成环则节点上锁，并保存在Blist中
                    }
                    h = h->next;
                }
            }
            stack[top1].next=NULL;
            if(stack[top1--].flag){
                stack[top1].flag=true;
            }//该节点访问完毕，退栈，找下一个节点
        }
    }

}


//给Blist压入节点。与邻接表类似
void graph::makeBlist(int a,int b){
    // cout<<a<<"="<<b<<" ";
    arcNode* p = new arcNode ;
    p->arcdata=b;
    //头插入创建节点。
    p->next = Blist[a].firstarc;
    Blist[a].firstarc = p;
}

//解锁完后删除所有节点。
void graph::deleBlist(int a){
    arcNode* p;arcNode* q;
    p = Blist[a].firstarc;
    Blist[a].firstarc=NULL;
    while(p!=nullptr){
        q=p->next;
        delete p;
        p = q;
    }
}
//非递归的形式实现联级解锁
void graph::unlock(int u,int* Btop,arcNode* Bstack){
    Bstack[++(*Btop)].arcdata=u;
    while(*Btop!=0){
        u = Bstack[*Btop].arcdata;
        block[u]=0;//解锁
        arcNode* r;
        if(Bstack[*Btop].next!=NULL){
            r = Bstack[*Btop].next->next;
        }else{
            r = Blist[u].firstarc;
        }
        while (r!=NULL)
        {
            int e = r->arcdata;
            if(block[e]){//联级解锁
                Bstack[*Btop].next= r;
                Bstack[++(*Btop)].arcdata= e;
                break;
            }else{
                r=r->next;
            }
        }
        if(r==NULL){
            Bstack[*Btop].next=NULL;
            deleBlist(Bstack[(*Btop)--].arcdata);// 删除Blist里的节点
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
        if(stackt[stackttop].next== NULL){
            p = vertscc[b].firstarc;
        }else{
            p = stackt[stackttop].next->next;
        }
        while(p!=NULL){
            c = p->arcdata;
            if(dfn[c]==0){
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
            while(q!= NULL){
                d = q->arcdata;
                if(dfn[d]>dfn[b]){
                    low[b] = min(low[b],low[d]);
                }else if(ins[d]==1){
                    low[b] = min(low[b],low[d]);
                }
                q = q->next;
            }
            if(dfn[b] == low[b]){
                ++cnt; int y;
                do{
                    y = stacks[ststop--];
                    ins[y]= 0;
                    color[y] = cnt;
                    scc[cnt].push_back(y);
                }while(b != y);
            }
            stackt[stackttop--].next = NULL;
        }
    }
}


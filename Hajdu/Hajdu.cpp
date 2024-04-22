#include "Hajdu.h"
#include <psapi.h>
using namespace std;

int top = 0;
int mun = 0;

graph::graph(int x,int y)
{
    vertices = new vNode[x+1];
    visited = new int[x+1];
    stack = new arcNode[1001];
    arcNode *o = new arcNode;
    o->arctime = -1;
    stack[0].next = o;
    for(int i=1;i<=x;i++) {
        vertices[i].firstarc=NULL;
        vertices[i].vdata = i;
        visited[i]=0;
    }
    for(int i=1;i<=1000;i++){
        stack[i].next=NULL;
    }
    vexnum = x;
    arcnum = y;
}


graph::~graph(){
    delete vertices;
    delete visited;
    delete stack;
}

//创建邻接表子节点
void graph::makegraph(int a, int b,int c){
    arcNode* p = new arcNode ;
    p->arcdata=b;
    p->arctime=c;
    //头插入创建节点。
    p->next = vertices[a].firstarc;
    vertices[a].firstarc = p;
}

//输出邻接表
void graph::outgraph(){
    for(int i=1 ;i<=vexnum;i++){
        cout<< i<<"-";
        arcNode* p = vertices[i].firstarc;
        while (p!=NULL)
        {
            cout<< p->arcdata<<"."<<p->arctime<<"-";
            p=p->next;
        }
        cout<< endl;
    }
}
//遍历求环。
void graph::findall(){
    for(int startindex=1;startindex<=vexnum;startindex++){
        Findcycles(startindex,startindex);
    }
}


//枚举环路
void graph::Findcycles(int start,int cur){//主要的找环算法。
    //start为开始节点，用来判断是否已成环，cur是当前访问的节点。
    stack[++top].arcdata=cur;//压入栈。
    visited[cur]=2;
    while(top!=0){
        cur=stack[top].arcdata;
        arcNode* p;
        if(stack[top].next== NULL){
            p = vertices[cur].firstarc;
        }else{
            p = stack[top].next->next;
        }
        while(p!=NULL){
            int e = p->arcdata;
            if(e==start){///找到路径
                if(stack[top-1].next->arctime<p->arctime){
                    // cout<<"保存和输出环路"<<endl;
                    // for(int l = 1;l<top;l++){
                    //     outfile<<stack[l].arcdata<<" "<<stack[l].arctime<<" ";
                    // }
                    // outfile<<stack[top].arcdata<<" "<<p->arctime<<" "<<e<<endl;
                    mun++;
                }
            }else{
                if(visited[e]==0){//未被访问。
                    if(stack[top-1].next->arctime<p->arctime){
                        stack[top].next=p;
                        stack[top].arctime = p->arctime;
                        stack[++top].arcdata=e;
                        visited[e]=2;
                        break;
                    }
                }
            }
            p=p->next;
        }
        if(p==NULL){
            visited[cur]=0;
            stack[top].next=NULL;
            top--;//该节点访问完毕，退栈，找下一个节点
        }
    }
}

SIZE_T GetProcessMemoryUsage() {
    HANDLE hProcess = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}


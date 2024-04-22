#include<windows.h>
#include<fstream>
#include <psapi.h>
#include<iostream>
using namespace std;

extern int top  ;
extern int mun  ;  //保存环路数量

// 声明函数  用于获取当前进程的内存使用量
SIZE_T GetProcessMemoryUsage();

struct arcNode{    //邻接表子节点
    int arcdata;
    int arctime;
    arcNode* next;
};

struct vNode{   //邻接表列节点
    int vdata;
    arcNode* firstarc;
};

class graph
{
private:
    vNode* vertices;//邻接表结构
    int* visited;//记录访问过的节点
    arcNode* stack;//data存递归节点，next存访问到的子节点。
    int vexnum;
    int arcnum;
    void Findcycles(int a,int b);  //主体找环算法


public:
    graph(int x,int y); //
    ~graph();
    void makegraph(int a,int b,int c);// 构建图
    void outgraph();//输出邻接表
    void findall();//执行枚举算法
};


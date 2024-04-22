#include<iostream>
#include <atomic>
#include <thread>
#include<map>
#include <vector>
#include<windows.h>
using namespace std;


// 声明函数  用于获取当前进程的内存使用量
SIZE_T GetProcessMemoryUsage();

struct arcNode{
    int arcdata;
    int arctime;
//    int arcmoney;
    arcNode* next;
};
struct sarcNode{
    arcNode *next;
};
struct vNode{
    arcNode* firstarc;
};
class graph
{
private:


    int vexnum;
    int arcnum;;
    void tarjan(int a);//获取强连通分量
public:
    arcNode* stackt;//Tarjan模拟递归数组
    int *dfn, *low,*ins,*color,*stacks;//tarjan算法需要的标记数组
    int * visited;//是否访问数组

    int stackttop,cnt,ststop,stmp;//tarjan搜索所需的标记

    int *timeblock;//阻塞时间数组
    vNode *Blist;//阻塞列表
    sarcNode * Bstack ;//模拟递归解锁数组
    vNode* vertscc;//邻接表

    graph(int x,int y,int t);//x为节点数。y为边数.t为线程数量
    ~graph();

    //    void makescc(int a,int b,int c,int d);//构建邻接表
    void makescc(int a,int b,int c);//构建邻接表

    void findallcyc(int i);

    void findallscc();//每个节点执行tarjan

    void missfind(int i);

    void getdi(int i);//辅助线程获取任务
    int finits(int i);//辅助线程获取任务

//    bool recurFindcycles(int index,int getmoney,int getvex,int gettime,int* Btop,arcNode* Bstack,int* cyctime,bool flags);
    bool recurFindcycles(int index,int getvex,int gettime,int* Btop,arcNode* Bstack,int* cyctime);
    ///// index是起始节点，getvex是p的发出节点。gettime是上一条边的发生时间；

    void makeBlist(int a,int b,int time);//给Blist压入节点。

    void deleBlist(int a,int time);//清除掉blist上的节点。

    void unlock(int u,int* Btop,arcNode* Bstack,int overtime);//非递归的形式实现联级解锁

    bool absa(int a,int b);//差值比例
};

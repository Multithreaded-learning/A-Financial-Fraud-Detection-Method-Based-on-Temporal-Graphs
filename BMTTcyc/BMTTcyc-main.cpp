#include "BMTTcyc.h"
#include "Threadpool.h"
#include<iostream>
using namespace std;

extern int mode;
extern  vector<int> scc[];

int main(){

    SIZE_T initialMemory = GetProcessMemoryUsage();

    int N,M,T;
    cout<<"--Thread num:--"<<endl;
    cin>>T;//输入线程数量


    double BMMTcyc_run_time;
    LARGE_INTEGER time_start3;	//开始时间
    LARGE_INTEGER time_over3;	//结束时间
    double dqFreq3;		//计时器频率
    LARGE_INTEGER f3;	//计时器频率
    QueryPerformanceFrequency(&f3);
    dqFreq3=(double)f3.QuadPart;
    QueryPerformanceCounter(&time_start3);	//计时开始



    ifstream infile;
    ifstream infliree;
    infliree.open("C:\\Users\\13420\\Desktop\\data\\TLmoney.txt");
    if(!infile){
        cout<<"无法打开文件"<<endl;
        exit(1);
    }

    infliree>>N>>M;
    cout<<N<<"----"<<M<<endl;

    graph g(N,M,T);//创建节点图
    int a,b,c,d;
//    while(infliree>>a>>b>>c>>d){///有金额
//        g.makescc(a,b,c,d);
//    }
    while(infliree>>a>>b>>c>>d){
        g.makescc(a,b,c);//构建邻接表
    }

    g.findallscc();

    //重新构建强连通的邻接表
    for(int i=1;i<=N;i++){
        arcNode *l,*q,*t =NULL;
        l = g.vertscc[i].firstarc;
        while(l!=NULL){
            q = l->next;
            if(g.color[i]!=g.color[l->arcdata]){
                if(t == NULL){
                    g.vertscc[i].firstarc = q;
                    delete l;
                }else{
                    t->next = q;
                    delete l;
                }
            }else  t = l;
            l = q;
        }
    }


    QueryPerformanceCounter(&time_over3);	//计时结束
    BMMTcyc_run_time=(time_over3.QuadPart-time_start3.QuadPart)/dqFreq3;
    //乘以1000把单位由秒化为微秒，精度为1000/毫秒
    printf("makegraph time: %fs\n",BMMTcyc_run_time);



    ThreadPool p(mode);
    p.startPool();//启动线程池；

    size_t totalMemory = 0;
    totalMemory += sizeof(*scc);

    for(int i=1;i<=mode;i++){//先创建任务 使得多个线程排序好
        p.addTask(std::bind(&graph::getdi,&g,placeholders::_1),i);
    }

    p.times();

    for(int i=1;i<=g.cnt;i++){//创建任务
        if(scc[i].size()>1){
            p.addTask(std::bind(&graph::missfind,&g,placeholders::_1),i);
        }
    }


    // 在算法结束后获取最终内存使用量和结束时间点
    SIZE_T finalMemory = GetProcessMemoryUsage();

    // 计算算法使用的内存增量
    SIZE_T memoryUsage = finalMemory - initialMemory + totalMemory;
    double memUsedInMB = static_cast<double>(memoryUsage) / (1024 * 1024);
    std::cout << "Algorithm memory usage: " << memUsedInMB << " MB" << std::endl;



    return 0 ;
}

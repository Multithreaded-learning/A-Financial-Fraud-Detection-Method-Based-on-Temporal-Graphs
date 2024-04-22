#include "Hajdu.h"

using namespace std;

int main(){
    SIZE_T initialMemory = GetProcessMemoryUsage();
    int N,M;
    ifstream infile;
    infile.open("D:\\clionfile\\40000s.txt");
    if(!infile){
        cout<<"无法打开文件"<<endl;
        exit(1);
    }
    infile>>N>>M;
    graph g(N,M);
    int a,b,c,d;
    while(infile>>a>>b>>c){//输入文件流
        g.makegraph(a,b,c);//
    }
    // g.outgraph();

//  用于计算时间
    double run_time;
    LARGE_INTEGER time_start;	//开始时间
    LARGE_INTEGER time_over;	//结束时间
    double dqFreq;		//计时器频率
    LARGE_INTEGER f;	//计时器频率
    QueryPerformanceFrequency(&f);
    dqFreq=(double)f.QuadPart;
    QueryPerformanceCounter(&time_start);	//计时开始



    g.findall();//开始枚举算法


    cout<<endl;
    cout <<"have:"<<mun<<"cycle"<<endl;


    ///输出所用时间
    QueryPerformanceCounter(&time_over);	//计时结束
    run_time=(time_over.QuadPart-time_start.QuadPart)/dqFreq;
    printf("run_time:%fs\n",run_time);


    // 在算法结束后获取最终内存使用量和结束时间点
    SIZE_T finalMemory = GetProcessMemoryUsage();

    // 计算算法使用的内存增量
    SIZE_T memoryUsage = finalMemory - initialMemory;
    double memUsedInMB = static_cast<double>(memoryUsage) / (1024 * 1024);
    std::cout << "Algorithm memory usage: " << memUsedInMB << " MB" << std::endl;

    return 0 ;
}
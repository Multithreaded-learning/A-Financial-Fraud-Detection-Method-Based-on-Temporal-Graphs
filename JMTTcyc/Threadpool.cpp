#include "Threadpool.h"
#include "JMTTcyc.h"
using namespace std;


extern double JMMTcyc_run_time;

ThreadPool::ThreadPool(int num  )
        :m_ithreadNum(num)
{}

void ThreadPool::stopPool()
{
    m_bIsRunning = false;
    m_conDequeNotEmpty.notify_all();
    for (auto &item : m_vecThread)
    {
        //回收线程资源
        if (item.joinable())
        {
            item.join();
        }
    }
}


void ThreadPool::times()
{
    QueryPerformanceFrequency(&f);
    dqFreq=(double)f.QuadPart;
    QueryPerformanceCounter(&time_start);	//计时开始
}

//像任务队列中添加任务
void ThreadPool::addTask(Task task)
{
    if (m_bIsRunning)
    {
        //上锁，添加
        std::unique_lock<std::mutex> lck(m_mutex);
        m_deqTask.push_back(task);
        //通知任务队列不空
        m_conDequeNotEmpty.notify_one();
    }
}
//开启线程池，创建一定数量的线程，并绑定dotask函数
void ThreadPool::startPool()
{
    m_bIsRunning = true;
    for (int i = 0; i < m_ithreadNum; ++i)
    {
        std::thread t(&ThreadPool::doTask, this);
        m_vecThread.push_back(move(t));
    }
}

ThreadPool::~ThreadPool()
{
    m_bIsRunning = false;

    //通知所有线程不要再等待了
    m_conDequeNotEmpty.notify_all();
    for (auto &item : m_vecThread)
    {
//        回收线程资源
         if (item.joinable())
         {
        item.join();
         }
    }
    cout <<"have:"<<mun<<" cycles"<<endl;
    QueryPerformanceCounter(&time_over);	//计时结束
    Thread_run_time=(time_over.QuadPart-time_start.QuadPart)/dqFreq;
    printf("run_time: %fs\n",Thread_run_time);
}
void ThreadPool::doTask()
{
    //循环操作
    while (1)
    {
        //要获取的任务
        Task task;
        {
            std::unique_lock<std::mutex> lck(m_mutex);
            //等待任务队列不空或者状态为退出
            m_conDequeNotEmpty.wait(lck,[this]() {return !m_bIsRunning || !m_deqTask.empty(); });
            //不在运行状态且任务队列空了那就退出
            if (!m_bIsRunning&&m_deqTask.empty())
            {
                return;
            }
            //有任务就把顶端的拿出来
            task = move(m_deqTask.front());
            m_deqTask.pop_front();
        }
        //出了上边的作用域,lck就自动解锁了
        //执行任务
        task();
    }
}


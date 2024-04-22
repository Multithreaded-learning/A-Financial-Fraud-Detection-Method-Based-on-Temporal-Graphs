#include <deque>
#include <mutex>
#include <functional>
#include <thread>
#include <future>
#include<cstring>
#include<windows.h>
#include<fstream>
#include<iostream>
#include <climits>
using namespace std;


using Task = std::function<void()>;

extern  vector<int> scc[];
extern  atomic<int> mun;

class ThreadPool
{
public:

    //创建计时
    double Thread_run_time;
    LARGE_INTEGER time_start;	//开始时间
    LARGE_INTEGER time_over;	//结束时间
    double dqFreq;		//计时器频率
    LARGE_INTEGER f;	//计时器频率


    //构造函数，默认开启10个线程
    ThreadPool(int num=10);
    ~ThreadPool();
    //开启此线程池
    void stopPool();
    //关闭线程池
    void startPool();
    //像任务队列中增加任务
    void addTask(Task task);
    //线程执行任务的函数
    void doTask();
    void times();

//使可以添加任意形式的任务。
    template<typename F, typename...Args>
    auto addTask(F&& f, Args&&... args)
    {
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        std::function<void()> wrapperFunc = [=]() {
            (*task_ptr)();
        };

        m_deqTask.push_back(wrapperFunc);
        m_conDequeNotEmpty.notify_one();
        return task_ptr->get_future();
    }

private:
    std::condition_variable m_conDequeNotEmpty; //任务队列非空通知
    std::deque<Task> m_deqTask;     //任务队列
    std::mutex m_mutex;     //锁
    std::vector<std::thread> m_vecThread;   //线程存储在vec中
    int m_ithreadNum;   //线程数量
    std::atomic<bool> m_bIsRunning{ false };    //是否正在运行
};
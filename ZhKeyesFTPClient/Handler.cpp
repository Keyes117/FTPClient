#include "Handler.h"


#include <functional>
#include <memory>
#include <thread>


Handler::Handler()
{
}


//TODO: Windows系统其实自带一个线程池API，可以尝试使用


bool Handler::init(int8_t threadNum /*= 4*/)
{
    //TODO: 对threadCount 进行校验

    for (int i = 0; i < threadNum; i++)
    {
        auto spThread = std::make_shared<std::thread>(
            std::bind(&Handler::threadProc, this));

        m_threads.push_back(std::move(spThread));
    }
}

bool Handler::close()
{
    size_t threadCount = m_threads.size();
    for (int i = 0; i < threadCount; i++)
    {
        m_threads[i]->join();
    }
}

void Handler::threadProc()
{

    while(m_running)
    {
        std::unique_lock<std::mutex> guard(m_mutex);

        while (m_tasks.empty())
        {
            //如果获得了互斥锁， 但是条件不合适的话， pthread_cond_wait会释放锁不往下执行
            //当发生变化后，条件合适，pthread_cond_wait将直接获得锁
            m_cv.wait(guard);
        }

        pTask = m_task.front();
        m_task.pop_front();

        if (pTask == NULL)
            continue;

        pTask->doTask();
        delete pTask;
        pTask = NULL;
    }
    
}

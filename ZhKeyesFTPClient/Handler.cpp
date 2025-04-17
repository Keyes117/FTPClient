#include "Handler.h"


#include <functional>
#include <memory>
#include <thread>


Handler::Handler()
{
}


//TODO: Windowsϵͳ��ʵ�Դ�һ���̳߳�API�����Գ���ʹ��


bool Handler::init(int8_t threadNum /*= 4*/)
{
    //TODO: ��threadCount ����У��

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
            //�������˻������� �������������ʵĻ��� pthread_cond_wait���ͷ���������ִ��
            //�������仯���������ʣ�pthread_cond_wait��ֱ�ӻ����
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

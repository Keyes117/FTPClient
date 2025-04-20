#include "Handler.h"


#include <functional>
#include <memory>
#include <thread>


Handler::Handler()
{
}


//TODO: Windowsϵͳ��ʵ�Դ�һ���̳߳�API�����Գ���ʹ��


Handler& Handler::getInstance()
{
    static Handler handler;
    return handler;
}

bool Handler::init(int8_t threadNum /*= 4*/)
{
    //TODO: ��threadCount ����У��

    //TODO: ��ǰ���Ǽ�Ϊ�����̣߳�һ��UI->����->����  һ������->����->UI �����ڿ����̳߳�
    m_running = true;
    m_spSendThread = std::make_unique<std::thread>(std::bind(&Handler::sendThreadProc, this));
    m_spRecvThread = std::make_unique<std::thread>(std::bind(&Handler::recvThreadProc, this));

    return true;
}

void Handler::close()
{
    m_running = false;

    m_sendCV.notify_one();
    m_recvCV.notify_one();

    m_spRecvThread->join();
    m_spSendThread->join();

    LOGI("Handler::close() successfully ..... ");
}

void Handler::registerSendTask(std::shared_ptr<Task>&& task)
{
    std::lock_guard<std::mutex> guard(m_sendMutex);
    m_sendTasks.emplace_back(std::move(task));
    m_sendCV.notify_one();
}

void Handler::registerRecvTask(std::shared_ptr<Task>&& task)
{
    std::lock_guard<std::mutex> guard(m_recvMutex);
    m_recvTasks.emplace_back(std::move(task));
    m_recvCV.notify_one();
}

void Handler::sendThreadProc()
{
    LOGI("Handler::sendThreadProc()..... ");

    while (m_running)
    {
        std::unique_lock<std::mutex> guard(m_sendMutex);

        while (m_sendTasks.empty())
        {
            if (!m_running)
                return;
            //�������˻������� �������������ʵĻ��� pthread_cond_wait���ͷ���������ִ��
            //�������仯���������ʣ�pthread_cond_wait��ֱ�ӻ����
            m_sendCV.wait(guard);
        }

        auto pTask = m_sendTasks.front();
        m_sendTasks.pop_front();

        if (pTask == NULL)
            continue;

        pTask->doTask();
        m_sendTasks.pop_front();
        pTask.reset();
    }
}

void Handler::recvThreadProc()
{

    LOGI("Handler::recvThreadProc()..... ");
    while (m_running)
    {
        std::unique_lock<std::mutex> guard(m_recvMutex);

        while (m_recvTasks.empty())
        {
            if (!m_running)
                return;
            //�������˻������� �������������ʵĻ��� pthread_cond_wait���ͷ���������ִ��
            //�������仯���������ʣ�pthread_cond_wait��ֱ�ӻ����
            m_recvCV.wait(guard);
        }

        auto pTask = m_recvTasks.front();
        m_recvTasks.pop_front();

        if (pTask == NULL)
            continue;

        pTask->doTask();
        m_recvTasks.pop_front();
        pTask.reset();
    }
}

//void Handler::threadProc()
//{
//
//    while(m_running)
//    {
//        std::unique_lock<std::mutex> guard(m_mutex);
//
//        while (m_tasks.empty())
//        {
//            //�������˻������� �������������ʵĻ��� pthread_cond_wait���ͷ���������ִ��
//            //�������仯���������ʣ�pthread_cond_wait��ֱ�ӻ����
//            m_cv.wait(guard);
//        }
//
//        pTask = m_task.front();
//        m_task.pop_front();
//
//        if (pTask == NULL)
//            continue;
//
//        pTask->doTask();
//        delete pTask;
//        pTask = NULL;
//    }
//    
//}

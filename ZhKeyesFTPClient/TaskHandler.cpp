#include "TaskHandler.h"


#include <functional>
#include <memory>
#include <thread>


TaskHandler::TaskHandler()
{
}


//TODO: Windowsϵͳ��ʵ�Դ�һ���̳߳�API�����Գ���ʹ��


TaskHandler& TaskHandler::getInstance()
{
    static TaskHandler handler;
    return handler;
}

bool TaskHandler::init()
{


    //TODO: ��ǰ���Ǽ�Ϊ�����̣߳�һ��UI->����->����  һ������->����->UI �����ڿ����̳߳�
    m_running = true;
    m_spSendThread = std::make_unique<std::thread>(std::bind(&TaskHandler::sendThreadProc, this));
    m_spRecvThread = std::make_unique<std::thread>(std::bind(&TaskHandler::recvThreadProc, this));

    return true;
}

void TaskHandler::close()
{
    m_running = false;

    m_sendCV.notify_one();
    m_recvCV.notify_one();

    m_spRecvThread->join();
    m_spSendThread->join();

    LOGI("Handler::close() successfully ..... ");
}

void TaskHandler::registerSendTask(std::shared_ptr<Task>&& task)
{
    std::lock_guard<std::mutex> guard(m_sendMutex);
    m_sendTasks.emplace_back(std::move(task));
    m_sendCV.notify_one();
}

void TaskHandler::registerRecvTask(std::shared_ptr<Task>&& task)
{
    std::lock_guard<std::mutex> guard(m_recvMutex);
    m_recvTasks.emplace_back(std::move(task));
    m_recvCV.notify_one();
}

void TaskHandler::sendThreadProc()
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


        if (pTask == NULL)
            continue;

        pTask->doTask();
        m_sendTasks.pop_front();
        pTask.reset();
    }
}

void TaskHandler::recvThreadProc()
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

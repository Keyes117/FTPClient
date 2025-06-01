/**
 * @desc:   数据处理类，Handler.h
 * @author: ZhKeyes
 * @date:   2025/4/17
 */

#ifndef HANDLER_TASKHANDLER_H_
#define HANDLER_TASKHANDLER_H_

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <list>
#include <mutex>

#include "AsyncLog.h"

#include "Task.h"

class TaskHandler final
{

public:

    static TaskHandler& getInstance();

    bool init();
    void close();

    void registerSendTask(std::shared_ptr<Task>&& task);
    void registerRecvTask(std::shared_ptr<Task>&& task);

private:

    TaskHandler();
    ~TaskHandler() = default;

    void sendThreadProc();
    void recvThreadProc();

private:

    std::mutex                                  m_sendMutex;
    std::mutex                                  m_recvMutex;

    std::condition_variable                     m_sendCV;
    std::condition_variable                     m_recvCV;

    std::unique_ptr<std::thread>                m_spSendThread;
    std::unique_ptr<std::thread>                m_spRecvThread;

    std::list<std::shared_ptr<Task>>            m_sendTasks;
    std::list<std::shared_ptr<Task>>            m_recvTasks;

    //标记任务处理线程是否退出
    std::atomic<bool>                           m_running;
};







#endif  //! HANDLER_HANDLER_H_
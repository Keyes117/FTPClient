/**
 * @desc:   数据处理类，Handler.h
 * @author: ZhKeyes
 * @date:   2025/4/17
 */

#ifndef HANDLER_HANDLER_H_
#define HANDLER_HANDLER_H_

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <vector>

#include "Task.h"

class Handler
{
public:
    Handler();
    ~Handler() = default;


public:
    bool init(int8_t threadNum = 4);
    bool close();

private:
    void threadProc();

private:
    bool                                        m_running;
    std::mutex                                  m_mutex;
    std::vector<std::shared_ptr<std::thread>>   m_threads;
    std::vector<std::shared_ptr<Task>>          m_tasks;
    std::condition_variable                     m_cv;
};







#endif  //! HANDLER_HANDLER_H_
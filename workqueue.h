/*
    WorkQueue
    Copyright (c) 2015-2026, Chris Desjardins
    https://github.com/cdesjardins/ComBomb cjd@chrisd.info

    SPDX-License-Identifier: BSD-3-Clause
    See the LICENSE file at the project root for the full license text.
*/
#ifndef _WORK_QUEUE_Hxx
#define _WORK_QUEUE_Hxx

#include <functional>
#include <thread>
#include <memory>
#include "QueuePtr/ThreadSafeQueue.h"

template <typename T> class WorkQueue
{
public:
    typedef std::function<void (const std::shared_ptr<T>&)> WorkQueueCallback;

    static std::shared_ptr<WorkQueue<T> > createWorkQueue(WorkQueueCallback dataProcessorCallback)
    {
        std::shared_ptr<WorkQueue<T> > ret(new WorkQueue<T>(dataProcessorCallback));
        ret->_dataProcessorThread.reset(new std::thread(std::bind(&WorkQueue<T>::processDataThread, ret.get())));
        return ret;
    }

    virtual ~WorkQueue()
    {
        doneWithJobs();
    }

    bool processData(const std::shared_ptr<T>& data)
    {
        bool ret = true;
        _dataQueue.enqueue(data);
        return ret;
    }

    void doneWithJobs()
    {
        _running = false;
        if ((_dataProcessorThread) && (_dataProcessorThread->joinable()))
        {
            _dataProcessorThread->join();
        }
    }

protected:
    WorkQueue(WorkQueueCallback dataProcessorCallback)
        : _dataQueue(),
        _dataProcessorCallback(dataProcessorCallback),
        _running(true)
    {
    }

    void processDataThread()
    {
        std::vector<std::shared_ptr<T> > currentData;
        do
        {
            _dataQueue.dequeue(currentData, 1);
            processData(currentData);
            currentData.clear();
        } while (_running == true);
    }

    void processData(std::vector<std::shared_ptr<T> >& currentData)
    {
        for (std::size_t index = 0; index < currentData.size(); index++)
        {
            _dataProcessorCallback(currentData[index]);
            currentData[index].reset();
        }
    }

    std::shared_ptr<std::thread> _dataProcessorThread;
    ThreadSafeQueue<std::shared_ptr<T> > _dataQueue;
    WorkQueueCallback _dataProcessorCallback;
    bool _running;
};

#endif

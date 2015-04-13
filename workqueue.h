/*
    WorkQueue
    Copyright (C) 2015  Chris Desjardins
    http://blog.chrisd.info cjd@chrisd.info

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    typedef std::function<void (const std::shared_ptr<T> &)> WorkQueueCallback;

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

    bool processData(const std::shared_ptr<T> &data)
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

    void processData(std::vector<std::shared_ptr<T> > &currentData)
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


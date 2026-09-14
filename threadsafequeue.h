/*
    threadsafequeue
    Copyright (c) 2015-2026, Chris Desjardins
    https://github.com/cdesjardins/ComBomb cjd@chrisd.info

    SPDX-License-Identifier: BSD-3-Clause
    See the LICENSE file at the project root for the full license text.
*/

/*
** This provides a thread safe queue
*/

#ifndef _THREAD_SAFE_QUEUE_Hxx
#define _THREAD_SAFE_QUEUE_Hxx

#include <chrono>
#include <condition_variable>
#include <list>
#include <mutex>
#include <vector>

/*
** On the wait functions in this class if timeout == -1, then wait forever...
** When dequeing a batch of messages, the return code is the total number of bytes
** in all messages in the batch.
*/

template <class T> class ThreadSafeQueue
{
public:
    ThreadSafeQueue()
        : _queue(),
        _queueMutex(),
        _msgNotification(),
        _numEnqueued(0)
    {
    }

    virtual ~ThreadSafeQueue()
    {
    }

    virtual void enqueue(const T& data)
    {
        { // create a new scope for the mutex
            std::unique_lock<std::mutex> lock(_queueMutex);
            pushData(data);
            _msgNotification.notify_all();
        }
    }

    virtual bool dequeue(T& data, const int msTimeout = 0)
    {
        bool ret = false;
        { // create a new scope for the mutex
            std::unique_lock<std::mutex> lock(_queueMutex);
            waitForData(lock, msTimeout);
            ret = popData(data);
        }
        return ret;
    }

    // Dequeue everything
    virtual size_t dequeue(std::vector<T>& dataVec, const int msTimeout = 0)
    {
        size_t size = 0;
        { // create a new scope for the mutex
            std::unique_lock<std::mutex> lock(_queueMutex);
            waitForData(lock, msTimeout);
            size = popData(dataVec);
        }
        return size;
    }

    size_t size() const
    {
        return _numEnqueued;
    }

    /*
    ** This function allows you to perform operations on the
    ** vector in a thread safe way. The functor is a function
    ** with the following signature:
    ** int func(std::list<T> &);
    ** The return value is the number of elements added or removed
    ** from the list, for example if 5 elements were removed
    ** and 3 new elements were added then the return value should
    ** be -2.
    */
    template <typename Functor> void iterate(Functor functor)
    {
        { // create a new scope for the mutex
            std::unique_lock<std::mutex> lock(_queueMutex);
            // the return value of this functor is added to the _numEnqueued
            // so if you add buffers then return the number of buffers added
            // or if you remove buffers then return -number of buffers removed
            int numChanged = functor(_queue);
            _numEnqueued += numChanged;
        }
    }

protected:

    void waitForData(std::unique_lock<std::mutex>& lock, const int msTimeout)
    {
        if (msTimeout != 0)
        {
            // This function assumes that _queueMutex is locked already!
            std::chrono::system_clock::time_point timeLimit = std::chrono::system_clock::now() +
                                                              std::chrono::milliseconds(msTimeout);
            while (_queue.empty() == true)
            {
                // if timeout is specified, then wait until the time is up
                // otherwise wait forever (forever is msTimeout = -1)
                if (msTimeout > 0)
                {
                    _msgNotification.wait_until(lock, timeLimit);
                    if (std::chrono::system_clock::now() >= timeLimit)
                    {
                        break;
                    }
                }
                else
                {
                    _msgNotification.wait(lock);
                }
            }
        }
    }

    void pushData(const T& data)
    {
        // This function assumes that _queueMutex is locked already!
        _queue.push_back(data);
        _numEnqueued++;
    }

    bool popData(T& data)
    {
        // This function assumes that _queueMutex is locked already!
        bool ret = false;
        if (_queue.empty() == false)
        {
            data = _queue.front();
            _queue.pop_front();
            _numEnqueued--;
            ret = true;
        }
        return ret;
    }

    size_t popData(std::vector<T>& dataVec)
    {
        // This function assumes that _queueMutex is locked already!
        size_t size = 0;
        T data;
        while (popData(data) == true)
        {
            size += sizeOfData(data);
            dataVec.push_back(data);
        }
        return size;
    }

    virtual size_t sizeOfData(const T&) const
    {
        return sizeof(T);
    }

    std::list<T> _queue;
    std::mutex _queueMutex;
    std::condition_variable _msgNotification;
    size_t _numEnqueued;
};

#endif

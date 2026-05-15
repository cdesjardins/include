#ifndef CIRCULAR_BUFFER_Hxx
#define CIRCULAR_BUFFER_Hxx

#include <cstddef>
#include <deque>

template <typename T>
class CircularBuffer
{
public:
    explicit CircularBuffer(size_t capacity)
        : _capacity(capacity)
    {
    }

    void clear()
    {
        _data.clear();
    }

    size_t capacity() const
    {
        return _capacity;
    }

    size_t size() const
    {
        return _data.size();
    }

    void push_back(const T& value)
    {
        if (_capacity == 0)
        {
            return;
        }
        if (_data.size() == _capacity)
        {
            _data.pop_front();
        }
        _data.push_back(value);
    }

    // Shrink/grow capacity; on shrink, drop oldest entries (keep most recent).
    void rset_capacity(size_t newCapacity)
    {
        _capacity = newCapacity;
        while (_data.size() > _capacity)
        {
            _data.pop_front();
        }
    }

    T& operator[](size_t index)
    {
        return _data[index];
    }

    const T& operator[](size_t index) const
    {
        return _data[index];
    }

private:
    std::deque<T> _data;
    size_t _capacity;
};

#endif

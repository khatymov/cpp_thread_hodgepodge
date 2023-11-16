/*! \file string_buffer.h
 * \brief ReadWriteBuffer class interface.
 *
 * Buffer stores in a queue values that can read and write in different threads
 * (producer/consumer)
 *
 */
#pragma once

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>

extern std::mutex buffer_mutex;
extern std::condition_variable buffer_condition_var;
extern std::atomic_bool is_file_closed;

template <typename T>
class ReadWriteBuffer
{
public:
    void read(const T& val)
    {
        std::unique_lock<std::mutex> lock(buffer_mutex);
        while (!_queue.empty())
        {
            buffer_condition_var.wait(lock);
        }
        _queue.push(val);
        buffer_condition_var.notify_all();
    }

    bool have_write_lines()
    {
        std::unique_lock<std::mutex> lock(buffer_mutex);
        while (_queue.empty())
        {
            buffer_condition_var.wait(lock);
            if (is_file_closed)
            {
                return false;
            }
        }

        return true;
    }

    T get_line()
    {
        const T val = _queue.front();
        _queue.pop();
        buffer_condition_var.notify_all();
        return val;
    }

private:
    std::queue<T> _queue;
};

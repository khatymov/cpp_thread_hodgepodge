/*! \file queue.h
* \brief Thread safe queue handler that's implement producer consumer technique.
*/

#pragma once

#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <queue>

template<typename T = char>
class DataQueue
{
public:
    explicit DataQueue(size_t buffer_size)
        : _data(std::make_shared<std::vector<T>>(buffer_size))
        , _output_data(buffer_size)
    {};
    //! \brief assign buffer and put it into queue
    void set(const std::vector<T>& data, size_t bytes_read)
    {
        {
            std::unique_lock<std::mutex> unique_lock(_mutex);
            _condition_var.wait(unique_lock, [this]{ return _data_queue.empty(); });
            _data->assign(data.begin(), data.begin() + bytes_read);
            _data_queue.push(_data);
        }
        _condition_var.notify_all();
    }
    //! \brief pop buffer from a queue and return it
    std::vector<T> get()
    {
        {
            std::unique_lock<std::mutex> unique_lock(_mutex);
            _condition_var.wait(unique_lock, [this] { return !_data_queue.empty(); });
            _output_data = *_data_queue.front().get();
            _data_queue.pop();
        }
        _condition_var.notify_all();
        return _output_data;
    }
    //! \brief check that queue is empty
    bool is_empty()
    {
        std::lock_guard<std::mutex> lock_guard(_mutex);
        return _data_queue.empty();
    }

private:
    std::mutex _mutex;
    std::condition_variable _condition_var;
    std::queue<std::shared_ptr<std::vector<T>>> _data_queue;
    std::shared_ptr<std::vector<T>> _data;
    //! \brief  since we can overwrite the data when we do return _output_data;
    //! in the set method - for safety we use the second vector
    std::vector<T> _output_data;
};

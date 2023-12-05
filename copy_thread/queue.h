//
// Created by Khatymov on 11/23/2023.
//

#ifndef TEST_PTR_BUFFER_H
#define TEST_PTR_BUFFER_H

#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <queue>

template<typename T>
class DataQueue
{
public:
    explicit DataQueue(size_t buffer_size)
        : _data(std::make_shared<std::vector<T>>(buffer_size))
        , _output_data(buffer_size)
    {};

    void set(const std::vector<T>& data)
    {
        {
            std::unique_lock<std::mutex> unique_lock(_mutex);
            _condition_var.wait(unique_lock, [this]{ return _data_queue.empty(); });
            _data->assign(data.begin(), data.end());
            _data_queue.push(_data);

        }
        _condition_var.notify_all();
    }

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

private:
    std::mutex _mutex;
    std::condition_variable _condition_var;
    std::queue<std::shared_ptr<std::vector<T>>> _data_queue;
    std::shared_ptr<std::vector<T>> _data;
    // тк мы можем перезаписать данные когда делаем return _output_data;
    // в методе set - для подстраховки используем второй вектор
    std::vector<T> _output_data;
};


#endif //TEST_PTR_BUFFER_H

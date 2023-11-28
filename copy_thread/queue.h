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
//        i_set++;
        {
            std::unique_lock<std::mutex> unique_lock(_mutex);
            while (!_data_queue.empty())
            {
                _condition_var.wait(unique_lock);
            }
            _data->assign(data.begin(), data.end());
            _data_queue.push(_data);

        }
        is_empty = false;
        _condition_var.notify_all();
    }

    std::vector<T> get()
    {
//        i_get++;
        {
            std::unique_lock<std::mutex> unique_lock(_mutex);

            while (_data_queue.empty())
            {
                _condition_var.wait(unique_lock);
            }
            _output_data = *_data_queue.front().get();
            _data_queue.pop();
        }
        is_empty = true;
        _condition_var.notify_all();
        return _output_data;
    }

    std::atomic_bool is_empty{true};
    std::condition_variable _condition_var;
    std::queue<std::shared_ptr<std::vector<T>>> _data_queue;
    uint i_get{0};
    uint i_set{0};
private:
    std::mutex _mutex;

    std::shared_ptr<std::vector<T>> _data;
    // тк мы можем перезаписать данные когда делаем return _output_data;
    // в методе set - для подстраховки используем второй вектор
    std::vector<T> _output_data;
//    std::queue<std::shared_ptr<std::vector<T>>> _data_queue;
};


#endif //TEST_PTR_BUFFER_H

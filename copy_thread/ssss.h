#include <iostream>
#include <cstdint>
#include <vector>
#include <mutex>
#include <condition_variable>


struct Buffer {
    std::mutex _mtx;
    std::vector<uint8_t> _data;
};


Buffer a;
Buffer b;
Buffer c;
Buffer d;

std::queue<Buffer &> buff_queue;


std::atomic_bool file_is_over = false;

void writer() {
    file_is_over = false;
    while(file.is_okay())
    {
        {
            std::lock_guard<std::mutex> guard_a(a._mtx);
            a._data = file_read();
        }
        a._cv.notify_one();

        {
            std::lock_guard<std::mutex> guard_b(b._mtx);
            b._data = file_read();
        }

        b._cv.notify_one();
    }
    file_is_over = true;
}

void reader() {
    while(!file_is_over) {
        {
            std::unique_lock<std::mutex> guard_a(a._mtx);
            a._cv.wait(guard_a);
            a._data = file_read();
        }

        {
            std::unique_lock<std::mutex> guard_b(b._mtx);
            b._cv.wait(guard_b);
            b._data = file_read();
        }
    }
}



int main(int argc, char **argv) {
    return 0;
}
//
// Created by Khaty on 11/24/2023.
//

#ifndef CPP_COURSE_SSSS_H
#define CPP_COURSE_SSSS_H

#endif // CPP_COURSE_SSSS_H


#include <iostream>
#include <cstdint>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>


struct Buffer {
    void Buffer() {
        _is_free = true;
    }
    std::vector<uint8_t> _data;
    std::atomic_bool _is_free;
    std::mutex _mtx;
};


Buffer a;
Buffer b;



std::mutex mtx;
std::condition_variable cv;
std::queue<Buffer &> buff_queue;


std::atomic_bool file_is_over = false;

void reader() {
    file_is_over = false;

    Buffer &main = &a;
    Buffer &back = &b;

    while(file.is_okay())
    {
        if(main._is_free) {
            std::lock_guard<std::mutex> guard(mtx);
            main._data = file_read();
            main._is_free = false;
            buff_queue.push(&main);
            _cv.notify_one();
        }


        if(b._is_free)  {
            std::lock_guard<std::mutex> guard_b(b._mtx);
            b._is_free = false;
            b._data = file_read();
            _cv.notify_one();
        }

    }
    file_is_over = true;
}

void writer() {
    while(!file_is_over) {

        Buffer &buf;

        {
            std::unique_lock<std::mutex> guard(mtx);
            a._cv.wait(guard);
            buf = buff_queue.front();
            buff_queue.pop();
            if (buff._is_free == true) {
                throw new std::runtime_exception("OHOHOH!");
            }
        }

        {
            std::unique_lock<std::mutex> guard(buf._mtx);
            file.write(buff._data);
            buff.is_free = true;
        }
    }
}



int main(int argc, char **argv) {
    return 0;
}


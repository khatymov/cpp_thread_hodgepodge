//
// Created by Khatymov on 12/5/2023.
//

#ifndef CPP_COURSE_TIME_H
#define CPP_COURSE_TIME_H

#include <chrono>
#include <iostream>

class Timer
{
public:
    Timer()
        :start_time(std::chrono::system_clock::now())
    {}

    ~Timer()
    {
        // Calculate duration
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time);
        // Extract seconds and milliseconds
        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
        const auto milliseconds = duration - seconds;
        // Print the results
        std::cout << "Copy time: " << seconds.count() << "s " << milliseconds.count() << " ms\n";
    }
private:
    const std::chrono::time_point<std::chrono::system_clock> start_time;
};

#endif // CPP_COURSE_TIME_H

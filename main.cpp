/*! \file main.cpp
 * \brief Entry point.
 */

#include <iostream>
#include <iterator>
#include <vector>
#include <tuple>
#include <filesystem>
#include <thread>
#include "read_thread.h"
#include <fstream>


//TODO: change to boost `options`
auto parse_arguments(int argc, char* argv[]) {
    const std::vector<std::string_view> args(argv + 1, argv + argc);
    if (argc != 3)
    {
        throw std::runtime_error("Wrong number of arguments");
    }
    std::cout << "source_file_path: " << args[0] << "\ntarget_file_path: " << args[1] << std::endl;
    return make_tuple(args[0], args[1]);
}

namespace fs = std::filesystem;


#include <condition_variable>
#include <mutex>
#include <queue>
#include <cstdlib>
#include <cassert>
#include <atomic>

std::mutex _mutex;
std::condition_variable cv;

std::atomic_bool is_file_closed{false};

class StringBuffer {
public:
    void read(const std::string& val) {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_strings_queue.empty()) {
            cv.wait(lock);
        }
        std::cout << "Here 4" << std::endl;
        _strings_queue.push(val);
        std::cout << "Here 6" << std::endl;
        cv.notify_all();
        std::cout << "Here 7" << std::endl;
    }

    bool have_write_lines()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        std::cout << "Here 9" << std::endl;
        while (_strings_queue.empty())
        {
            std::cout << "Here 10" << std::endl;
            cv.wait(lock);
            if (is_file_closed)
            {
                return false;
            }
        }

        return true;
    }

    std::string get_line()
    {
        const std::string val = _strings_queue.front();
        _strings_queue.pop();
        std::cout << "Here 12" << std::endl;
        cv.notify_all();
        std::cout << "Here 13" << std::endl;
        return val;
    }

    auto write()
    {
        std::cout << "Here 8" << std::endl;
        std::unique_lock<std::mutex> lock(_mutex);
        std::cout << "Here 9" << std::endl;
        while (_strings_queue.empty())
        {
            std::cout << "Here 10" << std::endl;
            cv.wait(lock);
            if (is_file_closed) return std::string{};
        }

        std::cout << "Here 11" << std::endl;
        const std::string val = _strings_queue.front();
        _strings_queue.pop();
        std::cout << "Here 12" << std::endl;
        cv.notify_all();
        std::cout << "Here 13" << std::endl;
        return val;
    }
private:
    std::queue<std::string> _strings_queue;
};


void reader(StringBuffer* string_buffer, const std::string_view& source_file) {

    std::ifstream read_file(source_file.data());

    if (!read_file.is_open())
    {
        throw std::runtime_error("Can't open source.txt file");
    }

    // Read the file content line by line
    std::string line;

    while (std::getline(read_file, line)) {
        string_buffer->read(line);
    }
    std::cout << "Here 14" << std::endl;
    read_file.close();

    if (!read_file.is_open())
    {
        std::cout << "Here 15" << std::endl;
        is_file_closed = true;
        cv.notify_one();
    }
}

void writer(StringBuffer *string_buffer, const std::string_view& dist_file) {

    //TODO: verify that ostream creates a file
    // Open the file for writing
    std::ofstream write_file(dist_file.data());
    if (!write_file.is_open())
    {
        throw std::runtime_error("Can't open destination file");
    }

    while (string_buffer->have_write_lines())
    {
        write_file << string_buffer->get_line() << '\n';
    }

    write_file.close();

    assert(std::system("diff ../data/source.txt ../data/target.txt | exit $(wc -l)") == 0);
}

int main(int argc, char* argv[])
{
    std::string_view source_path;
    std::string_view target_path;

    try {
        std::tie(source_path, target_path) = parse_arguments(argc, argv);
        if (!fs::exists(source_path))
        {
            throw std::runtime_error("Source file doesn't exist or has a wrong path");
        }
        if (fs::is_empty(source_path))
        {
            std::cout << "Source file is empty. Nothing to copy." << std::endl;
            return 0;
        }

    } catch (const std::exception &x) {
        std::cerr << "threads_read_write: " << x.what() << '\n';
        std::cerr << "usage: threads_read_write source_file_path target_file_path\n";
        return EXIT_FAILURE;
    }

    StringBuffer stringBuffer;
    std::jthread read_thread(reader, &stringBuffer, source_path);
    std::jthread write_thread(writer, &stringBuffer, target_path);

    return 0;
}
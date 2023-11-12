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

using namespace std;

//TODO: change to boost `options`
auto parse_arguments(int argc, char* argv[]) {
    const std::vector<std::string_view> args(argv + 1, argv + argc);
    if (argc != 3)
    {
        throw std::runtime_error("Wrong number of arguments");
    }
    cout << "source_file_path: " << args[0] << "\ntarget_file_path: " << args[1] << endl;
    return make_tuple(args[0], args[1]);
}

namespace fs = std::filesystem;

void read_file(const string_view& source_file)
{
    ifstream read_file(source_file.data());

    if (!read_file.is_open())
    {
        throw std::runtime_error("Can't open source file");
    }

    // Read the file content line by line
    std::string line;
    while (std::getline(read_file, line)) {
        std::cout << line << std::endl;
    }

    read_file.close();
}

#include <condition_variable>
#include <mutex>
#include <queue>

mutex _mutex;
condition_variable cv;

class StringBuffer {
public:
    void read(const string& val) {
        unique_lock<mutex> lock(_mutex);
        while (!q.empty()) {
            cv.wait(lock);
        }
        q.push(val);
        cv.notify_all();
        cout << "Reader reads: " <<  val << endl;
    }

    auto write() {
        unique_lock<mutex> lock(_mutex);
        while (q.empty()) {
            cv.wait(lock);
        }
        const string val = q.front();
        q.pop();
        cv.notify_all();
        cout << "Writer writes " <<  val << endl;
        return val;
    }
private:
    queue<string> q;
};


void reader(StringBuffer* string_buffer, const string_view& source_file) {

    ifstream read_file(source_file.data());

    if (!read_file.is_open())
    {
        throw std::runtime_error("Can't open source file");
    }

    // Read the file content line by line
    std::string line;
    while (std::getline(read_file, line)) {
        std::cout << line << std::endl;
        string_buffer->read(line);
    }

    read_file.close();
}

void writer(StringBuffer *string_buffer, const string_view& dist_file) {

    //TODO: verify that ostream creates a file
    // Open the file for writing
    std::ofstream write_file(dist_file.data());
    if (!write_file.is_open())
    {
        throw std::runtime_error("Can't open destination file");
    }

    write_file << string_buffer->write();

    write_file.close();
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
            cout << "Source file is empty. Nothing to copy." << endl;
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
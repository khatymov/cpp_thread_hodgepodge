/*! \file copy_thread.cpp
 * \brief CopyInThreads class implementation.
 */

#include <iostream>

#include "copy_thread.h"

//https://www.cppstories.com/2023/five-adv-init-techniques-cpp/
// constinit
// make_unique_for_overwrite
constinit size_t buffer_size{1};

CopyInThreads::CopyInThreads(const std::string_view& source_path, const std::string_view& target_path)
    :_source_path(source_path.data())
    ,_target_path(target_path.data())
    ,_queue1(buffer_size)
    ,_queue2(buffer_size)
{}

void CopyInThreads::run()
{
    // exception pointer for read thread
    std::exception_ptr error_read;
    // move to class member
    std::atomic_bool is_file_over = false;

    //TODO: c++ hardware_destructive_interference_size,
    //Launch read thread
    std::thread read_thread(&CopyInThreads::_read, this, std::ref(is_file_over), std::ref(error_read));
    _write(std::ref(is_file_over));

    read_thread.join();

    // See if read/write thread has thrown any exception
    if (error_read)
    {
        std::cout << "Main thread received exception, rethrowing it..." << std::endl;
        rethrow_exception(error_read);
    }
}

void CopyInThreads::_read(std::atomic_bool& is_file_over, std::exception_ptr& err)
{
    try
    {
        //The abbreviation  "rb"  includes the representation of binary mode, as denoted by  b  code.
        std::FILE* f = std::fopen(_source_path.c_str(), "rb");

        if (f == nullptr){
            const std::string error = "Unable to open read file " + _source_path;
            throw std::runtime_error(error);
        }

        std::vector<char> buf(buffer_size);
        while (std::fread(&buf[0], sizeof buf[0], buf.size(), f) == 1)
        {
            _queue1.set(buf);
//            if (std::fread(&buf[0], sizeof buf[0], buf.size(), f) == 1)
//            {
//                _queue2.set(buf);
//            }
        }
        is_file_over = true;
//        auto ptr = std::make_shared<std::vector<char>>('a');
//        _queue1._data_queue.push(ptr);

        std::cout << "read_thread is finished" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//
//        std::cout << "queue_size = " << _queue1._data_queue.size() << std::endl;
//        std::cout << "set index = " << _queue1.i_set << std::endl;
//        _queue1._condition_var.notify_all();
        // Verify that we reached the end of the file and close it
//        if (!read_file.eof())
        {
            // We did not reach the end-of-file.
//            const std::string error = "Unable to finish reading file " + _source_path;
//            throw std::runtime_error(error);
        }
        fclose(f);
    } catch ( const std::exception& e )
    {
        err = std::current_exception();
    }
}

void print(const std::vector<char>& v)
{
    for (const auto& it: v)
        std::cout << it;
}

void CopyInThreads::_write(std::atomic_bool& is_file_over)
{
    try
    {
        std::cout << "Write thread start" << std::endl;
        std::FILE* f = std::fopen(_target_path.c_str(), "w");
        if (f == nullptr){
            const std::string error = "Unable to write to a file " + _target_path;
            throw std::runtime_error(error);
        }

        while (!is_file_over or !_queue1.is_empty)
        {
//            if (_queue1.is_empty)
//                continue;
            auto res1 = _queue1.get();
            fwrite(&res1[0], sizeof res1[0] , res1.size(), f);
            std::cout << res1[0];

//            if (_queue2.is_empty)
//                continue;
//            {
//                auto res2 = _queue2.get();
//                fwrite(&res2[0], sizeof res2[0], res2.size(), f);
//                std::cout << res2[0];
//            }
//            print(res1);
        }
        std::cout << "Write thread finished" << std::endl;
        fclose(f);
    } catch ( const std::exception& e )
    {
        std::cerr << e.what() << std::endl;
    }
}

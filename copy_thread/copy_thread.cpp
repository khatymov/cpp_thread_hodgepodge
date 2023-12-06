/*! \file copy_thread.cpp
 * \brief CopyInThreads class implementation.
 */

#include <iostream>

#include "copy_thread.h"

#include "timer.h"



CopyInThreads::CopyInThreads(const std::string_view& source_path, const std::string_view& target_path)
    :_source_path(source_path.data())
    ,_target_path(target_path.data())
{
}

void CopyInThreads::run()
{
    // exception pointer for read thread
    std::exception_ptr error_read;
    // TODO: move to class member?
    //Use these vars as flags that we can't read anymore from source file, which means that we finished
    std::atomic_bool is_first_buffer_over = false;
    std::atomic_bool is_second_buffer_over = false;

    //TODO: c++ hardware_destructive_interference_size,
    //Launch read thread
    {
        Timer timer;
        std::thread read_thread(&CopyInThreads::_read, this, std::ref(is_first_buffer_over), std::ref(is_second_buffer_over), std::ref(error_read));
        // We write to a target file via main thread
        _write(std::ref(is_first_buffer_over), std::ref(is_second_buffer_over));

        read_thread.join();
    }
    // See if read/write thread has thrown any exception
    if (error_read)
    {
        std::cout << "Main thread received exception, rethrowing it..." << std::endl;
        rethrow_exception(error_read);
    }
}

void CopyInThreads::_read(std::atomic_bool& is_first_buffer_over, std::atomic_bool& is_second_buffer_over, std::exception_ptr& err)
{
    try
    {
        //The abbreviation  "rb"  includes the representation of binary mode, as denoted by  b  code.
        std::FILE* read_file = std::fopen(_source_path.c_str(), "rb");

        if (read_file == nullptr){
            const std::string error = "Unable to open read file " + _source_path;
            throw std::runtime_error(error);
        }

        std::vector<char> buf(buffer_size);

        // Read data in chunks
        size_t read_bytes_1{0};
        size_t read_bytes_2{0};
        while (true)
        {
            read_bytes_1 = std::fread(&buf[0], sizeof buf[0], buf.size(), read_file);
            if (read_bytes_1 > 0)
            {
                _queue1.set(buf, read_bytes_1);
            }
            else
            {
                is_first_buffer_over = true;
                break;
            }

            read_bytes_2 = std::fread(&buf[0], sizeof buf[0], buf.size(), read_file);
            if (read_bytes_2 > 0)
            {
                _queue2.set(buf, read_bytes_2);
            }
            else
            {
                is_second_buffer_over = true;
                break;
            }
        }

        fclose(read_file);
    } catch ( const std::exception& e )
    {
        err = std::current_exception();
    }
}

void CopyInThreads::_write(std::atomic_bool& is_first_buffer_over, std::atomic_bool& is_second_buffer_over)
{
    try
    {
        std::FILE* write_file = std::fopen(_target_path.c_str(), "w");
        if (write_file == nullptr){
            const std::string error = "Unable to write to a file " + _target_path;
            throw std::runtime_error(error);
        }

        //Create once to assign vector from queue
        std::vector<char> result_buffer(buffer_size);
        while (true)
        {
            if (!is_first_buffer_over or !_queue1.is_empty())
            {
                result_buffer = _queue1.get();
                fwrite(&result_buffer[0], sizeof result_buffer[0] , result_buffer.size(), write_file);
            } else
            {
                break;
            }

            if (!is_second_buffer_over or !_queue2.is_empty())
            {
                result_buffer = _queue2.get();
                fwrite(&result_buffer[0], sizeof result_buffer[0], result_buffer.size(), write_file);
            } else
            {
                break;
            }
        }

        fclose(write_file);
    } catch ( const std::exception& e )
    {
        std::cerr << e.what() << std::endl;
    }
}

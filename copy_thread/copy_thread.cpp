/*! \file copy_thread.cpp
 * \brief CopyInThreads class implementation.
 */

#include <iostream>

#include "copy_thread.h"

#include "timer.h"

CopyInThreads::CopyInThreads(const std::string_view& source_path, const std::string_view& target_path)
    : _source_path(source_path.data()), _target_path(target_path.data())
{
}

void CopyInThreads::run()
{
    // Use these vars as flags that we can't read anymore from source file, which means that we finished reading
    std::atomic_bool is_first_buffer_over = false;
    std::atomic_bool is_second_buffer_over = false;

    // Launch read thread
    {
        // Calculate time using RAII
        Timer timer;
        std::thread read_thread(&CopyInThreads::_read, this, std::ref(is_first_buffer_over), std::ref(is_second_buffer_over));
        // We write to a target file via main thread
        _write(std::ref(is_first_buffer_over), std::ref(is_second_buffer_over));

        read_thread.join();
    }
}

void CopyInThreads::_read(std::atomic_bool& is_first_buffer_over, std::atomic_bool& is_second_buffer_over)
{
    // name the thread for debugging simplification
    pthread_setname_np(pthread_self(), "Read thread");

    // The abbreviation  "rb"  includes the representation of binary mode, as denoted by  b  code.
    std::FILE* read_file = std::fopen(_source_path.c_str(), "rb");

    if (read_file == nullptr)
    {
        const std::string error = "Unable to open read file " + _source_path;
        throw std::runtime_error(error);
    }

    // vector where we put our bytes and send it to the QueueHandler
    std::vector<char> byte_vector(buffer_size);

    // Read data in chunks
    size_t read_bytes_1{0};
    size_t read_bytes_2{0};
    while (true)
    {
        read_bytes_1 = std::fread(&byte_vector[0], sizeof byte_vector[0], byte_vector.size(), read_file);
        if (read_bytes_1 > 0)
        {
            _queue1.set(byte_vector, read_bytes_1);
        }
        else
        {
            is_first_buffer_over = true;
            break;
        }

        read_bytes_2 = std::fread(&byte_vector[0], sizeof byte_vector[0], byte_vector.size(), read_file);
        if (read_bytes_2 > 0)
        {
            _queue2.set(byte_vector, read_bytes_2);
        }
        else
        {
            is_second_buffer_over = true;
            break;
        }
    }

    fclose(read_file);
}

void CopyInThreads::_write(std::atomic_bool& is_first_buffer_over, std::atomic_bool& is_second_buffer_over)
{
    std::FILE* write_file = std::fopen(_target_path.c_str(), "w");
    if (write_file == nullptr)
    {
        const std::string error = "Unable to write to a file " + _target_path;
        throw std::runtime_error(error);
    }

    // Create once to assign vector from queue
    std::vector<char> result_vector(buffer_size);
    while (true)
    {
        // Added !_queue1.is_empty() because of the following situation:
        // read thread is filled the buffer in queue and set is_first_buffer_over=true
        // and we have something in buffer, BUT! since is_first_buffer_over=true we just skip writing
        if (!is_first_buffer_over or !_queue1.is_empty())
        {
            result_vector = _queue1.get();
            fwrite(&result_vector[0], sizeof result_vector[0], result_vector.size(), write_file);
        }
        else
        {
            break;
        }

        if (!is_second_buffer_over or !_queue2.is_empty())
        {
            result_vector = _queue2.get();
            fwrite(&result_vector[0], sizeof result_vector[0], result_vector.size(), write_file);
        }
        else
        {
            break;
        }
    }

    fclose(write_file);
}

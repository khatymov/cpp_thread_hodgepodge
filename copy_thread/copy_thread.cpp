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

    // Read data in chunks
    size_t read_bytes{0};
    while (true)
    {
        read_bytes = std::fread(&byte_vector[0], sizeof byte_vector[0], byte_vector.size(), read_file);
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

    while (true)
    {
    }

    fclose(write_file);
}

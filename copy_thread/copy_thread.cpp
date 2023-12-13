/*! \file copy_thread.cpp
 * \brief CopyInThreads class implementation.
 */

#include <iostream>

#include "copy_thread.h"

#include "timer.h"

CopyInThreads::CopyInThreads(const std::string_view& source_path, const std::string_view& target_path)
    : _source_path(source_path.data()), _target_path(target_path.data())
    , buffer_rotator(std::make_shared<BufferRotator>())
{
}

void CopyInThreads::run()
{
    // Launch read thread
    {
        // Calculate time using RAII
        Timer timer;
        std::thread read_thread(&CopyInThreads::_read, this);
        // We write to a target file via main thread
        _write();

        read_thread.join();
    }
}

void CopyInThreads::_read()
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
//    read_bytes = std::fread(&byte_vector[0], sizeof byte_vector[0], byte_vector.size(), read_file);
    FileData* file_data = nullptr;
    size_t read_bytes{1};
    //ATTENTION!!!!
    while (read_bytes)
    {
        file_data = buffer_rotator->get_available_buffer(BufferMode::read);
        file_data->size = std::fread(&file_data->data[0], sizeof file_data->data[0], buffer_size, read_file);
        read_bytes = file_data->size;
        buffer_rotator->transfer_buffer_to(file_data, BufferMode::write);
    }

    fclose(read_file);
}

void CopyInThreads::_write()
{
    std::FILE* write_file = std::fopen(_target_path.c_str(), "w");
    if (write_file == nullptr)
    {
        const std::string error = "Unable to write to a file " + _target_path;
        throw std::runtime_error(error);
    }

    FileData* file_data = nullptr;
    size_t write_bytes = 1;
    while (write_bytes)
    {
        file_data = buffer_rotator->get_available_buffer(BufferMode::write);
        write_bytes = file_data->size;
        fwrite(&file_data[0], sizeof file_data[0], file_data->size, write_file);
        buffer_rotator->transfer_buffer_to(file_data, BufferMode::read);
    }

    fclose(write_file);
}

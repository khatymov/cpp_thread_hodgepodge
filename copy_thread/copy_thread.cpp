/*! \file copy_thread.cpp
 * \brief CopyInThreads class implementation.
 */

#include <iostream>
#include <thread>

#include "copy_thread.h"

#include "timer.h"

CopyInThreads::CopyInThreads(const std::string_view& source_path, const std::string_view& target_path)
    : _source_path(source_path.data()), _target_path(target_path.data()), buffer_rotator(std::make_unique<BufferRotator>())
{
}

void CopyInThreads::run()
{
    // Calculate time using RAII
    Timer timer;
    std::thread read_thread(&CopyInThreads::_read, this);
    // We write to a target file via main thread
    _write();

    read_thread.join();
}

void CopyInThreads::_read()
{
    // name the thread for debugging simplification
    pthread_setname_np(pthread_self(), "Read thread");

    // The abbreviation  "rb"  includes the representation of binary mode, as denoted by  b  code.
    std::FILE* read_file = std::fopen(_source_path.c_str(), "rb");

    if (read_file == nullptr)
    {
        std::cerr << "Unable to open read file " + _source_path << "\n";
        return;
    }

    do
    {
        FileData* file_data = buffer_rotator->get_available_buffer(BufferMode::read);
        if (file_data == nullptr)
        {
            std::this_thread::yield();
            continue;
        }
        file_data->size = std::fread(&file_data->data, sizeof(char), buffer_size, read_file);
        const bool everything_done = file_data->size == 0;
        buffer_rotator->transfer_buffer_to(file_data, BufferMode::write);
        if (everything_done)
        {
            break;
        }
    } while (true);

    //    int i = 0;
    //
    //    for (; i < db.row_count(); ++i)
    //    {
    //        if (db.at(i))
    //        {
    //            ...
    //            ...
    //            // a lot of logic with db
    //            ...
    //            goto mark;
    //        }
    ////        usual processing
    //    }
    //
    //    for (; i < db.row_count(); ++i)
    //    {
    //        :mark;
    //        //  usual processing
    //    }

    fclose(read_file);
}

void CopyInThreads::_write()
{
    std::FILE* write_file = std::fopen(_target_path.c_str(), "w");
    if (write_file == nullptr)
    {
        const std::string error = "Unable to write to a file " + _target_path;
        std::cerr << "Unable to open read file " + _target_path << "\n";
        return;
    }

    FileData* file_data = nullptr;
    while (true)
    {
        file_data = buffer_rotator->get_available_buffer(BufferMode::write);
        if (file_data == nullptr)
        {
            std::this_thread::yield();
            continue;
        }
        fwrite(file_data->data, sizeof(char), file_data->size, write_file);
        if (file_data->size == 0)
        {
            break;
        }
        buffer_rotator->transfer_buffer_to(file_data, BufferMode::read);
    }

    fclose(write_file);
}

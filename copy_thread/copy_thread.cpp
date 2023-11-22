/*! \file copy_thread.cpp
 * \brief CopyInThreads class implementation.
 */

#include <iostream>

#include "copy_thread.h"

std::mutex buffer_mutex;
std::condition_variable buffer_cond_var;

//https://www.cppstories.com/2023/five-adv-init-techniques-cpp/
// constinit
// make_unique_for_overwrite
constinit size_t buffer_size{1};

CopyInThreads::CopyInThreads(const std::string_view& source_path, const std::string_view& target_path)
    : _source_path(source_path.data())
    , _target_path(target_path.data())
    , _read_buffer(std::make_unique_for_overwrite<char[]>(buffer_size))
    , _write_buffer(std::make_unique_for_overwrite<char[]>(buffer_size))
{}

void CopyInThreads::run()
{

    // exception pointer for read thread
    std::exception_ptr error_read;

    //TODO: c++ hardware_destructive_interference_size,
    //Launch read thread
    std::thread read_thread(&CopyInThreads::_read, this, std::ref(error_read));
    _write();
    read_thread.join();

    // See if read/write thread has thrown any exception
    if (error_read)
    {
        std::cout << "Main thread received exception, rethrowing it..." << std::endl;
        rethrow_exception(error_read);
    }
}

void CopyInThreads::_read(std::exception_ptr& err)
{
    try
    {
        // Open and prepare file
        std::ifstream read_file(_source_path, std::ios::binary);
        if (!read_file.is_open())
        {
            // We failed to open the file: throw an exception
            const std::string error = "Unable to open file " + _source_path;

            throw std::invalid_argument(error);
        }

        //disables skipping of leading whitespace
        read_file.unsetf(std::ios::skipws);
        // Read the file content line by line
        while (read_file)
        {
            //lock
            // сколько байт в буффере валидных

            read_file.read((char*)_read_buffer.get(), sizeof(char) * buffer_size);
            for (size_t i = 0; i < buffer_size; i++)
            {
                _write_buffer[i] = _read_buffer[i];
            }
        }


        // Verify that we reached the end of the file and close it
        if (!read_file.eof())
        {
            // We did not reach the end-of-file.
            const std::string error = "Unable to read file " + _source_path;
            throw std::runtime_error(error);
        }

        read_file.close();
    } catch ( ... )
    {
        err = std::current_exception();
    }
}

void CopyInThreads::_write()
{
    try
    {
        std::ofstream target_file(_target_path);
        if (!target_file.is_open())
        {
            // We failed to open the file: throw an exception
            const std::string error = "Unable to open file " + _target_path;
            throw std::invalid_argument(error);
        }

        while (true)
        {
            // lock

            target_file << _write_buffer;

            if (is_finished)
            {
                //buff written
                break;
            }
        }

        target_file.close();
    } catch ( ... )
    {}


}

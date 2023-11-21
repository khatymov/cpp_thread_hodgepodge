/*! \file copy_thread.cpp
 * \brief CopyInThreads class implementation.
 */

#include <cassert>

#include "copy_thread.h"

std::mutex buffer_mutex;
std::condition_variable buffer_condition_var;
std::atomic_bool is_file_closed{false};

CopyInThreads::CopyInThreads(const std::string_view& source_path, const std::string_view& target_path)
    : _source_path(source_path.data())
    , _target_path(target_path.data())
    , _rw_buf(new ReadWriteBuffer<std::string>())
{}

void CopyInThreads::run()
{
    // exception pointer for read thread
    std::exception_ptr error_read;
    // exception pointer for write thread
    std::exception_ptr error_write;

    // Launch read thread
    std::thread read_thread(&CopyInThreads::_read, this, std::ref(error_read));
    // Launch write thread
    std::thread write_thread(&CopyInThreads::_write, this, std::ref(error_write));

    read_thread.join();
    write_thread.join();

    // See if read/write thread has thrown any exception
    if (error_read or error_write)
    {
        std::cout << "Main thread received exception, rethrowing it..." << std::endl;
        if (error_read)
        {
            rethrow_exception(error_read);
        }

        if (error_write)
        {
            rethrow_exception(error_write);
        }
    }
}

void CopyInThreads::_read(std::exception_ptr& promise)
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

        // Read the file content line by line
        std::string line;

        while (std::getline(read_file, line))
        {
            _rw_buf->read(line);
        }

        // Verify that we reached the end of the file and close it
        if (!read_file.eof())
        {
            // We did not reach the end-of-file.
            const std::string error = "Unable to read file " + _source_path;
            throw std::runtime_error(error);
        }

        read_file.close();

        if (!read_file.is_open())
        {
            is_file_closed = true;
            buffer_condition_var.notify_one();
        }
    } catch ( ... )
    {
        promise = std::current_exception();
    }
}

void CopyInThreads::_write(std::exception_ptr& err)
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

        while (_rw_buf->have_write_lines())
        {
            target_file << _rw_buf->get_line() << '\n';
        }

        target_file.close();
    } catch ( ... )
    {
        err = std::current_exception();
    }

//    assert(std::system("diff ../data/source.txt ../data/target.txt | exit $(wc -l)") == 0);
}

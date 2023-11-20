/*! \file copy_thread.cpp
 * \brief CopyThread class implementation.
 */

#include <cassert>

#include "copy_thread.h"

std::mutex buffer_mutex;
std::condition_variable buffer_condition_var;
std::atomic_bool is_file_closed{false};

CopyThread::CopyThread(const std::string_view& source_path, const std::string_view& target_path)
    : _source_path(source_path.data())
    , _target_path(target_path.data())
    ,_rw_buf(new ReadWriteBuffer<std::string>())
{}

void CopyThread::run()
{
    _read_thread = std::jthread(&CopyThread::_read, this);
    _write_thread = std::jthread(&CopyThread::_write, this);
}

void CopyThread::_read()
{
    std::ifstream read_file(_source_path);

    if (!read_file.is_open())
    {
        // We failed to open the file: throw an exception
        const std::string error = "Unable to open file " + _source_path;
        throw std::invalid_argument(error);
    }

    // Read the file content line by line
    std::string line;

    //Read string one by one until the end
    while (std::getline(read_file, line)) {
        _rw_buf->read(line);
    }

    if (!read_file.eof()) {
        // We did not reach the end-of-file.
        // This means that some error occurred while reading the file.
        // Throw an exception.
        const std::string error = "Unable to read file " + _source_path;
        throw std::runtime_error(error);
    }

    read_file.close();

    if (!read_file.is_open())
    {
        is_file_closed = true;
        buffer_condition_var.notify_one();
    }
}

void CopyThread::_write()
{
    std::ofstream target_file(_target_path);
    //TODO: verify that ostream creates a file
    // Open the file for writing
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

    assert(std::system("diff ../data/source.txt ../data/target.txt | exit $(wc -l)") == 0);
}

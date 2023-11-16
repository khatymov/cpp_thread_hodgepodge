/*! \file copy_thread.cpp
 * \brief CopyThread class implementation.
 */

#include <cassert>

#include "copy_thread.h"

std::mutex buffer_mutex;
std::condition_variable buffer_condition_var;
std::atomic_bool is_file_closed{false};

CopyThread::CopyThread(const std::string_view& source_path, const std::string_view& target_path)
    :_read_file(source_path.data())
    ,_target_file(target_path.data())
    ,_rw_buf(new ReadWriteBuffer<std::string>())
{}

void CopyThread::run()
{
    _read_thread = std::jthread(&CopyThread::_read, this);
    _write_thread = std::jthread(&CopyThread::_write, this);
}

void CopyThread::_read()
{
    if (!_read_file.is_open())
    {
        throw std::runtime_error("Can't open source.txt file");
    }

    // Read the file content line by line
    std::string line;

    while (std::getline(_read_file, line)) {
        _rw_buf->read(line);
    }
    _read_file.close();

    if (!_read_file.is_open())
    {
        is_file_closed = true;
        buffer_condition_var.notify_one();
    }
}

void CopyThread::_write()
{
    //TODO: verify that ostream creates a file
    // Open the file for writing
    if (!_target_file.is_open())
    {
        throw std::runtime_error("Can't open target file");
    }

    while (_rw_buf->have_write_lines())
    {
        _target_file << _rw_buf->get_line() << '\n';
    }

    _target_file.close();

    assert(std::system("diff ../data/source.txt ../data/target.txt | exit $(wc -l)") == 0);
}

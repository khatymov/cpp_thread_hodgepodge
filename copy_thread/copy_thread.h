/*! \file copy_thread.h
 * \brief CopyThread class interface.
 *
 * Class description.
 *
 */
#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <thread>

#include "read_write_buffer.h"

/*! \class CopyThread
 * \brief Some briefing
 */
class CopyThread
{
public:
    //! \brief default constructor.
    CopyThread(const std::string_view& source_path, const std::string_view& target_path);

    void run();

    //! \brief default destructor.
    ~CopyThread() = default;

private:
    //! List of private variables.
    std::ifstream _read_file;
    std::ofstream _target_file;

    std::shared_ptr<ReadWriteBuffer<std::string>> _rw_buf;

    std::jthread _read_thread;
    std::jthread _write_thread;

    void _read();
    void _write();
};

/*! \file copy_thread.h
 * \brief CopyInThreads class interface.
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

/*! \class CopyInThreads
 * \brief Read a file in one thread and copy that data in the second thread to target file
 */
class CopyInThreads
{
public:
    //! \brief default constructor.
    CopyInThreads(const std::string_view& source_path, const std::string_view& target_path);

    //! \brief run coping a file in 2 threads
    void run();

    //! \brief default destructor.
    ~CopyInThreads() = default;

private:

    //! \brief source file path
    std::string _source_path;
    //! \brief target file path
    std::string _target_path;

    //! \brief smart pointer which points on a safe thread buffer
    std::shared_ptr<ReadWriteBuffer<std::string>> _rw_buf;

    //! \brief a thread in which the source file is read into a buffer
    std::jthread _read_thread;
    //! \brief a thread in which the target file is written from a buffer
    std::jthread _write_thread;

    //! \brief used by _read_thread to read from source file
    void _read();
    //! \brief used by _write_thread to write to target file
    void _write();
};

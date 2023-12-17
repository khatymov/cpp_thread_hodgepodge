/*! \file copy_thread.h
 * \brief CopyInThreads class interface.
 *
 * Class description.
 *
 */
#pragma once

#include <string>
#include <thread>
#include <memory>

#include "buffer_rotator.h"

/*! \class CopyInThreads
 * \brief Read a file in one thread and copy that data in the second thread to target file
 */
class CopyInThreads
{
public:
    //! \brief default constructor.
    CopyInThreads(const std::string_view& source_path, const std::string_view& target_path);

    //! \brief run coping a file in 2 threads (main and child)
    void run();

    //! \brief default destructor.
    ~CopyInThreads() = default;

private:
    //! \brief buffer handler is shared between read and write thread
    std::unique_ptr<BufferRotator> buffer_rotator;
    //! \brief source file path
    std::string _source_path;
    //! \brief target file path
    std::string _target_path;

    //! \brief used by read_thread to read from source file
    void _read();

    //! \brief used by main_thread to write to target file
    void _write();
};

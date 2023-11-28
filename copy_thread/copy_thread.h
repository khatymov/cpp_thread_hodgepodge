/*! \file copy_thread.h
 * \brief CopyInThreads class interface.
 *
 * Class description.
 *
 */
#pragma once

#include <string>
#include <thread>

#include "queue.h"

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

    //! \brief source file path
    std::string _source_path;
    //! \brief target file path
    std::string _target_path;

    // Add description
    DataQueue<char> _queue1;
    // Add description
    DataQueue<char> _queue2;

    //! \brief used by read_thread to read from source file
    void _read(std::atomic_bool& is_first_buffer_over, std::atomic_bool& is_second_buffer_over, std::exception_ptr& err);

    //! \brief used by main_thread to write to target file
    void _write(std::atomic_bool& is_file_over, std::atomic_bool& is_second_buffer_over);
};

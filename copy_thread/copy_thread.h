/*! \file copy_thread.h
 * \brief CopyInThreads class interface.
 *
 * Class description.
 *
 */
#pragma once

#include <fstream>
#include <future>
#include <memory>
#include <string>
#include <thread>






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

//    char* buffer = std::make_unique_for_overwrite<T[]>(num);
    // TODO: Why ptr char, not vector?
    // TODO: make separate buffer class
    std::unique_ptr<char[]> _read_buffer;
    std::unique_ptr<char[]> _write_buffer;

    //! \brief used by read_thread to read from source file
    void _read(std::exception_ptr& err);

    //! \brief used by main_thread to write to target file
    void _write();
};

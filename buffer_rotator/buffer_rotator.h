/*! \file buffer_rotator.h
 * \brief BufferRotator class interface.
 *
 * Class description.
 *
 */


#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>

const size_t buffer_size = 1024*256;


/*! \class FileData
 * \brief Data that we read from a file and get the size how many bytes we read
 */
struct FileData
{
    char data[buffer_size];
    size_t size = 0;
};

/*! \class BufferMode
 * \brief Choose current mode: write or read
 */
enum class BufferMode : int
{
    read,
    write
};

/*! \class BufferRotator
 * \brief Some briefing
 */
class BufferRotator
{
    BufferRotator (const BufferRotator&) = delete;
    BufferRotator (BufferRotator&&) = delete;
    BufferRotator& operator = (const BufferRotator&) = delete;
    BufferRotator& operator = (BufferRotator &&) = delete;
public:

    //! \brief default constructor.    
    BufferRotator();

    //! \brief default destructor.
    ~BufferRotator();
    //! \brief get buffer according to buffer mode (from  _read_buffers or _write_buffers)
    FileData* get_available_buffer(const BufferMode mode);

    //! \brief give buffer to another thread (read or write)
    void transfer_buffer_to(FileData* buffer_data, const BufferMode mode);

protected:
    std::queue<FileData*>* get_buffer_from_queue(const BufferMode mode);
private:
    //! \brief buffers with data
    FileData _buffer_data[2];

    //! \brief thread synchronization primitives
    //! // to prevent from access to queue
    std::mutex _mutex;

    //! \brief queues with buffers
    std::queue<FileData*> _read_buffers;
    std::queue<FileData*> _write_buffers;
};

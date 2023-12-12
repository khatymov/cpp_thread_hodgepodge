/*! \file buffer_rotator.cpp
 * \brief BufferRotator class implementation.
 */

#include <thread>

#include "buffer_rotator.h"

using namespace std;

BufferRotator::BufferRotator()
{
    _read_buffers.push(&_buffer_data[0]);
    _write_buffers.push(&_buffer_data[1]);
}

FileData* BufferRotator::get_available_buffer(const BufferMode mode)
{
    //в зависимости от типа получить буфер либо из очереди на чтение,
    //либо на запись
    FileData* data = nullptr;
    std::queue<FileData*>* current_queue = get_buffer_from_queue(mode);
    std::lock_guard<std::mutex> lock_guard(_guard);
    if (!current_queue->empty())
    {
        data = current_queue->front();
        current_queue->pop();
    }
    std::this_thread::yield();
    return data;
}

void BufferRotator::transfer_buffer(FileData* buffer_data, const BufferMode mode)
{
    std::lock_guard<std::mutex> lock_guard(_guard);
    std::queue<FileData*>* current_queue = get_buffer_from_queue(mode);
    current_queue->push(buffer_data);
    std::swap(_read_buffers, _write_buffers);
}

std::queue<FileData*>* BufferRotator::get_buffer_from_queue(const BufferMode mode)
{
    return mode == BufferMode::read ? &_read_buffers : &_write_buffers;
}
BufferRotator::~BufferRotator()
{
}

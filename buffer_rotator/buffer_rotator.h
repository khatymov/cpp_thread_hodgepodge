/*! \file buffer_rotator.h
 * \brief BufferRotator class interface.
 *
 * Class description.
 *
 */


#pragma once

#include <mutex>
#include <queue>


struct FileData
{
    char data[1024*256];
    size_t size = 0;
};

/*! \class BufferRotator
 * \brief Some briefing
 */
class BufferRotator
{
public:

    //! \brief default constructor.    
    BufferRotator();

    //! \brief default destructor.
    ~BufferRotator() = default;

private:

    //! List of private variables.

};

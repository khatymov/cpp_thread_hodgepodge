/*! \file main.cpp
 * \brief Entry point.
 */

#include <filesystem>
#include <iostream>
#include <span>

#include "copy_thread.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: ./cpp_course path_to_source path_to_target." << std::endl;
        return EXIT_FAILURE;
    }

    // https://en.cppreference.com/w/cpp/container/span
    // warning: 'do not use pointer arithmetic'
    auto args = std::span(argv, size_t(argc));

    const std::string_view source_path{args[1]};
    const std::string_view target_path{args[2]};

    if (!fs::exists(source_path.data()))
    {
        std::cout << "Source file doesn't exist." << std::endl;
        return EXIT_FAILURE;
    }

    // initialize CopyInThreads class object
    CopyInThreads copy_in_threads(source_path, target_path);

    try
    {
        //execute copy in threads
        copy_in_threads.run();
    } catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
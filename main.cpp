/*! \file main.cpp
 * \brief Entry point.
 */

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <tuple>

#include "copy_thread.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: ./cpp_course path_to_source path_to_target." << std::endl;
        return EXIT_FAILURE;
    }

    std::string_view source_path{argv[1]};
    std::string_view target_path{argv[2]};

    if (!fs::exists(source_path.data()))
    {
        std::cout << "Source file doesn't exist." << std::endl;
        return EXIT_FAILURE;
    }

    CopyThread copy_thread(source_path, target_path);

    try
    {
        copy_thread.run();
    } catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
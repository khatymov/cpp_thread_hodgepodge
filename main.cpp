/*! \file main.cpp
 * \brief Entry point.
 */

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <tuple>
#include <vector>

#include "copy_thread.h"

//TODO: change to boost `options`
auto parse_arguments(int argc, char* argv[]) {
    const std::vector<std::string_view> args(argv + 1, argv + argc);
    if (argc != 3)
    {
        throw std::runtime_error("Wrong number of arguments");
    }
    std::cout << "source_file_path: " << args[0] << "\ntarget_file_path: " << args[1] << std::endl;
    return make_tuple(args[0], args[1]);
}

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    std::string_view source_path{};
    std::string_view target_path{};

    try {
        std::tie(source_path, target_path) = parse_arguments(argc, argv);
        if (!fs::exists(source_path))
        {
            throw std::runtime_error("Source file doesn't exist or has a wrong path");
        }
        if (fs::is_empty(source_path))
        {
            std::cout << "Source file is empty. Nothing to copy." << std::endl;
            return 0;
        }

    } catch (const std::exception &exception) {
        std::cerr << "threads_read_write: " << exception.what() << '\n';
        std::cerr << "usage: threads_read_write source_file_path target_file_path\n";
        return EXIT_FAILURE;
    }

    CopyThread copy_thread(source_path, target_path);

    copy_thread.run();

    return 0;
}
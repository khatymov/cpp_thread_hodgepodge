cmake_minimum_required(VERSION 3.2)
project(threads_read_write LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_CURRENT_LIST_DIR}/copy_thread/copy_thread.cmake)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

add_executable(${PROJECT_NAME}
                main.cpp
                ${COPY_THREAD}
)

target_link_libraries(${PROJECT_NAME} -pthread)

#target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)

#add_subdirectory(tests)

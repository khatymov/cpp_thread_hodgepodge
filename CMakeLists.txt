cmake_minimum_required(VERSION 3.2)
project(cpp_course LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_CURRENT_LIST_DIR}/copy_thread/copy_thread.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/buffer_rotator/buffer_rotator.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/timer/timer.cmake)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

add_executable(${PROJECT_NAME}
                main.cpp
                ${TIMER}
                ${COPY_THREAD}
                ${BUFFER_ROTATOR}
)

target_link_libraries(${PROJECT_NAME} -pthread)

#target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)

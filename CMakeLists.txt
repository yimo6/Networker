cmake_minimum_required(VERSION 3.10)
project(YNetworker)

set(CMAKE_C_STANDARD 99)


include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/libs/include)

set(LIBS_SOURCES
    libs/src/buffer.c
    libs/src/container.c
    libs/src/list.c
    libs/src/mem.c
)

set(CORE_SOURCES
    src/app.c
    src/connectionPool.c
    src/process.c
    src/public.c
    src/work.c
    src/worker.c
)

add_library(COMMON_LIBS ${LIBS_SOURCES})
add_library(CORE_LIBS ${CORE_SOURCES})
add_library(SYSTEM_LIBS INTERFACE)

if(WIN32)
    target_link_libraries(SYSTEM_LIBS INTERFACE ws2_32)
endif()

add_executable(YNetworker examples/main.c)
target_link_libraries(YNetworker PRIVATE CORE_LIBS)
target_link_libraries(YNetworker PRIVATE COMMON_LIBS)
target_link_libraries(YNetworker PRIVATE SYSTEM_LIBS)

install(TARGETS YNetworker DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include DESTINATION include)

add_executable(WorkerTest tests/test_worker.c)
target_link_libraries(WorkerTest PRIVATE CORE_LIBS)
target_link_libraries(WorkerTest PRIVATE COMMON_LIBS)
target_link_libraries(WorkerTest PRIVATE SYSTEM_LIBS)

add_executable(bufTest tests/test_buffer.c)
target_link_libraries(bufTest PRIVATE COMMON_LIBS)

add_test(NAME WorkerTest COMMAND WorkerTest)
add_test(NAME BufferTest COMMAND bufTest)
add_compile_options(-Wall -Wextra -Wpedantic)

enable_testing()

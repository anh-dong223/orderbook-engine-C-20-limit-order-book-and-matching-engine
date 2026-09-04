cmake_minimum_required(VERSION 3.20)
project(orderbook_engine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(engine
    src/order_book.cpp
    src/matching_engine.cpp
)
target_include_directories(engine PUBLIC include)
target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic)

add_executable(orderbook src/main.cpp)
target_link_libraries(orderbook PRIVATE engine)

add_executable(engine_tests tests/order_book_test.cpp)
target_link_libraries(engine_tests PRIVATE engine)

add_executable(engine_bench bench/throughput.cpp)
target_link_libraries(engine_bench PRIVATE engine)

include(CTest)
add_test(NAME engine_tests COMMAND engine_tests)

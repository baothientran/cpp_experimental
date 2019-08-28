cmake_minimum_required(VERSION 3.0.0)
project(experimental)

set(CMAKE_CXX_STANDARD 17)

add_executable(experimental  
                    "lib/catch.hpp"
                    "src/experimental.hpp"
                    "src/any.hpp"
                    "src/type_lists.hpp"
                    "src/hierarchy_generator.hpp"
                    "test/experimental_test.cpp"
                    "test/hierarchy_generator_test.cpp"
                    "test/main.cpp"
                    "test/any_test.cpp"
                    "test/type_lists_test.cpp")

target_include_directories(experimental PUBLIC ${PROJECT_SOURCE_DIR})
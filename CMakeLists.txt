cmake_minimum_required (VERSION 3.9)

project (cpptest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(USE_ABSEIL "Use Abseil" OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# platform compile flags
if(APPLE)
    add_definitions(-Wall)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
endif(APPLE)
if(MSVC)
    add_definitions(/D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4244 /wd4305 /wd4800 /wd4267)
    set(CMAKE_CXX_FLAGS "/EHsc")
endif(MSVC)

include_directories(ext/robin-hood-hashing/src/include)
include_directories(ext/abseil-cpp)

add_executable(streamspeed streamspeed.cpp)
add_executable(valuesemantic valuesemantic.cpp)
add_executable(hashmap hashmap.cpp)

if(USE_ABSEIL)
find_package(absl REQUIRED)
target_link_libraries(hashmap absl::city absl::hash absl::raw_hash_set)
endif(USE_ABSEIL)
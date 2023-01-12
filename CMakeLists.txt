if (POLICY CMP0048)
  cmake_policy (SET CMP0048 NEW)
endif ()

project("FalconReplay" LANGUAGES CXX)

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

# Add Gflags to project as dependency from github link
include(FetchContent)

FetchContent_Declare(
  gflags
  GIT_REPOSITORY https://github.com/gflags/gflags.git
  GIT_TAG v2.2.2
  GIT_SHALLOW ON
)

FetchContent_Declare(
  rapidcsv
  GIT_REPOSITORY https://github.com/d99kris/rapidcsv.git
  GIT_TAG v8.69
  GIT_SHALLOW ON
)

FetchContent_Declare(
  csv2
  GIT_REPOSITORY https://github.com/p-ranav/csv2.git
  GIT_TAG v0.1
  GIT_SHALLOW ON
)

FetchContent_MakeAvailable(gflags rapidcsv csv2)

# Add denbug flag
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wpedantic -Werror -O3")

file(GLOB ALL_SRC src/*.cc)
file(GLOB ALL_INC src/*.h)

# compile target falcon_replay.cc
add_executable(falcon_replay falcon_replay.cc)
target_include_directories(falcon_replay PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  rapidcsv
  csv2
)
target_link_libraries(falcon_replay PRIVATE gflags rapidcsv csv2)

add_executable(simple_udp_server simple_udp_server.cc)
target_include_directories(simple_udp_server PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  rapidcsv
  csv2
)
target_link_libraries(simple_udp_server PRIVATE gflags rapidcsv csv2)



cmake_minimum_required(VERSION 3.21)

# Set the project name and language
project(
  hazard-ptr-exmaples
  VERSION 0.0.1
  DESCRIPTION "Exemplary Usages of Hazard Pointers"
  LANGUAGES CXX C
)

list(APPEND CMAKE_PREFIX_PATH "/home/philipp.schubert/Repos/folly-build/installed/folly/lib/cmake")
list(APPEND CMAKE_PREFIX_PATH "/home/philipp.schubert/Repos/folly-build/installed/fmt-mQUBO60ALSflQR3FYlxfFlyQQeVTn4YoWYGK3OCBTWk/lib/cmake")

find_package(Boost REQUIRED)
find_package(folly REQUIRED)
find_package(Threads REQUIRED)
find_package(gflags REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address,undefined")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address,undefined")

set_and_check(FOLLY_INCLUDE_DIR /home/philipp.schubert/Repos/folly-build/installed/folly/include/folly)
set_and_check(FOLLY_CMAKE_DIR /home/philipp.schubert/Repos/folly-build/installed/folly/lib/cmake/folly)
if (NOT TARGET Folly::folly)
  include("${FOLLY_CMAKE_DIR}/folly-targets.cmake")
endif()

set(FOLLY_LIBRARIES Folly::folly)

if (NOT folly_FIND_QUIETLY)
  message(STATUS "Found folly: ${PACKAGE_PREFIX_DIR}")
endif()

add_executable(
  main
  main.cpp
)
target_link_libraries(
  main
  ${Boost_LIBRARIES}
  ${FOLLY_LIBRARIES}
)

cmake_minimum_required(VERSION 3.21)

project(
  hazard-ptr-exmaples
  VERSION 0.0.1
  DESCRIPTION "Exemplary Usages of Hazard Pointers"
  LANGUAGES CXX C
)

set(FOLLY_INSTALL_DIR "/home/philipp.schubert/Repos/folly-build/installed")

list(APPEND CMAKE_PREFIX_PATH "${FOLLY_INSTALL_DIR}/folly/lib/cmake")
list(APPEND CMAKE_PREFIX_PATH "${FOLLY_INSTALL_DIR}/fmt-mQUBO60ALSflQR3FYlxfFlyQQeVTn4YoWYGK3OCBTWk/lib/cmake")

find_package(Boost REQUIRED)
find_package(folly REQUIRED)
find_package(Threads REQUIRED)
find_package(gflags REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

set(USE_SANITIZER "" CACHE STRING "Use sanitizers (possible values: 'address,undefined' or 'thread')")

string(TOLOWER ${CMAKE_BUILD_TYPE} LOWERCASE_CMAKE_BUILD_TYPE)
set(CANNOT_USE_SAN_IN_NON_DBG_BUILD_MSG "Cannot use sanitizers in non-debug build!")

if (("${USE_SANITIZER}" STREQUAL "address,undefined") OR
    ("${USE_SANITIZER}" STREQUAL "undefined,address"))
  if (NOT "${LOWERCASE_CMAKE_BUILD_TYPE}" STREQUAL "debug")
    message(FATAL_ERROR "${CANNOT_USE_SAN_IN_NON_DBG_BUILD_MSG}")
  endif()
  message(STATUS "Building with address and undefined behavior sanitizer")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address,undefined")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address,undefined")
endif()

if ("${USE_SANITIZER}" STREQUAL "thread")
  if (NOT "${LOWERCASE_CMAKE_BUILD_TYPE}" STREQUAL "debug")
    message(FATAL_ERROR "${CANNOT_USE_SAN_IN_NON_DBG_BUILD_MSG}")
  endif()
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")
endif()


set_and_check(FOLLY_INCLUDE_DIR "${FOLLY_INSTALL_DIR}/folly/include/folly")
set_and_check(FOLLY_CMAKE_DIR ${FOLLY_INSTALL_DIR}/folly/lib/cmake/folly)
if (NOT TARGET Folly::folly)
  include("${FOLLY_CMAKE_DIR}/folly-targets.cmake")
endif()

set(FOLLY_LIBRARIES Folly::folly)

if (NOT folly_FIND_QUIETLY)
  message(STATUS "Found folly: ${PACKAGE_PREFIX_DIR}")
endif()

add_executable(
  lock-free-vs-locks
  lock-free-vs-locks.cpp
)

set(TARGET_LIBS ${Boost_LIBRARIES} ${FOLLY_LIBRARIES})

add_executable(
  problem
  problem.cpp
)
target_link_libraries(
  problem
  ${TARGET_LIBS}
)

add_executable(
  lock-solution
  lock-solution.cpp
)
target_link_libraries(
  lock-solution
  ${TARGET_LIBS}
)

add_executable(
  refcount-solution
  refcount-solution.cpp
)
target_link_libraries(
  refcount-solution
  ${TARGET_LIBS}
)

add_executable(
  rcu-solution
  rcu-solution.cpp
)
target_link_libraries(
  rcu-solution
  ${TARGET_LIBS}
)

add_executable(
  hazptr-solution
  hazptr-solution.cpp
)
target_link_libraries(
  hazptr-solution
  ${TARGET_LIBS}
)

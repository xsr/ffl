cmake_policy(SET CMP0048 NEW)
project(test VERSION 0.0.1)
cmake_minimum_required(VERSION 3.0)

set(TON_USE_ROCKSDB OFF)
set(CMAKE_CROSSCOMPILING ON)
set(TDUTILS_MIME_TYPE OFF)

set(CRC32C_BUILD_TESTS OFF CACHE BOOL "Build CRC32C's unit tests")
set(CRC32C_BUILD_BENCHMARKS OFF CACHE BOOL "Build CRC32C's benchmarks")
set(CRC32C_USE_GLOG OFF CACHE BOOL "Build CRC32C's tests with Google Logging")
set(CRC32C_INSTALL OFF CACHE BOOL "Install CRC32C's header and library")
add_subdirectory(../ton/third-party/crc32c crc32c)
set(CRC32C_FOUND 1)

set(PRE_CONFIGURE_FILE "../ton/git.cc.in")
set(POST_CONFIGURE_FILE "${CMAKE_CURRENT_BINARY_DIR}/git.cc")
include(../ton/git_watcher.cmake)

add_library(git STATIC ${POST_CONFIGURE_FILE})
target_include_directories(git PUBLIC ../ton/)
add_dependencies(git check_git)

set(TDUTILS_MIME_TYPE OFF CACHE BOOL "Generate mime type conversion")
add_subdirectory(../ton/tdutils tdutils)
add_subdirectory(../ton/tddb tddb)
add_subdirectory(../ton/tdactor tdactor)
add_subdirectory(../ton/crypto crypto)
add_subdirectory(../ton/tl tl)
add_subdirectory(../ton/tdtl tdtl)

include_directories(../ton/)
include_directories(../ton/crypto/)
include_directories(../ton/crypto/func/)
include_directories(../ton/crypto/block/)
include_directories(../ton/tdutils/)
include_directories(../ton/tddb/)
include_directories(../ton/tdtl/)
include_directories(../ton/tdactor/)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/tdutils/)

set(FUNC_LIB_SOURCE
  ../ton/crypto/func/keywords.cpp
  ../ton/crypto/func/unify-types.cpp
  ../ton/crypto/func/parse-func.cpp
  ../ton/crypto/func/abscode.cpp
  ../ton/crypto/func/gen-abscode.cpp
  ../ton/crypto/func/analyzer.cpp
  ../ton/crypto/func/asmops.cpp
  ../ton/crypto/func/builtins.cpp
  ../ton/crypto/func/stack-transform.cpp
  ../ton/crypto/func/optimize.cpp
  ../ton/crypto/func/codegen.cpp
)

add_library(lib func.cpp fift.cpp ${FUNC_LIB_SOURCE})

add_executable(test test.cpp)
target_link_libraries(test PUBLIC lib fift-lib ton_crypto src_parser git ton_block tdutils ton_crypto)

configure_file(test.fc test.fc COPYONLY)
file(COPY ../ton/crypto/fift/lib DESTINATION .)
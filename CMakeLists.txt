cmake_minimum_required (VERSION 3.10)

################################################################################
# Project definition
################################################################################

project(lru_cache
        VERSION 1.1.0
        LANGUAGES CXX)

################################################################################
# Build options
################################################################################

option(USE_ADDR_SANITIZER   "Build project with address sanitizer" OFF)
option(USE_LEAK_SANITIZER   "Build project with leak sanitizer" OFF)
option(USE_BEHAVIOR_SANITIZER   "Build project with undefined behavior sanitizer" OFF)
option(USE_THREAD_SANITIZER "Build project with thread sanitizer" OFF)

#option(USE_COVERAGE         "Try to use coverage flag" OFF)
option(USE_FAST_MATH        "Tell the compiler to use fast math" OFF)
option(USE_LTO              "Use link-time optimization for release builds" ON)
option(USE_PEDANTIC         "Tell the compiler to be pedantic" ON)
#option(USE_PTHREAD          "Use pthread library" OFF)
option(USE_WERROR           "Tell the compiler to make the build fail when warnings are present" ON)

option(USE_BOOST_INTRUSIVE  "Use boost intrusive backend" ON)
option(BUILD_BOOST          "Build boost" ON)

option(BUILD_EXAMPLES       "Build examples" ON)
option(BUILD_TESTS          "Build perftests and unittests" ON)

################################################################################
# Init cmake modules path
################################################################################

set(COMMON_CMAKE_DIR cmake)
set(COMMON_MAIN_CMAKE ${COMMON_CMAKE_DIR}/main.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/${COMMON_CMAKE_DIR} ${CMAKE_MODULE_PATH})

include(${COMMON_MAIN_CMAKE} PUBLIC)

################################################################################
# Include source code
################################################################################

if (USE_BOOST_INTRUSIVE)
    set(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE "THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE")
    set(boost "boost")
endif()

add_subdirectory(externals)
add_subdirectory(src)


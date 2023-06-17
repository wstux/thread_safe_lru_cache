# The MIT License
#
# Copyright (c) 2022 wstux
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

cmake_minimum_required (VERSION 3.10)

################################################################################
# Build check
################################################################################

include(common)

if (NOT COMMON_CMAKE_DIR)
    message(FATAL_ERROR "[FATAL] COMMON_CMAKE_DIR variable is not setted")
endif()

AddPlatform("Linux")

_is_supported_platform("${CMAKE_SYSTEM_NAME}" _is_support)
if (_is_support)
    message(STATUS "[INFO ] Build for '${CMAKE_SYSTEM_NAME}' platform")
else()
    message(FATAL_ERROR "[FATAL] Unsupported '${CMAKE_SYSTEM_NAME}' platform")
endif()

################################################################################
# Definitions
################################################################################

message(STATUS "[INFO ] CMAKE_BINARY_DIR: '${CMAKE_BINARY_DIR}'")
# Common options
# To write if need.

# Build options
option(USE_ADDR_SANITIZER   "Try to use the address sanitizer" OFF)
option(USE_COVERAGE         "Try to use coverage flag" OFF)
option(USE_FAST_MATH        "Tell the compiler to use fast math" OFF)
option(USE_LTO              "Use link-time optimization for release builds" ON)
option(USE_PEDANTIC         "Tell the compiler to be pedantic" ON)
option(USE_PTHREAD          "Use pthread library" OFF)
option(USE_WERROR           "Tell the compiler to make the build fail when warnings are present" ON)

################################################################################
# Configuration options
################################################################################

include(CMakeDependentOption)

include(build_flags)

if(CMAKE_C_COMPILER)
    message(STATUS "[INFO ] C compiler flags: '${CMAKE_C_FLAGS}'")
    if(CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "[INFO ] C debug compiler flags: '${CMAKE_C_FLAGS_DEBUG}'")
    elseif(CMAKE_BUILD_TYPE STREQUAL "release" OR CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "[INFO ] C release compiler flags: '${CMAKE_C_FLAGS_RELEASE}'")
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        message(STATUS "[INFO ] C rel_with_deb_info compiler flags: '${CMAKE_C_FLAGS_RELWITHDEBINFO}'")
    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        message(STATUS "[INFO ] C min_size_rel compiler flags: '${CMAKE_C_FLAGS_MINSIZEREL}'")
    endif()
    message(STATUS "[INFO ] C link executable: ${CMAKE_C_LINK_EXECUTABLE}")
endif()
if(CMAKE_CXX_COMPILER)
    message(STATUS "[INFO ] C++ compiler flags: '${CMAKE_CXX_FLAGS}'")
    if(CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "[INFO ] C++ debug compiler flags: '${CMAKE_CXX_FLAGS_DEBUG}'")
    elseif(CMAKE_BUILD_TYPE STREQUAL "release" OR CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "[INFO ] C++ release compiler flags: '${CMAKE_CXX_FLAGS_RELEASE}'")
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        message(STATUS "[INFO ] C++ rel_with_deb_info compiler flags: '${CMAKE_CXX_FLAGS_RELWITHDEBINFO}'")
    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        message(STATUS "[INFO ] C++ min_size_rel compiler flags: '${CMAKE_CXX_FLAGS_MINSIZEREL}'")
    endif()
    message(STATUS "[INFO ] C++ link executable: ${CMAKE_CXX_LINK_EXECUTABLE}")
endif()

message(STATUS "[INFO ] Exe linker flags: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "[INFO ] Module linker flags: ${CMAKE_MODULE_LINKER_FLAGS}")
message(STATUS "[INFO ] Shared linker flags: ${CMAKE_SHARED_LINKER_FLAGS}")

################################################################################
# Include targets functions
################################################################################

include(build_targets)
include(driver_targets)
include(external_targets)
include(wrapper_targets)


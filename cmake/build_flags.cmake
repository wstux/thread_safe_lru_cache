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

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

################################################################################
# Macro for set build flags
################################################################################

# Set C compiler flag.
macro(set_c_flag FLAG)
    if(CMAKE_C_COMPILER)
        if(${ARGC} GREATER 1)
            set(CMAKE_C_FLAGS_${ARGV1} "${CMAKE_C_FLAGS_${ARGV1}} ${FLAG}")
            message(TRACE "[TRACE] Macro 'set_c_flag': ARGV1='${ARGV1}', FLAG='${FLAG}'")
        else()
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
            message(TRACE "[TRACE] Macro 'set_c_flag': FLAG='${FLAG}'")
        endif()
    endif()
endmacro()

# Set C++ compiler flag.
macro(set_cxx_flag FLAG)
    if(CMAKE_CXX_COMPILER)
        if(${ARGC} GREATER 1)
            set(CMAKE_CXX_FLAGS_${ARGV1} "${CMAKE_CXX_FLAGS_${ARGV1}} ${FLAG}")
            message(TRACE "[TRACE] Macro 'set_cxx_flag': ARGV1='${ARGV1}', FLAG='${FLAG}'")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
            message(TRACE "[TRACE] Macro 'set_cxx_flag': FLAG='${FLAG}'")
        endif()
    endif()
endmacro()

# Set C and C++ compiler flag.
macro(set_flag FLAG)
    set_c_flag(${FLAG} ${ARGN})
    set_cxx_flag(${FLAG} ${ARGN})
endmacro()

# Set C and C++ compiler flag if isset option.
macro(set_flag_by_opt OPT FLAG)
    message(TRACE "[TRACE] Macro 'set_flag_by_opt': OPT='${OPT}', OPT VALUE='${${OPT}}'")
    if("${${OPT}}" STREQUAL "ON")
        set_flag(${FLAG})
    endif()
endmacro()

# Try set C++ compiler flag if flag supported.
macro(try_set_cxx_flag PROP FLAG)
    message(TRACE "[TRACE] Macro 'try_set_cxx_flag': PROP='${PROP}', FLAG='${FLAG}', ARGV2='${ARGV2}'")
    check_cxx_compiler_flag(${FLAG} FLAG_${PROP})
    if(FLAG_${PROP})
        set_cxx_flag(${FLAG} ${ARGV2})
    endif()
endmacro()

# Try set C and C++ compiler flag if flag supported.
macro(try_set_flag PROP FLAG)
    message(TRACE "[TRACE] Macro 'try_set_flag': PROP='${PROP}', FLAG='${FLAG}', ARGV2='${ARGV2}'")
    check_cxx_compiler_flag(${FLAG} FLAG_${PROP})
    if(FLAG_${PROP})
        set_c_flag(${FLAG} ${ARGV2})
        set_cxx_flag(${FLAG} ${ARGV2})
    endif()
endmacro()

# Try set C and C++ compiler flag if isset option.
macro(try_set_flag_by_opt OPT FLAG)
    message(TRACE "[TRACE] Macro 'try_set_flag_by_opt': OPT='${OPT}', OPT VALUE='${${OPT}}', FLAG='${FLAG}'")
    if("${${OPT}}" STREQUAL "ON")
        try_set_flag(${OPT} ${FLAG} ${ARGV2})
    endif()
endmacro()

################################################################################
# Macro for set linker flags
################################################################################

# Set linker flag.
macro(set_linker_flag FLAG)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FLAG}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${FLAG}")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${FLAG}")
    message(TRACE "[TRACE] Macro 'set_linker_flag': FLAG='${FLAG}'")
endmacro()

# Set linker flag if isset option.
macro(set_linker_flag_by_opt OPT FLAG)
    message(TRACE "[TRACE] Macro 'set_linker_flag_by_opt': OPT='${OPT}', OPT VALUE='${${OPT}}'")
    if("${${OPT}}" STREQUAL "ON")
        set_linker_flag(${FLAG})
    endif()
endmacro()

# Try set linker flag.
macro(try_set_linker_flag PROP FLAG)
    # Check it with the C compiler
    set(CMAKE_REQUIRED_FLAGS ${FLAG})
    check_C_compiler_flag(${FLAG} FLAG_${PROP})
    set(CMAKE_REQUIRED_FLAGS "")
    if(FLAG_${PROP})
        set_linker_flag(${FLAG})
    endif()
endmacro()

# Try set linker flag if isset option.
macro(try_set_linker_flag_by_opt OPT FLAG)
    message(TRACE "[TRACE] Macro 'try_set_linker_flag_by_opt': OPT='${OPT}', OPT VALUE='${${OPT}}', FLAG='${FLAG}'")
    if("${${OPT}}" STREQUAL "ON")
        try_set_linker_flag(${OPT} ${FLAG})
    endif()
endmacro()

################################################################################
# Set compiler flags
################################################################################

# Check support C++ standard
try_set_cxx_flag(CXX_17 "-std=gnu++17")
if(FLAG_CXX_17)
    message(STATUS "[INFO ] Use C++17 standard")
else()
    try_set_cxx_flag(CXX_14 "-std=gnu++14")
    if(FLAG_CXX_14)
        message(STATUS "[INFO ] Use C++14 standard")
    else()
        try_set_cxx_flag(CXX_11 "-std=gnu++11")
        if(FLAG_CXX_11)
            message(STATUS "[INFO ] Use C++11 standard")
        else()
            try_set_cxx_flag(CXX_0X "-std=gnu++0x")
            if(FLAG_CXX_0X)
                message(STATUS "[INFO ] Use C++0x standard")
            else()
                message(FATAL_ERROR "[FATAL] C++ not supported by compiler")
            endif()
        endif()
    endif()
endif()

# Set common flags
set_flag("-Os -DNDEBUG"     MINSIZEREL)
set_flag("-O3 -DNDEBUG"     RELEASE)
set_flag("-O2 -DNDEBUG -g3" RELWITHDEBINFO)
set_flag("-O0 -g3"          DEBUG)
set_flag("-Wall -Wextra")

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set_flag("-rdynamic")
    set_flag("-fPIC")
    set_flag("-ggdb3")
    set_flag("-ffunction-sections")
    set_flag("-fstrict-aliasing")
endif()

#try_set_flag(FPIE "-fPIE")
#try_set_linker_flag(LINKER_PIE "-pie")

# Use hidden symbol visibility if possible.
# void __attribute__((visibility("default"))) Exported() {...}
#try_set_flag(FVISIBILITY_HIDDEN "-fvisibility=hidden")

# Protect stack
# try_set_flag(FSTACK_PROTECTOR "-fstack-protector-strong")
# if (NOT FLAG_FSTACK_PROTECTOR)
#     try_set_flag(FSTACK_PROTECTOR "-fstack-protector-all")
# endif()
# try_set_flag(WSTACK_PROTECTOR "-Wstack-protector")

# try_set_flag(FNO_STRICT_OVERFLOW "-fno-strict-overflow")

# Set flags if isset options
set_flag_by_opt(USE_ADDR_SANITIZER  "-fsanitize=address")
try_set_flag_by_opt(USE_COVERAGE    "--coverage")
set_flag_by_opt(USE_FAST_MAT        "--ffast-math")
set_flag_by_opt(USE_PEDANTIC        "-pedantic")
set_flag_by_opt(USE_WERROR          "-Werror")

set_flag_by_opt(USE_LTO             "-flto")
set_linker_flag_by_opt(USE_LTO      "-flto")

try_set_flag_by_opt(USE_PTHREAD             "-pthread")
if (LINUX)
    try_set_linker_flag_by_opt(USE_PTHREAD  "-pthread")
endif()

 if (USE_PEDANTIC)
    try_set_flag(WARNINGS "-pedantic-errors")
endif()


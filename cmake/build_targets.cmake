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

include(build_utils)

################################################################################
# Targets
################################################################################

macro(LibTarget TARGET_NAME)
    set(_flags_kw   MODULE SHARED STATIC INTERFACE)
    set(_values_kw  COMMENT INCLUDE_DIR LINKER_LANGUAGE)
    set(_lists_kw   HEADERS SOURCES LIBRARIES DEPENDS COMPILE_DEFINITIONS)
    _parse_target_args(${TARGET_NAME}
        _flags_kw _values_kw _lists_kw ${ARGN}
    )

    set(_include_dir "")
    if(${TARGET_NAME}_INCLUDE_DIR)
        set(_include_dir "${PROJECT_SOURCE_DIR}/${${TARGET_NAME}_INCLUDE_DIR}")
    else()
        _get_default_include_dirs(${TARGET_NAME} _include_dir)
    endif()

    set(_qualifier PUBLIC)
    if (${TARGET_NAME}_INTERFACE)
        add_library(${TARGET_NAME} INTERFACE)
        set(_qualifier INTERFACE)
    elseif (${TARGET_NAME}_SHARED OR ${TARGET_NAME}_STATIC)
        set(LIB_TYPE SHARED)
        if (${TARGET_NAME}_STATIC)
            set(LIB_TYPE STATIC)
        endif()

        add_library(${TARGET_NAME} ${LIB_TYPE}
            ${${TARGET_NAME}_HEADERS} ${${TARGET_NAME}_SOURCES}
        )

        target_include_directories(${TARGET_NAME} PRIVATE ${_include_dir})
        set_target_properties(${TARGET_NAME} PROPERTIES INCLUDE_DIRECTORIES ${_include_dir})
    else()
        message(ERROR "[ERROR] Unsupported library type")
    endif()

    set(_public_headers "")
    if (${TARGET_NAME}_HEADERS)
        foreach (_hdr IN LISTS ${TARGET_NAME}_HEADERS)
            set(_public_headers "${_public_headers}" "${CMAKE_CURRENT_SOURCE_DIR}/${_hdr}")
        endforeach()
    else()
        file(GLOB_RECURSE _public_headers
            "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
        )
    endif()

    target_include_directories(
        ${TARGET_NAME} ${_qualifier}
            "$<BUILD_INTERFACE:${_include_dir}>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    set_property(DIRECTORY PROPERTY ${TARGET_NAME}_INCLUDE_DIR  ${_include_dir})

    if ("${${TARGET_NAME}_LINKER_LANGUAGE}" STREQUAL "C" OR "${${TARGET_NAME}_LINKER_LANGUAGE}" STREQUAL "CXX")
        set_target_properties(${TARGET_NAME} PROPERTIES
            LINKER_LANGUAGE ${${TARGET_NAME}_LINKER_LANGUAGE}
        )
    endif()

    _configure_target(${TARGET_NAME})

    set_target_properties(${TARGET_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    )
    set_target_properties(${TARGET_NAME} PROPERTIES
        OUTPUT_NAME     ${TARGET_NAME}
        PUBLIC_HEADER   "${_public_headers}"
    )
    install(TARGETS ${TARGET_NAME}
        EXPORT ${TARGET_NAME}-targets
        PUBLIC_HEADER DESTINATION   "include/${TARGET_NAME}"
        LIBRARY DESTINATION         "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION         "${CMAKE_INSTALL_LIBDIR}"
    )
endmacro()

macro(ExecTarget TARGET_NAME)
    set(_flags_kw   )
    set(_values_kw  COMMENT INCLUDE_DIR LINKER_LANGUAGE)
    set(_lists_kw   HEADERS SOURCES LIBRARIES DEPENDS COMPILE_DEFINITIONS)
    _parse_target_args(${TARGET_NAME}
        _flags_kw _values_kw _lists_kw ${ARGN}
    )

    add_executable(${TARGET_NAME}
        ${${TARGET_NAME}_HEADERS} ${${TARGET_NAME}_SOURCES}
    )

    if ("${${TARGET_NAME}_LINKER_LANGUAGE}" STREQUAL "C" OR "${${TARGET_NAME}_LINKER_LANGUAGE}" STREQUAL "CXX")
        set_target_properties(${TARGET_NAME} PROPERTIES
            LINKER_LANGUAGE ${${TARGET_NAME}_LINKER_LANGUAGE}
        )
    endif()

    _configure_target(${TARGET_NAME})

    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    )
endmacro()

macro(TestTarget TARGET_NAME)
    set(_flags_kw   DISABLE)
    set(_values_kw  COMMENT INCLUDE_DIR LINKER_LANGUAGE)
    set(_lists_kw   HEADERS SOURCES LIBRARIES DEPENDS COMPILE_DEFINITIONS)
    _parse_target_args(${TARGET_NAME}
        _flags_kw _values_kw _lists_kw ${ARGN}
    )

    set(_target_dir "${CMAKE_BINARY_DIR}/test")
    set(_enable_autorun true)
    if (${TARGET_NAME}_DISABLE)
        set(_enable_autorun false)
    endif()

    add_executable(${TARGET_NAME}
        ${${TARGET_NAME}_HEADERS} ${${TARGET_NAME}_SOURCES}
    )

    if ("${${TARGET_NAME}_LINKER_LANGUAGE}" STREQUAL "C" OR "${${TARGET_NAME}_LINKER_LANGUAGE}" STREQUAL "CXX")
        set_target_properties(${TARGET_NAME} PROPERTIES
            LINKER_LANGUAGE ${${TARGET_NAME}_LINKER_LANGUAGE}
        )
    endif()

    _configure_target(${TARGET_NAME})

    if (${_enable_autorun})
        add_test(NAME ${TARGET_NAME}
            COMMAND $<TARGET_FILE:${TARGET_NAME}>
        )
    endif()

    CustomTarget(${TARGET_NAME}_run
        COMMAND "${_target_dir}/${TARGET_NAME}"
        DEPENDS ${TARGET_NAME}
        VERBATIM
    )

    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${_target_dir}"
    )
endmacro()


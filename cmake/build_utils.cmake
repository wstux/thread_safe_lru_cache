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

################################################################################
# Functions
################################################################################

function(_get_qualifier TARGET_NAME RESULT)
    set(${RESULT} PRIVATE PARENT_SCOPE)
    get_target_property(_type ${TARGET_NAME} TYPE)
    if(_type STREQUAL "INTERFACE_LIBRARY")
        set(${RESULT} INTERFACE PARENT_SCOPE)
    endif()
endfunction()

function(_configure_target TARGET_NAME)
    if (NOT ${TARGET_NAME}_INCLUDE_DIR)
        _get_qualifier(${TARGET_NAME}   _qualifier)
        target_include_directories(${TARGET_NAME}
            ${_qualifier} ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()

    if (${TARGET_NAME}_COMPILE_DEFINITIONS)
        foreach (_def IN LISTS ${TARGET_NAME}_COMPILE_DEFINITIONS)
            _get_qualifier(${TARGET_NAME}   _qualifier)
            target_compile_definitions(${TARGET_NAME} ${_qualifier} ${_def})
        endforeach()
    endif()

    get_property(_depends DIRECTORY PROPERTY ${TARGET_NAME}_DEPENDS)
    foreach (_dep IN LISTS _depends)
        if (TARGET "${_dep}")
            set(_target_dep ${_dep})
        else()
            add_custom_target(${_target_dep} DEPENDS ${_dep})
        endif()
        add_dependencies(${TARGET_NAME} ${_dep})
    endforeach()

    get_property(_libraries DIRECTORY PROPERTY ${TARGET_NAME}_LIBRARIES)
    if (_libraries)
        target_link_libraries(${TARGET_NAME} ${_libraries})
    endif()

    foreach(_lib IN LISTS _libraries _depends)
        if (NOT TARGET ${_lib})
            continue()
        endif()

        get_target_property(_target_type ${_lib} TYPE)
        if(_target_type STREQUAL "INTERFACE_LIBRARY")
            get_property(_include_dirs DIRECTORY PROPERTY ${_lib}_INCLUDE_DIR)
            if (_include_dirs)
                target_include_directories(${TARGET_NAME} PRIVATE ${_include_dirs})
            endif()
            continue()
        endif()

        if (_target_type STREQUAL "STATIC_LIBRARY" OR _target_type STREQUAL "MODULE_LIBRARY" OR _target_type STREQUAL "SHARED_LIBRARY")
            target_link_libraries(${TARGET_NAME} ${_lib})
        endif ()

        get_target_property(_dep_libraries ${_lib} LIBRARIES)
        if (_dep_libraries)
            target_link_libraries(${TARGET_NAME} ${_dep_libraries})
        endif()

        get_target_property(_include_dirs ${_lib} INCLUDE_DIRECTORIES)
        if (_include_dirs)
            target_include_directories(${TARGET_NAME} PRIVATE ${_include_dirs})
        else()
            get_property(_include_dirs DIRECTORY PROPERTY ${_lib}_INCLUDE_DIR)
            if (_include_dirs)
                target_include_directories(${TARGET_NAME} PRIVATE ${_include_dirs})
            endif()
        endif()
    endforeach()
endfunction()

function(_is_kw CHECK_STR KW_LIST RESULT)
    set(${RESULT} 0 PARENT_SCOPE)
    list(FIND ${KW_LIST} "${CHECK_STR}" IS_FIND)
    if (NOT IS_FIND EQUAL -1)
        set(${RESULT} 1 PARENT_SCOPE)
    endif()
endfunction()

function(_parse_target_args TARGET_NAME KW_FLAGS KW_VALUES KW_LISTS)
    # https://cmake.org/cmake/help/latest/command/cmake_parse_arguments.html
    # https://stackoverflow.com/questions/23327687/how-to-write-a-cmake-function-with-more-than-one-parameter-groups
    cmake_parse_arguments(PARSE_ARGV 1 ${TARGET_NAME}
        "${${KW_FLAGS}}" "${${KW_VALUES}}" "${${KW_LISTS}}"
    )

    set(kw_list "${${KW_FLAGS}}" "${${KW_VALUES}}" "${${KW_LISTS}}")
    foreach(arg IN LISTS ARGN)
        # Check is 'arg' a keyword.
        _is_kw(${arg} kw_list is_keyword)

        # If 'arg' is keyword - save 'arg' to 'key' variable and save key-flag to parent scope.
        if (is_keyword)
            set(key "${arg}")
            set_property(DIRECTORY APPEND PROPERTY ${TARGET_NAME}_${key} "${${TARGET_NAME}_${key}}")
            set(${TARGET_NAME}_${key} "${${TARGET_NAME}_${key}}" PARENT_SCOPE)
        endif()
    endforeach()
endfunction()


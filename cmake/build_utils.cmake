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

function(_configure_target TARGET_NAME)
    if (NOT ${TARGET_NAME}_INCLUDE_DIR)
        get_target_property(_cur_target_type ${TARGET_NAME} TYPE)
        if(NOT _cur_target_type STREQUAL "INTERFACE_LIBRARY")
            target_include_directories(${TARGET_NAME}
                PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
            )
        endif()
    endif()

    if (${TARGET_NAME}_COMPILE_DEFINITIONS)
        foreach (_def IN LISTS ${TARGET_NAME}_COMPILE_DEFINITIONS)
            target_compile_definitions(${TARGET_NAME} PRIVATE ${_def})
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
        get_target_property(_target_type ${_lib} TYPE)
        if(_target_type STREQUAL "INTERFACE_LIBRARY")
            get_property(_include_dirs DIRECTORY PROPERTY ${_lib}_INCLUDE_DIR)
            if (_include_dirs)
                target_include_directories(${TARGET_NAME} PRIVATE ${_include_dirs})
            endif()
            continue()
        endif()

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

function(_parse_target_args TARGET_NAME KW_LIST)
    set(key "")
    set(to_parent_scope FALSE)
    foreach(arg IN LISTS ARGN)
        # Check is 'arg' a keyword.
        _is_kw(${arg} "${KW_LIST}" is_keyword)

        # If 'arg' is keyword - save 'arg' to 'key' variable and save key-flag to parent scope.
        if (is_keyword)
            if (to_parent_scope)
                if (NOT DEFINED ${TARGET_NAME}_${key})
                    set_property(DIRECTORY PROPERTY ${TARGET_NAME}_${key} "")
                    set(${TARGET_NAME}_${key} TRUE)
                endif()
                set_property(DIRECTORY APPEND PROPERTY ${TARGET_NAME}_${key} "${${TARGET_NAME}_${key}}")
                set(${TARGET_NAME}_${key} "${${TARGET_NAME}_${key}}" PARENT_SCOPE)
                set(to_parent_scope FALSE)
            endif()

            set(key "${arg}")
            set(to_parent_scope TRUE)
            continue()
        endif()

        # If 'key' variable is defined - add data to key args and add to parent scope.
        if (key)
            if (NOT DEFINED ${TARGET_NAME}_${key})
                set_property(DIRECTORY PROPERTY ${TARGET_NAME}_${key} "${arg}")
                set(${TARGET_NAME}_${key} "${arg}")
            else()
                set_property(DIRECTORY APPEND PROPERTY ${TARGET_NAME}_${key} "${arg}")
                set(${TARGET_NAME}_${key} "${${TARGET_NAME}_${key}};${arg}")
            endif()
        endif()
    endforeach()

    if (to_parent_scope)
        if (NOT DEFINED ${TARGET_NAME}_${key})
            set_property(DIRECTORY PROPERTY ${TARGET_NAME}_${key} "")
            set(${TARGET_NAME}_${key} TRUE)
        endif()
        get_property(_${TARGET_NAME}_${key}_prop DIRECTORY PROPERTY ${TARGET_NAME}_${key})
        if (NOT _${TARGET_NAME}_${key}_prop)
            set_property(DIRECTORY APPEND PROPERTY ${TARGET_NAME}_${key} "${${TARGET_NAME}_${key}}")
        endif()
        set(${TARGET_NAME}_${key} "${${TARGET_NAME}_${key}}" PARENT_SCOPE)
        set(to_parent_scope FALSE)
    endif()
endfunction()

macro(_validate_args TARGET_NAME KW_LIST FLAG_KW_LIST VALUES_KW_LIST)
    foreach(key IN LISTS ${KW_LIST})
        if (NOT DEFINED ${TARGET_NAME}_${key})
            continue()
        endif()

        _is_kw(${key} "${FLAG_KW_LIST}" is_flag)
        if (is_flag)
            if ("${${TARGET_NAME}_${key}}" MATCHES "TRUE")
                continue()
            elseif ("${${TARGET_NAME}_${key}}" MATCHES "FALSE")
                continue()
            else()
                message(FATAL_ERROR " Invalid flag value for key '${key}'")
            endif()

            continue()
        endif()

        _is_kw(${key} "${VALUES_KW_LIST}" is_value)
        if (is_value)
            if ("${${TARGET_NAME}_${key}}" MATCHES "TRUE")
                message(FATAL_ERROR " Invalid value for key '${key}'")
            elseif ("${${TARGET_NAME}_${key}}" MATCHES "FALSE")
                message(FATAL_ERROR " Invalid value for key '${key}'")
            endif()

            continue()
        endif()

        message(ERROR " Unsupported key '${key}'")
    endforeach()
endmacro()


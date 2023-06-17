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
# Keywords
################################################################################

set(_COMMON_TARGET_KW   HEADERS     # headers list
                        SOURCES     # sources list
                        COMMENT     # message before build target
                        LIBRARIES
                        DEPENDS
                        COMPILE_DEFINITIONS
)

set(_CUSTOM_TARGET_KW   COMMAND
                        DEPENDS
)

set(_EXEC_TARGET_KW     ${_COMMON_TARGET_KW}
)

set(_LIB_TARGET_KW      ${_COMMON_TARGET_KW}
                        MODULE      # Dinamic module library type
                        SHARED      # Dinamic library type
                        STATIC      # Static library type
                        INTERFACE
                        INCLUDE_DIR
)

set(_LIST_VALUES_KW     HEADERS
                        SOURCES
                        LIBRARIES
                        COMMAND
                        DEPENDS
                        INCLUDE_DIR
)

set(_FLAG_KW            MODULE
                        SHARED
                        STATIC
                        INTERFACE
)

################################################################################
# Targets
################################################################################

macro(CustomTarget TARGET_NAME)
#    _parse_target_args(${TARGET_NAME} _CUSTOM_TARGET_KW ${ARGN})

#    foreach(key IN LISTS _CUSTOM_TARGET_KW)
#        foreach(dep IN LISTS ${TARGET_NAME}_${key})
#            list(APPEND ${TARGET_NAME}_BUILD_ARGS ${key} ${dep})
#        endforeach()
#    endforeach()

    add_custom_target(${TARGET_NAME} ${ARGN})
endmacro()

macro(LibTarget TARGET_NAME)
    _parse_target_args(${TARGET_NAME} _LIB_TARGET_KW ${ARGN})
    _validate_args(${TARGET_NAME} _LIB_TARGET_KW _FLAG_KW _LIST_VALUES_KW)

    if(${TARGET_NAME}_INTERFACE)
#        message(INFO " Configure INTERFACE LIB target '${TARGET_NAME}'")
        add_library(${TARGET_NAME} INTERFACE)
        target_include_directories(
            ${TARGET_NAME} INTERFACE
                "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/${${TARGET_NAME}_INCLUDE_DIR}>"
                "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )
    elseif(${TARGET_NAME}_SHARED OR ${TARGET_NAME}_STATIC)
        set(LIB_TYPE SHARED)
        if (${TARGET_NAME}_STATIC)
            set(LIB_TYPE STATIC)
        endif()

#        message(INFO " Configure ${LIB_TYPE} LIB target '${TARGET_NAME}'")
        add_library(${TARGET_NAME} ${LIB_TYPE} ${${TARGET_NAME}_HEADERS}
                                               ${${TARGET_NAME}_SOURCES}
        )

        target_include_directories(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_INCLUDE_DIR})

        set_target_properties(${TARGET_NAME}
                              PROPERTIES
                                  INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/${${TARGET_NAME}_INCLUDE_DIR}
        )
    else()
        message(ERROR "[ERROR] Unsupported library type")
    endif()

    set_property(DIRECTORY PROPERTY ${TARGET_NAME}_INCLUDE_DIR
                 ${PROJECT_SOURCE_DIR}/${${TARGET_NAME}_INCLUDE_DIR}
    )

    _configure_target(${TARGET_NAME})

    install(TARGETS ${TARGET_NAME} LIBRARY DESTINATION libs)
endmacro()

macro(ExecTarget TARGET_NAME)
    _parse_target_args(${TARGET_NAME} _EXEC_TARGET_KW ${ARGN})
    _validate_args(${TARGET_NAME} _EXEC_TARGET_KW _FLAG_KW _LIST_VALUES_KW)

#    message(INFO " Configure EXEC target '${TARGET_NAME}'")
    add_executable(${TARGET_NAME} ${${TARGET_NAME}_HEADERS}
                                  ${${TARGET_NAME}_SOURCES}
    )

    _configure_target(${TARGET_NAME})

    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)
endmacro()

macro(TestTarget TARGET_NAME)
    _parse_target_args(${TARGET_NAME} _EXEC_TARGET_KW ${ARGN})
    _validate_args(${TARGET_NAME} _EXEC_TARGET_KW _FLAG_KW _LIST_VALUES_KW)

#    message(INFO " Configure TEST target '${TARGET_NAME}'")
    add_executable(${TARGET_NAME} ${${TARGET_NAME}_HEADERS}
                                  ${${TARGET_NAME}_SOURCES}
    )
    _configure_target(${TARGET_NAME})

    add_test(
        NAME ${TARGET_NAME}
        COMMAND $<TARGET_FILE:${TARGET_NAME}>
    )

    CustomTarget(${TARGET_NAME}_run
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
        DEPENDS ${TARGET_NAME}
        VERBATIM
    )

    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION tests)
endmacro()


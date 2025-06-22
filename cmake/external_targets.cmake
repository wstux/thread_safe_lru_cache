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

include(ExternalProject)

include(build_utils)

################################################################################
# Setting of cmake policies
################################################################################

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	cmake_policy(SET CMP0135 NEW)
endif()

################################################################################
# Constants
################################################################################

set(EXTERNALS_PREFIX "${CMAKE_BINARY_DIR}/externals")

################################################################################
# Utility functiona
################################################################################

function(_set_command VAR VALUE DFL_VALUE)
    set(${VAR} "${DFL_VALUE}" PARENT_SCOPE)
    if (${VALUE})
        set(${VAR} bash -c "${${VALUE}}" PARENT_SCOPE)
    endif()
endfunction()

################################################################################
# Targets
################################################################################

function(ExternalTarget EXT_TARGET_NAME)
    set(_flags_kw   )
    set(_values_kw  BUILD_COMMAND
                    CONFIGURE_COMMAND
                    INCLUDE_DIR
                    INSTALL_COMMAND
                    INSTALL_DIR
                    URL #URL_MD5
    )
    set(_lists_kw   DEPENDS
                    LIBRARIES
                    PATCHES
    )
    _parse_target_args_strings(${EXT_TARGET_NAME}
        _flags_kw _values_kw _lists_kw ${ARGN}
    )

    set(_target_name    "${EXT_TARGET_NAME}")
    set(_target_dir     "${EXTERNALS_PREFIX}/${_target_name}")

    set(_ext_url        ${${EXT_TARGET_NAME}_URL})
    set(_ext_url_hash   ${${EXT_TARGET_NAME}_URL_MD5})

    _set_command(_configure_cmd ${EXT_TARGET_NAME}_CONFIGURE_COMMAND "true")
    _set_command(_build_cmd     ${EXT_TARGET_NAME}_BUILD_COMMAND     "true")

    set(_include_dir "${${EXT_TARGET_NAME}_INCLUDE_DIR}")
    set(_install_dir "${${EXT_TARGET_NAME}_INSTALL_DIR}")
    _set_command(_install_command ${EXT_TARGET_NAME}_INSTALL_COMMAND "true")

    set(_depends "${${EXT_TARGET_NAME}_DEPENDS}")

    set(_patch_cmd "true")
    if (${EXT_TARGET_NAME}_PATCHES)
        foreach (_patch IN LISTS ${EXT_TARGET_NAME}_PATCHES)
            set(_patch_command  "bash -c \"patch -p1 --dry-run < ${_patch}\"\n")
            file(APPEND ${_target_dir}/${_target_name}-prefix/patch_target.sh "${_patch_command}")
        endforeach()
        set(_patch_cmd bash ${_target_dir}/${_target_name}-prefix/patch_target.sh)
    endif()

    ExternalProject_Add(${_target_name}
        PREFIX              ${_target_dir}
        STAMP_DIR           ${_target_dir}/stamp
        TMP_DIR             ${_target_dir}/tmp
        URL                 ${_ext_url}
        URL_MD5             ${_ext_url_hash}
        CONFIGURE_COMMAND   ${_configure_cmd}
        BUILD_IN_SOURCE     1
        INSTALL_COMMAND     ${_install_command}
        INSTALL_DIR         ${_install_dir}
        BUILD_COMMAND       ${_build_cmd}
        PATCH_COMMAND       ${_patch_cmd}
        DEPENDS ${_depends}
    )

    set(_libraries "")
    if (${EXT_TARGET_NAME}_LIBRARIES)
        foreach (_lib IN LISTS ${EXT_TARGET_NAME}_LIBRARIES)
            set(_lib_path   "${_install_dir}/lib/${_lib}")
            if (_lib MATCHES ".*\.so.*")
                set(_cp_command  "bash -c \"cp -a ${_lib_path} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/\"\n")
                set(_cpl_command "bash -c \"test -h ${_lib_path} && cp -a `readlink -f ${_lib_path}` ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/ || true\"\n")
            elseif (_lib MATCHES ".*\.a.*")
                set(_cp_command  "bash -c \"cp -a ${_lib_path} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/\"\n")
            endif()
            file(APPEND ${_target_dir}/${_target_name}-prefix/copy_libraries.sh "${_cp_command}")
            file(APPEND ${_target_dir}/${_target_name}-prefix/copy_libraries.sh "${_cpl_command}")

            if (_libraries)
                set(_libraries "${_libraries}" "${_lib_path}")
            else()
                set(_libraries "${_lib_path}")
            endif()
        endforeach()

        ExternalProject_Add_Step(${EXT_TARGET_NAME} copy-libraries
            COMMAND bash ${_target_dir}/${_target_name}-prefix/copy_libraries.sh
            WORKING_DIRECTORY <SOURCE_DIR>
            DEPENDEES install
            LOG 1
        )
    endif()

    foreach (_dep IN LISTS _depends)
        if (TARGET ${_dep})
            get_target_property(_dep_include_dirs ${_dep} INCLUDE_DIRECTORIES)
            if (_dep_include_dirs)
                include_directories(${_dep_include_dirs})
            endif()

            #get_target_property(_dep_libraries ${_dep} LIBRARIES)
            #if (_dep_libraries)
            #    target_link_libraries(${EXT_TARGET_NAME} ${_dep_libraries})
            #endif()
        endif()
    endforeach()

    set_target_properties(${EXT_TARGET_NAME} PROPERTIES
        INCLUDE_DIRECTORIES "${_include_dir}"
        IMPORTED_LOCATION   "${_libraries}"
        LIBRARIES           "${_libraries}"
        INSTALL_DIR         "${install_dir}"
    )
endfunction()


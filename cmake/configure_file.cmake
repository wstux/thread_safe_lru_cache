# The MIT License
#
# Copyright (c) 2023 wstux
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

set(_CONFIGURE_FILE_KW  DEFINES
                        SRC_FILE
                        DEST_FILE
)

################################################################################
# Targets
################################################################################

function(ConfigureFile TARGET_NAME)
    _parse_target_args(${TARGET_NAME} _CONFIGURE_FILE_KW ${ARGN})

    foreach (def IN LISTS ${TARGET_NAME}_DEFINES)
        string(REPLACE "=" ";" def_list ${def})
        list(LENGTH def_list def_list_len)
        if (NOT ${def_list_len} MATCHES "2")
            message(FATAL_ERROR "[FATAL] Invalid defile value '${def}'")
        endif()

        list (GET def_list 0 variable)
        list (GET def_list 1 value)
        set(${variable} ${value})
    endforeach()

    set(dest_file "${CMAKE_CURRENT_BINARY_DIR}/${${TARGET_NAME}_DEST_FILE}")
    configure_file(${${TARGET_NAME}_SRC_FILE} ${dest_file})
endfunction()


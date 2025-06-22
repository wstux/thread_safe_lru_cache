# CMake C++ Project Template
*cmake_cpp_project_template* is a simple template for C and C++ projects.
The template includes targets for building libraries, tests, and executables.

Currently supported build on GNU/Linux.

The *cmake_cpp_project_template* assumes you want to setup a project using
* CMake (3.0 or above)
* C/C++ compiler


## Contents

* [Description](#description)
* [Usage](#usage)
* [Targets](#targets)
  * [Libraries](#libraries)
  * [Executables](#executables)
  * [Tests](#tests)
  * [Drivers](#drivers)
  * [Externals](#externals)
  * [Custom targets](#custom-targets)
* [Build](#build)
* [License](#license)

## Description

The template sets up hooks for Git. In particular, a check for the presence of
last whitespace characters in lines is added. This can lead to errors due to
empty lines at the end of the file. To fix this error, you need to run the
command:
```
git config --global core.whitespace -blank-at-eof
```

## Usage

### Build project

The project is built with the `make` command. Assembly takes place in three stages:
1. the `make` command starts configuring the project with `cmake`;
2. the project is configured by the `cmake` system;
3. start building the project/target with `make`.

The build looks like this:
```
user -> make -> cmake -> make
```

## Targets

The library supports targets:
* [LibTarget](#libraries) - building libraries;
* [ExecTarget](#executables) - building executable files;
* [TestTarget](#tests) - building tests;
* [DriverTarget](#drivers) - building kernel modules;
* [ExternalTarget and WrapperTarget](#externals) - building external modules;
* [CustomTarget](#custom-targets) - custom targets.

### Libraries

`LibTarget` declares the library as the target of the build. `LibTarget`
supports keywords:
* library type `SHARED`, `STATIC` or `INTERFACE`;
* `HEADERS` - list of public header files. If this parameter is not specified,
  all header files in the directory containing the target's source code will be used;
* `SOURCES` - list of files with target source code;
* `INCLUDE_DIR` - directory relative to which the target's header files will
  be searched;
* `LINKER_LANGUAGE` - specifies language whose compiler will invoke the linker
  (supports only 'C' or 'CXX');
* `COMPILE_DEFINITIONS` - preprocessor definitions for compiling a target's sources;
* `LIBRARIES` - list of libraries the target depends on;
* `DEPENDS` - list of the target's dependencies.

The `LIBRARIES` field specifies the list of libraries (targets) included in the
project, the `DEPENDS` field specifies the system/third-party libraries (targets)
required to build the project/target.

Library build target template:
```
LibTarget(<lib_name> <lib_type>
    HEADERS     <list_of_headers>
    SOURCES     <list_of_source_files>
    INCLUDE_DIR     <directory>
    LINKER_LANGUAGE <lang>
    COMPILE_DEFINITIONS
        <preprocessor_definitions>
    LIBRARIES   <list_of_libraries_target_depends_on>
    DEPENDS     <list_of_target_dependencies>
)
```

Usage example:
```
LibTarget(shared_lib SHARED
    HEADERS
        shared_lib.h
        details/shared_lib_impl.h
    SOURCES
        details/shared_lib.cpp
        details/shared_lib_impl.cpp
    INCLUDE_DIR     libs
    LINKER_LANGUAGE CXX
    COMPILE_DEFINITIONS
        BOOST_LOG_DYN_LINK
    LIBRARIES
        interface_lib
        shared_lib_2
        static_lib
    DEPENDS
        boost
        ext1
        ext2
)
```

### Executables

`ExecTarget` declares the build target to be an executable file. `ExecTarget`
supports keywords:
* `HEADERS` - list of header files included in the target (this parameter
  is optional);
* `SOURCES` - list of files with target source code;
* `INCLUDE_DIR` - directory relative to which the target's header files will
  be searched;
* `LINKER_LANGUAGE` - specifies language whose compiler will invoke the linker
  (supports only 'C' or 'CXX');
* `COMPILE_DEFINITIONS` - preprocessor definitions for compiling a target's sources;
* `LIBRARIES` - list of libraries the target depends on;
* `DEPENDS` - list of the target's dependencies.

The `LIBRARIES` field specifies the list of libraries (targets) included in the
project, the `DEPENDS` field specifies the system/third-party libraries (targets)
required to build the project/target.

Executable file build target template:
```
ExecTarget(<exec_name>
    HEADERS     <list_of_headers>
    SOURCES     <list_of_source_files>
    INCLUDE_DIR     <directory>
    LINKER_LANGUAGE <lang>
    COMPILE_DEFINITIONS
        <preprocessor_definitions>
    LIBRARIES   <list_of_libraries_target_depends_on>
    DEPENDS     <list_of_target_dependencies>
)
```

Usage example:
```
ExecTarget(executable
    HEADERS
        executable.h
    SOURCES
        main.cpp
    LIBRARIES
        interface_lib
        shared_lib
        static_lib
    DEPENDS
        boost
        ext1
        ext2
)
```

### Tests

`TestTarget` declares the build target to be an executable file. `cmake` supports
a `test` metatarget that runs all declared tests, `TestTarget` is essentially an
executable and is needed to use the `cmake` build system test framework. `TestTarget`
supports keywords:
* `HEADERS` - list of header files included in the target (this parameter
  is optional);
* `SOURCES` - list of files with target source code;
* `INCLUDE_DIR` - directory relative to which the target's header files will
  be searched;
* `LINKER_LANGUAGE` - specifies language whose compiler will invoke the linker
  (supports only 'C' or 'CXX');
* `COMPILE_DEFINITIONS` - preprocessor definitions for compiling a target's sources;
* `LIBRARIES` - list of libraries the target depends on;
* `DEPENDS` - list of the target's dependencies;
* `DISABLE` - disable autorun test with `make test` command.

The `LIBRARIES` field specifies the list of libraries (targets) included in the
project, the `DEPENDS` field specifies the system/third-party libraries (targets)
required to build the project/target.

Test build target template:
```
TestTarget(<test_name>
    HEADERS     <list_of_headers>
    SOURCES     <list_of_source_files>
    INCLUDE_DIR     <directory>
    LINKER_LANGUAGE <lang>
    COMPILE_DEFINITIONS
        <preprocessor_definitions>
    LIBRARIES   <list_of_libraries_target_depends_on>
    DEPENDS     <list_of_target_dependencies>
    DISABLE
)
```

Usage example:
```
TestTarget(ut_test
    HEADERS
        test.h
    SOURCES
        ut_test.cpp
    LIBRARIES
        interface_lib
        shared_lib
        static_lib
    DEPENDS
        boost
        ext1
        ext2
    DISABLE
)
```

### Drivers

`DriverTarget` declares the kernel driver as the build target. `DriverTarget`
supports keywords:
* `INCLUDE_DIR` - directory relative to which the target's header files will
  be searched;
* `SOURCES` - list of files with target source code;
* `EXTRA_CFLAGS` - list of additional target compilation flags;
* `EXTRA_LDFLAGS` - list of additional target linking flags;
* `COMPILE_DEFINITIONS` - preprocessor definitions for compiling a target's sources.

The assembly of driders is somewhat unique, so it takes place in several stages:
1. in the build directory, a subdirectory of driver build source codes
   `__kbuild_src` is created with a directory tree up to the driver, which
   (directory tree) corresponds to the directory tree in the source code directory;
2. all source codes of the driver are copied to the subdirectory of driver
   assembly source codes (`__kbuild_src`) and `Makefile` is configured to build
   the driver based on the template `Makefile_driver.in`;
3. the driver is built based on the rules specified in the configured `Makefile`.

Driver build target template:
```
DriverTarget(<driver_name>
    INCLUDE_DIRS    <list_of_include_directories>
    SOURCES         <list_of_source_files>
    EXTRA_CFLAGS    <list_of_c_compile_flags>
    EXTRA_LDFLAGS   <list_of_ld_flags>
    COMPILE_DEFINITIONS
        <list_of_compile_definitions>
)
```

Usage example:
```
DriverTarget(hello_module
    INCLUDE_DIRS
        module
    SOURCES
        main.c
    EXTRA_CFLAGS
        -Wno-unused-variable
        -Wno-unused-function
    EXTRA_LDFLAGS
        --strip-all
        -O3
    DEFINES
        MOD_NAME="hello_module"
        MOD_CONFIG
        MOD_NUM=1
)
```

### Externals

#### Externals

`ExternalTarget` creates a custom target to drive download, update/patch,
configure, build, install and test steps of an external project. `ExternalTarget`
supports keywords:
* `CONFIGURE_COMMAND` - configure command for target;
* `BUILD_COMMAND` - build command for target;
* `INSTALL_COMMAND` - build command for target;
* `INCLUDE_DIR` - directory relative to which the target's header files will
  be searched;
* `INSTALL_DIR` - installation directory. This directory will be searched for
  header files and libraries of the compiled target;
* `URL` - internet address or path to the archive with the target's source files;
* `LIBRARIES` - list of libraries provided by the target;
* `DEPENDS` - list of the target's dependencies.

External project build target template:
```
ExternalTarget(<ext_name>
    URL
    CONFIGURE_COMMAND   <command>
    BUILD_COMMAND       <command>
    INSTALL_COMMAND     <command>
    INSTALL_DIR     <directory>
    INCLUDE_DIR     <directory>
    LIBRARIES   <list_of_libraries>
    DEPENDS     <list_of_target_dependencies>
)
```

Usage example:
```
ExternalTarget(testing
    URL
        https://github.com/wstux/testing_template/archive/refs/heads/master.zip
    CONFIGURE_COMMAND
        cmake --install-prefix ${EXTERNALS_PREFIX}/testing/install  ./
    BUILD_COMMAND
        cmake --build ./
    INSTALL_COMMAND
        cmake --install ./
    INCLUDE_DIR
        ${EXTERNALS_PREFIX}/testing/install/include
    INSTALL_DIR
        ${EXTERNALS_PREFIX}/testing/install
)
```

#### Wrappers

### Custom targets

#### Custom commands

`CustomCommand` is a delineation of the `cmake` `add_custom_command` function.
Documentation on `add_custom_command` can be requested from the official
[website](https://cmake.org/cmake/help/latest/command/add_custom_command.html).

#### Custom targets

`CustomTarget` is a delineation of the `cmake` `add_custom_target` function.
Documentation on `add_custom_target` can be requested from the official
[website](https://cmake.org/cmake/help/latest/command/add_custom_target.html).

#### Custom test targets

`CustomTestTarget` declares a custom test build target that is not an executable
file. `cmake` supports a `test` metatarget that runs all declared tests,
`CustomTestTarget` is essentially a script file and is needed to use the `cmake`
build system testing framework. `CustomTestTarget` supports keywords:
* `INTERPRETER` - interpreter for running the script;
* `SOURCE` - script file with target source code;
* `ARGUMENTS` - command line arguments to run the script;
* `DEPENDS` - list of the target's dependencies;
* `DISABLE` - disable autorun test with `make test` command.

Custom test build target template:
```
CustomTestTarget(<test_name>
    INTERPRETER <interpreter>
    SOURCE      <source_file>
    ARGUMENTS   <list_of_arguments>
    DEPENDS     <list_of_target_dependencies>
    DISABLE
)
```

Usage example:
```
CustomTestTarget(ut_custom_test_target_with_args
    SOURCE
        ut_custom_test_target_with_args.sh
    INTERPRETER
        /bin/bash
    ARGUMENTS
        ${CMAKE_BINARY_DIR}
    DISABLE
)
```

## Build

## ToDo

To do:
* for `LibTarget` add the ability to specify several types of libraries for one
  target (`SHARED` and `STATIC`);
* for `LibTarget` change `HEADERS` or `INCLUDE_DIR` to specify the public
  interface, and make the rest private, [example](https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/).

## License

&copy; 2022 Chistyakov Alexander.

Open sourced under MIT license, the terms of which can be read here — [MIT License](http://opensource.org/licenses/MIT).


# CMake C++ Project Template
*cmake_cpp_project_template* is a simple template for C and C++ projects.
The template includes targets for building libraries, tests, and executables.

Currently supported build on GNU/Linux.

The *cmake_cpp_project_template* assumes you want to setup a project using
* CMake (3.0 or above)
* C/C++ compiler


## Contents

* [Usage](#usage)
* [Targets](#targets)
  * [Libraries](#libraries)
  * [Executables](#executables)
  * [Tests](#tests)
  * [Custom targets](#custom-targets)
  * [Drivers](#drivers)
  * [Externals](#externals)
* [Build](#build)
* [License](#license)

## Usage

### Install template

Template usage:
* add a template to the project as a submodule/copy to a subdirectory of the
  project root directory;
* copy the files `CMakeLists.txt.in` and `Makefile.in` to the root directory
  of the project;
* rename `CMakeLists.txt.in` -> `CMakeLists.txt` and replace `@project_name@`
  with the name of the project being developed, `@common_cmake_dir@` replace
  with the path to the directory with template files relative to the root
  directory of the project;
* rename `Makefile.in` -> `Makefile` and replace `@common_cmake_dir@` with the
  path to the template directory relative to the project root directory.

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
* [CustomTarget](#custom-targets) - adding custom target;
* [DriverTarget](#drivers) - building kernel modules;
* [ExternalTarget and WrapperTarget](#externals) - building external modules.

### Libraries

`LibTarget` declares the library as the target of the build. `LibTarget`
supports keywords:
* library type `SHARED`, `STATIC` or `INTERFACE`;
* `HEADERS` - list of header files included in the target (this parameter
  is optional);
* `SOURCES` - list of files with target source code;
* `INCLUDE_DIR` - directory relative to which the target's header files will
  be searched;
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
    INCLUDE_DIR <directory>
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
    INCLUDE_DIR
        libs
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
* `LIBRARIES` - list of libraries the target depends on;
* `DEPENDS` - list of the target's dependencies.

The `LIBRARIES` field specifies the list of libraries (targets) included in the
project, the `DEPENDS` field specifies the system/third-party libraries (targets)
required to build the project/target.

Test build target template:
```
TestTarget(<test_name>
    HEADERS     <list_of_headers>
    SOURCES     <list_of_source_files>
    LIBRARIES   <list_of_libraries_target_depends_on>
    DEPENDS     <list_of_target_dependencies>
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
)
```

### Custom targets

`CustomTarget` is a delineation of the `cmake` `add_custom_target` function.
Documentation on `add_custom_target` can be requested from the official [website](https://cmake.org/cmake/help/latest/command/add_custom_target.html).

### Drivers

`DriverTarget` declares the kernel driver as the build target. `DriverTarget`
supports keywords:
* `INCLUDE_DIR` - directory relative to which the target's header files will
  be searched;
* `SOURCES` - list of files with target source code;
* `EXTRA_CFLAGS` - list of additional target compilation flags;
* `EXTRA_LDFLAGS` - list of additional target linking flags;
* `DEFINES` - list of compile definitions.

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
    DEFINES         <list_of_compile_definitions>
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

#### Wrappers

## Build

## ToDo

To do:
* for `LibTarget` add the ability to specify several types of libraries for one
  target (`SHARED` and `STATIC`);
* for `LibTarget` change `HEADERS` or `INCLUDE_DIR` to specify the public
  interface, and make the rest private, [example](https://pabloariasal.github.io/2018/02/19/its-time-to- do-cmake-right/).

## License

&copy; 2022 Chistyakov Alexander.

Open sourced under MIT license, the terms of which can be read here — [MIT License](http://opensource.org/licenses/MIT).


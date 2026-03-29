if (BUILD_BOOST)
    ExternalTarget(boost
        URL
            https://github.com/boostorg/boost/releases/download/boost-1.88.0/boost-1.88.0-cmake.tar.gz
        #URL_MD5
        #    fe33be48eeb6de4dc2fefe60a3834c1f
        CONFIGURE_COMMAND
            ./bootstrap.sh --prefix=${EXTERNALS_PREFIX}/boost/install --with-toolset=gcc --with-libraries=intrusive
        INSTALL_COMMAND
            ./b2 linkflags=\"${CMAKE_EXE_LINKER_FLAGS}\" cflags=\"${CMAKE_C_FLAGS}\" cxxflags=\"${CMAKE_CXX_FLAGS}\" architecture=x86 address-model=64 link=shared,static --debug-configuration -q --prefix=${EXTERNALS_PREFIX}/boost/install --layout=tagged -j8 threading=multi runtime-link=shared define=BOOST_LOG_USE_NATIVE_SYSLOG variant=release install
        INCLUDE_DIR
            ${EXTERNALS_PREFIX}/boost/install/include
        INSTALL_DIR
            ${EXTERNALS_PREFIX}/boost/install
    )
endif()

if (BUILD_TESTS)
    ExternalTarget(testing
        VERSION "1.0.0"
        URL
            https://github.com/wstux/testing_template/archive/refs/tags/v1.0.0.tar.gz
        #URL_MD5
        #    fe33be48eeb6de4dc2fefe60a3834c1f
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

    ExternalTarget(googlebenchmark
        VERSION "1.9.5"
        URL     https://github.com/google/benchmark/archive/refs/tags/v1.9.5.tar.gz
        CONFIGURE_COMMAND
            cmake -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
                -DBENCHMARK_DOWNLOAD_DEPENDENCIES=OFF
                -DCMAKE_BUILD_TYPE=Release
                --install-prefix ${EXTERNALS_PREFIX}/googlebenchmark/install ./
        BUILD_COMMAND
            cmake --build ./ --config Release && cmake --install ./
        INCLUDE_DIR
            ${EXTERNALS_PREFIX}/googlebenchmark/install/include
        INSTALL_DIR
            ${EXTERNALS_PREFIX}/googlebenchmark/install
        LIBRARIES
            libbenchmark.a
            libbenchmark_main.a
    )

    ExternalTarget(googletest
        VERSION "1.17.0"
        URL     https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
        CONFIGURE_COMMAND
            cmake --install-prefix ${EXTERNALS_PREFIX}/googletest/install ./
        BUILD_COMMAND
            cmake --build ./ && cmake --install ./
        INCLUDE_DIR
            ${EXTERNALS_PREFIX}/googletest/install/include
        INSTALL_DIR
            ${EXTERNALS_PREFIX}/googletest/install
        LIBRARIES
            libgmock.a
            libgmock_main.a
            libgtest.a
            libgtest_main.a
    )
#    ExternalTarget(tbb
#        URL
#            https://github.com/uxlfoundation/oneTBB/archive/refs/tags/v2022.1.0.tar.gz
#        CONFIGURE_COMMAND
#            cmake --install-prefix ${EXTERNALS_PREFIX}/tbb/install -DTBB_TEST=OFF -DTBB_EXAMPLES=OFF -DTBBMALLOC_BUILD=OFF -DTBB_FILE_TRIM=OFF ./
#        BUILD_COMMAND
#            cmake --build ./
#        INSTALL_COMMAND
#            cmake --install ./
#        INCLUDE_DIR
#            ${EXTERNALS_PREFIX}/tbb/install/include
#        INSTALL_DIR
#            ${EXTERNALS_PREFIX}/tbb/install
#        LIBRARIES
#            libtbb.so
#    )

#    ExternalTarget(tstarling_thread_safe_lru
#        URL
#            https://github.com/tstarling/thread-safe-lru/archive/refs/heads/master.zip
#        INCLUDE_DIR
#            ${EXTERNALS_PREFIX}/tstarling_thread_safe_lru/src/tstarling_thread_safe_lru
#        #PATCHES
#        #    ${CMAKE_SOURCE_DIR}/externals/patches/2025.06.22_thread_safe_lru_use_std.patch
#        DEPENDS
#            tbb
#    )
endif()

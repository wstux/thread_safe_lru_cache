add_subdirectory(benchmark)

# Unit tests

TestTarget(ut_cache_std
    SOURCES
        ut_cache.cpp
    LIBRARIES
        thread_safe_cache
    COMPILE_DEFINITIONS
        THREAD_SAFE_CACHE_DISABLE_BOOST
    DEPENDS
        googletest
)

TestTarget(ut_thread_safe_cache_std
    SOURCES
        ut_thread_safe_cache.cpp
    LIBRARIES
        thread_safe_cache
    COMPILE_DEFINITIONS
        THREAD_SAFE_CACHE_DISABLE_BOOST
    DEPENDS
        googletest
)

TestTarget(ut_fifo_cache_std
    SOURCES
        ut_fifo_cache.cpp
    LIBRARIES
        thread_safe_cache
    COMPILE_DEFINITIONS
        THREAD_SAFE_CACHE_DISABLE_BOOST
    DEPENDS
        googletest
)

TestTarget(ut_lru_cache_std
    SOURCES
        ut_lru_cache.cpp
    LIBRARIES
        thread_safe_cache
    COMPILE_DEFINITIONS
        THREAD_SAFE_CACHE_DISABLE_BOOST
    DEPENDS
        googletest
)

TestTarget(ut_ttl_cache_std
    SOURCES
        ut_ttl_cache.cpp
    LIBRARIES
        thread_safe_cache
    COMPILE_DEFINITIONS
        THREAD_SAFE_CACHE_DISABLE_BOOST
    DEPENDS
        googletest
)

if (USE_BOOST_INTRUSIVE)
    TestTarget(ut_cache_intrusive
        SOURCES
            ut_cache.cpp
        LIBRARIES
            thread_safe_cache
        DEPENDS
            googletest
    )

    TestTarget(ut_thread_safe_cache_intrusive
        SOURCES
            ut_thread_safe_cache.cpp
        LIBRARIES
            thread_safe_cache
        DEPENDS
            googletest
    )

    TestTarget(ut_fifo_cache_intrusive
        SOURCES
            ut_fifo_cache.cpp
        LIBRARIES
            thread_safe_cache
        DEPENDS
            googletest
    )

    TestTarget(ut_lru_cache_intrusive
        SOURCES
            ut_lru_cache.cpp
        LIBRARIES
            thread_safe_cache
        DEPENDS
            googletest
    )

    TestTarget(ut_ttl_cache_intrusive
        SOURCES
            ut_ttl_cache.cpp
        LIBRARIES
            thread_safe_cache
        DEPENDS
            googletest
    )
endif()

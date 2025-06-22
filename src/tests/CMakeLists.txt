# Unit tests

TestTarget(ut_lru_cache
    SOURCES
        ut_lru_cache.cpp
    LIBRARIES
        lru_cache
    DEPENDS
        testing
)

TestTarget(ut_ttl_cache
    SOURCES
        ut_ttl_cache.cpp
    LIBRARIES
        ttl_cache
    DEPENDS
        testing
)

if (USE_BOOST_INTRUSIVE)
    TestTarget(ut_std_lru_cache
        SOURCES
            ut_lru_cache.cpp
        LIBRARIES
            lru_cache
        COMPILE_DEFINITIONS
            THREAD_SAFE_CACHE_DISABLE_BOOST
        DEPENDS
            testing
    )

    TestTarget(ut_std_ttl_cache
        SOURCES
            ut_ttl_cache.cpp
        LIBRARIES
            ttl_cache
        COMPILE_DEFINITIONS
            THREAD_SAFE_CACHE_DISABLE_BOOST
        DEPENDS
            testing
    )
endif()

# Performance tests

TestTarget(perf_lru_cache
    SOURCES
        perf_lru_cache.cpp
    LIBRARIES
        lru_cache
    DEPENDS
        testing
)

TestTarget(perf_thread_safe_lru_cache
    SOURCES
        perf_thread_safe_lru_cache.cpp
    LIBRARIES
        lru_cache
    DEPENDS
        testing
)

if (BUILD_EXT_LRU)
    TestTarget(perf_compare
        SOURCES
            perf_compare.cpp
        LIBRARIES
            lru_cache
        DEPENDS
            testing
            tbb
            tstarling_thread_safe_lru
    )
endif()

if (USE_BOOST_INTRUSIVE)
    TestTarget(perf_std_lru_cache
        SOURCES
            perf_lru_cache.cpp
        LIBRARIES
            lru_cache
        COMPILE_DEFINITIONS
            THREAD_SAFE_CACHE_DISABLE_BOOST
        DEPENDS
            testing
    )

    TestTarget(perf_thread_safe_std_lru_cache
        SOURCES
            perf_thread_safe_lru_cache.cpp
        LIBRARIES
            lru_cache
        COMPILE_DEFINITIONS
            THREAD_SAFE_CACHE_DISABLE_BOOST
        DEPENDS
            testing
    )

    if (BUILD_EXT_LRU)
        TestTarget(perf_std_compare
            SOURCES
                perf_compare.cpp
            COMPILE_DEFINITIONS
                THREAD_SAFE_CACHE_DISABLE_BOOST
            LIBRARIES
                lru_cache
            DEPENDS
                testing
                tbb
                tstarling_thread_safe_lru
        )
    endif()
endif()

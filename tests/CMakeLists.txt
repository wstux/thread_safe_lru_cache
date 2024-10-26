# Unit tests

TestTarget(ut_lru_cache
    SOURCES
        ut_lru_cache.cpp
    LIBRARIES
        lru_cache
    DEPENDS
        testing
)

if (THREAD_SAFE_LRU_CACHE_USE_BOOST)
    TestTarget(ut_std_lru_cache
        SOURCES
            ut_lru_cache.cpp
        LIBRARIES
            lru_cache
        COMPILE_DEFINITIONS
            THREAD_SAFE_LRU_CACHE_DISABLE_BOOST
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

if (THREAD_SAFE_LRU_CACHE_USE_BOOST)
    TestTarget(perf_std_lru_cache
        SOURCES
            perf_lru_cache.cpp
        LIBRARIES
            lru_cache
        COMPILE_DEFINITIONS
            THREAD_SAFE_LRU_CACHE_DISABLE_BOOST
        DEPENDS
            testing
    )

    TestTarget(perf_thread_safe_std_lru_cache
        SOURCES
            perf_thread_safe_lru_cache.cpp
        LIBRARIES
            lru_cache
        COMPILE_DEFINITIONS
            THREAD_SAFE_LRU_CACHE_DISABLE_BOOST
        DEPENDS
            testing
    )
endif()

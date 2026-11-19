# Performance tests

PerfTestTarget(perf_cache_std
    SOURCES
        perf_cache.cpp
    LIBRARIES
        thread_safe_cache
    COMPILE_DEFINITIONS
        THREAD_SAFE_CACHE_DISABLE_BOOST
    DEPENDS
        googlebenchmark
    DISABLE
)

PerfTestTarget(perf_thread_safe_cache_std
    SOURCES
        perf_thread_safe_cache.cpp
    LIBRARIES
        thread_safe_cache
    COMPILE_DEFINITIONS
        THREAD_SAFE_CACHE_DISABLE_BOOST
    DEPENDS
        googlebenchmark
    DISABLE
)

if (USE_BOOST_INTRUSIVE)
    PerfTestTarget(perf_cache_intrusive
        SOURCES
            perf_cache.cpp
        LIBRARIES
            thread_safe_cache
        DEPENDS
            googlebenchmark
        DISABLE
    )

    PerfTestTarget(perf_thread_safe_cache_intrusive
        SOURCES
            perf_thread_safe_cache.cpp
        LIBRARIES
            thread_safe_cache
        DEPENDS
            googlebenchmark
        DISABLE
    )
endif()

# Performance tests

PerfTestTarget(perf_cache
    SOURCES
        perf_cache.cpp
    LIBRARIES
        thread_safe_cache
    DEPENDS
        googlebenchmark
    DISABLE
)

PerfTestTarget(perf_thread_safe_cache
    SOURCES
        perf_thread_safe_cache.cpp
    LIBRARIES
        thread_safe_cache
    DEPENDS
        googlebenchmark
    DISABLE
)

#PerfTestTarget(perf_lru_cache
#    SOURCES
#        perf_lru_cache.cpp
#    LIBRARIES
#        thread_safe_cache
#    DEPENDS
#        testing
#    DISABLE
#)

#PerfTestTarget(perf_thread_safe_lru_cache
#    SOURCES
#        perf_thread_safe_lru_cache.cpp
#    LIBRARIES
#        thread_safe_cache
#    DEPENDS
#        testing
#    DISABLE
#)

if (USE_BOOST_INTRUSIVE)
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

#    PerfTestTarget(perf_lru_cache_std
#        SOURCES
#            perf_lru_cache.cpp
#        LIBRARIES
#            thread_safe_cache
#        COMPILE_DEFINITIONS
#            THREAD_SAFE_CACHE_DISABLE_BOOST
#        DEPENDS
#            testing
#        DISABLE
#    )

#    PerfTestTarget(perf_thread_safe_lru_cache_std
#        SOURCES
#            perf_thread_safe_lru_cache.cpp
#        LIBRARIES
#            thread_safe_cache
#        COMPILE_DEFINITIONS
#            THREAD_SAFE_CACHE_DISABLE_BOOST
#        DEPENDS
#            testing
#        DISABLE
#    )
endif()

# Unit tests

TestTarget(ut_lru_cache
    SOURCES
        ut_lru_cache.cpp
    LIBRARIES
        thread_safe_cache
    DEPENDS
        googletest
)

TestTarget(ut_ttl_cache
    SOURCES
        ut_ttl_cache.cpp
    LIBRARIES
        ttl_cache
    DEPENDS
        googletest
)

if (USE_BOOST_INTRUSIVE)
    TestTarget(ut_std_lru_cache
        SOURCES
            ut_lru_cache.cpp
        LIBRARIES
            thread_safe_cache
        COMPILE_DEFINITIONS
            THREAD_SAFE_CACHE_DISABLE_BOOST
        DEPENDS
            googletest
    )

    TestTarget(ut_std_ttl_cache
        SOURCES
            ut_ttl_cache.cpp
        LIBRARIES
            ttl_cache
        COMPILE_DEFINITIONS
            THREAD_SAFE_CACHE_DISABLE_BOOST
        DEPENDS
            googletest
    )
endif()

# Performance tests

#TestTarget(perf_lru_cache
#    SOURCES
#        perf_lru_cache.cpp
#    LIBRARIES
#        thread_safe_cache
#    DEPENDS
#        testing
#)

#TestTarget(perf_thread_safe_lru_cache
#    SOURCES
#        perf_thread_safe_lru_cache.cpp
#    LIBRARIES
#        thread_safe_cache
#    DEPENDS
#        testing
#)

if (USE_BOOST_INTRUSIVE)
#    TestTarget(perf_std_lru_cache
#        SOURCES
#            perf_lru_cache.cpp
#        LIBRARIES
#            thread_safe_cache
#        COMPILE_DEFINITIONS
#            THREAD_SAFE_CACHE_DISABLE_BOOST
#        DEPENDS
#            testing
#    )

#    TestTarget(perf_thread_safe_std_lru_cache
#        SOURCES
#            perf_thread_safe_lru_cache.cpp
#        LIBRARIES
#            thread_safe_cache
#        COMPILE_DEFINITIONS
#            THREAD_SAFE_CACHE_DISABLE_BOOST
#        DEPENDS
#            testing
#    )
endif()

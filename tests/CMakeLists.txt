# Unit tests

TestTarget(ut_lru_cache
    SOURCES
        ut_lru_cache.cpp
    LIBRARIES
        lru_cache
    DEPENDS
        testing
)

# Performance tests

ExecTarget(perf_lru_cache
    SOURCES
        perf_lru_cache.cpp
    LIBRARIES
        lru_cache
    DEPENDS
        testing
)

ExecTarget(perf_thread_safe_lru_cache
    SOURCES
        perf_thread_safe_lru_cache.cpp
    LIBRARIES
        lru_cache
    DEPENDS
        testing
)


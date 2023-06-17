# Unit tests

TestTarget(ut_lru_cache
    SOURCES
        ut_lru_cache.cpp
    LIBRARIES
        lru_cache
    DEPENDS
        testing
)

TestTarget(ut_thread_safe_lru_cache
    SOURCES
        ut_thread_safe_lru_cache.cpp
    LIBRARIES
        lru_cache
    DEPENDS
        testing
)


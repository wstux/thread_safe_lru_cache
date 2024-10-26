LibTarget(lru_cache INTERFACE
    INCLUDE_DIR libs
    COMPILE_DEFINITIONS
                ${THREAD_SAFE_LRU_CACHE_USE_BOOST}
    DEPENDS     boost
)


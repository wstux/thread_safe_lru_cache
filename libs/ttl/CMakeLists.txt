LibTarget(ttl_cache INTERFACE
    INCLUDE_DIR libs
    COMPILE_DEFINITIONS
                ${THREAD_SAFE_CACHE_USE_BOOST}
    DEPENDS     boost
)


add_subdirectory(libs/lru)
add_subdirectory(libs/ttl)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()


find_package(Boost)

add_custom_target(boost)
if(Boost_FOUND)
    message(STATUS "[INFO ] boost::intrusive has been found")

    set_target_properties(boost
                          PROPERTIES
                              LIBRARIES "${Boost_LIBRARIES}"
    )
    set_target_properties(boost
                          PROPERTIES
                              INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
    )

    set(THREAD_SAFE_CACHE_USE_BOOST "THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE" PARENT_SCOPE)
else()
    message(STATUS "[INFO ] boost::intrusive has not been found")
endif()


include(ExternalProject)

set(TARGET_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
set(TARGET_LIBS "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(TARGET_INCLUDES "${CMAKE_CURRENT_BINARY_DIR}/include")
set(libgtest_INCLUDES "${TARGET_INCLUDES}/google")


ExternalProject_Add(googletest
        PREFIX ${TARGET_PREFIX}
        URL ${PROJECT_SOURCE_DIR}/deps/googletest
        BUILD_IN_SOURCE 1
        CMAKE_ARGS -Dlibgtest_force_shared_crt=ON -DCMAKE_DEBUG_POSTFIX=""
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON
        INSTALL_COMMAND   ""
)

# define imported library libgtest
add_library(GTest IMPORTED STATIC GLOBAL)

# define imported library libgtest_main
add_library(GTestMain IMPORTED STATIC GLOBAL)

# define imported library libgtest
add_library(GMock IMPORTED STATIC GLOBAL)

# define imported library libgtest_main
add_library(GMockMain IMPORTED STATIC GLOBAL)

set_target_properties(GTest PROPERTIES
        "IMPORTED_LOCATION" "${TARGET_LIBS}/libgtest${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "INTERFACE_INCLUDE_DIRECTORIES"     "${libgtest_INCLUDES}"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )

set_target_properties(GTestMain PROPERTIES
        "IMPORTED_LOCATION" "${TARGET_LIBS}/libgtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )


set_target_properties(GMock PROPERTIES
        "IMPORTED_LOCATION" "${TARGET_LIBS}/libgmock${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )

set_target_properties(GMockMain PROPERTIES
        "IMPORTED_LOCATION" "${TARGET_LIBS}/libgmock_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )


# the libgtest include directory exists only after it is build, but it is used/needed
# for the set_target_properties call below, so make it to avoid an error
file(MAKE_DIRECTORY ${libgtest_INCLUDES})

# make libgtest depend on googletest
add_dependencies(GTest googletest)
add_dependencies(GTestMain googletest)
add_dependencies(GMock googletest)
add_dependencies(GMockMain googletest)

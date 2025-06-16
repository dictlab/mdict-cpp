include(ExternalProject)

set(TARGET_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
set(TARGET_LIBS "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(TARGET_INCLUDES "${CMAKE_CURRENT_BINARY_DIR}/include")
set(miniz_INCLUDES "${TARGET_INCLUDES}/miniz")

ExternalProject_Add(miniz
    PREFIX ${TARGET_PREFIX}
    URL ${PROJECT_SOURCE_DIR}/deps/miniz
    BUILD_IN_SOURCE 1
    CMAKE_ARGS -DCMAKE_DEBUG_POSTFIX=""
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
    INSTALL_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} --build . --target miniz
    COMMAND ${CMAKE_COMMAND} -E make_directory ${TARGET_LIBS}
    COMMAND ${CMAKE_COMMAND} -E copy ${TARGET_PREFIX}/src/miniz/libminiz${CMAKE_STATIC_LIBRARY_SUFFIX} ${TARGET_LIBS}/libminiz${CMAKE_STATIC_LIBRARY_SUFFIX}
)

# define imported library libminiz
add_library(Miniz IMPORTED STATIC GLOBAL)

set_target_properties(Miniz PROPERTIES
    "IMPORTED_LOCATION" "${TARGET_LIBS}/libminiz${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "INTERFACE_INCLUDE_DIRECTORIES" "${miniz_INCLUDES}"
)

# the miniz include directory exists only after it is built, but it is used/needed
# for the set_target_properties call above, so make it to avoid an error
file(MAKE_DIRECTORY ${miniz_INCLUDES})

# make Miniz depend on miniz
add_dependencies(Miniz miniz) 
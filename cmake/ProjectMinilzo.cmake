include(ExternalProject)

set(TARGET_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
set(TARGET_LIBS "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(TARGET_INCLUDES "${CMAKE_CURRENT_BINARY_DIR}/include")
set(minilzo_INCLUDES "${TARGET_INCLUDES}/minilzo")

ExternalProject_Add(minilzo
    PREFIX ${TARGET_PREFIX}
    URL ${PROJECT_SOURCE_DIR}/deps/minilzo
    BUILD_IN_SOURCE 1
    CMAKE_ARGS -DCMAKE_DEBUG_POSTFIX=""
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
    INSTALL_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} --build . --target minilzo
    COMMAND ${CMAKE_COMMAND} -E make_directory ${TARGET_LIBS}
    COMMAND ${CMAKE_COMMAND} -E copy ${TARGET_PREFIX}/src/minilzo/libminilzo${CMAKE_STATIC_LIBRARY_SUFFIX} ${TARGET_LIBS}/libminilzo${CMAKE_STATIC_LIBRARY_SUFFIX}
)

# define imported library libminilzo
add_library(Minilzo IMPORTED STATIC GLOBAL)

set_target_properties(Minilzo PROPERTIES
    "IMPORTED_LOCATION" "${TARGET_LIBS}/libminilzo${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "INTERFACE_INCLUDE_DIRECTORIES" "${minilzo_INCLUDES}"
)

# the minilzo include directory exists only after it is built, but it is used/needed
# for the set_target_properties call above, so make it to avoid an error
file(MAKE_DIRECTORY ${minilzo_INCLUDES})

# make Minilzo depend on minilzo
add_dependencies(Minilzo minilzo) 
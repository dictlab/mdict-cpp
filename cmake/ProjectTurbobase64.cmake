include(ExternalProject)

set(TARGET_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
set(TARGET_LIBS "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(TARGET_INCLUDES "${CMAKE_CURRENT_BINARY_DIR}/include")
set(turbobase64_INCLUDES "${TARGET_INCLUDES}/turbobase64")

ExternalProject_Add(turbobase64
    PREFIX ${TARGET_PREFIX}
    URL ${PROJECT_SOURCE_DIR}/deps/turbobase64
    BUILD_IN_SOURCE 1
    CMAKE_ARGS -DCMAKE_DEBUG_POSTFIX=""
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
    INSTALL_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} --build . --target base64
    COMMAND ${CMAKE_COMMAND} -E make_directory ${TARGET_LIBS}
    COMMAND ${CMAKE_COMMAND} -E copy ${TARGET_PREFIX}/src/turbobase64/libbase64${CMAKE_STATIC_LIBRARY_SUFFIX} ${TARGET_LIBS}/libturbobase64${CMAKE_STATIC_LIBRARY_SUFFIX}
)

# define imported library libturbobase64
add_library(TurboBase64 IMPORTED STATIC GLOBAL)

set_target_properties(TurboBase64 PROPERTIES
    "IMPORTED_LOCATION" "${TARGET_LIBS}/libturbobase64${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "INTERFACE_INCLUDE_DIRECTORIES" "${turbobase64_INCLUDES}"
)

# the turbobase64 include directory exists only after it is built, but it is used/needed
# for the set_target_properties call above, so make it to avoid an error
file(MAKE_DIRECTORY ${turbobase64_INCLUDES})

# make TurboBase64 depend on turbobase64
add_dependencies(TurboBase64 turbobase64) 
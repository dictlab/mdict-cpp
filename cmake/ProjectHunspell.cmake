include(ExternalProject)

set(TARGET_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
set(TARGET_LIBS "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(TARGET_INCLUDES "${CMAKE_CURRENT_BINARY_DIR}/include")

set(HUNSPELL_CONFIG autoreconf -vfi && ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR})
set(HUNSPELL_BUILD make)
set(HUNSPELL_INSTALL make install)


ExternalProject_Add(hunspell
        PREFIX ${TARGET_PREFIX}
        URL file://${PROJECT_SOURCE_DIR}/deps/hunspell
        BUILD_IN_SOURCE 1
        LOG_CONFIGURE 0
        LOG_BUILD 0
        LOG_INSTALL 0
        CONFIGURE_COMMAND ${HUNSPELL_CONFIG}
        BUILD_COMMAND ${HUNSPELL_BUILD}
        INSTALL_COMMAND ${HUNSPELL_INSTALL}
)

ADD_LIBRARY(HUNSPELL STATIC IMPORTED)
add_dependencies(HUNSPELL hunspell)

set_target_properties(HUNSPELL PROPERTIES
        IMPORTED_CONFIGURATIONS "DEBUG"
        IMPORTED_LOCATION_DEBUG "${TARGET_LIBS}/libhunspell-1.7.a"
        )
set_target_properties(HUNSPELL PROPERTIES
        IMPORTED_CONFIGURATIONS "RELEASE"
        IMPORTED_LOCATION_RELEASE "${TARGET_LIBS}/libhunspell-1.7.a"
        )

include(ExternalProject)

set(HUNSPELL_CONFIG autoreconf -vfi && ./configure --prefix=${PROJECT_SOURCE_DIR}/target)
set(HUNSPELL_BUILD make)
set(HUNSPELL_INSTALL make install)


ExternalProject_Add(hunspell
        PREFIX ${PROJECT_SOURCE_DIR}/target/deps/hunspell
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
        IMPORTED_LOCATION_DEBUG "${PROJECT_SOURCE_DIR}/target/lib/libhunspell-1.7.a"
        )
set_target_properties(HUNSPELL PROPERTIES
        IMPORTED_CONFIGURATIONS "RELEASE"
        IMPORTED_LOCATION_RELEASE "${PROJECT_SOURCE_DIR}/target/lib/libhunspell-1.7.a"
        )

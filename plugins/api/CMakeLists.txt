configure_file(api.json.in "${CMAKE_CURRENT_BINARY_DIR}/api.json")

set(SRC
    api.h
    api.cpp
    apiplugin.h
    apiplugin.cpp
    apiserver.h
    apiserver.cpp
)

add_library(api MODULE ${SRC})

set_target_properties(api PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_link_libraries(api nitroshare qhttpengine)

install(TARGETS api
    LIBRARY DESTINATION "${INSTALL_PLUGIN_PATH}"
)

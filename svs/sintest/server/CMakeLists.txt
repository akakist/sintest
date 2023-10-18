cmake_minimum_required (VERSION 3.0)

add_library(sinTestServer SHARED
    sinTestServer.cpp
    sinTestServerService.cpp

)

target_link_libraries(sinTestServer   megatron-core pthread)

install(TARGETS sinTestServer DESTINATION ${PLUGIN_TARGET_DIR})


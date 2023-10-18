cmake_minimum_required (VERSION 3.0)

add_library(sinTestClient SHARED
sinTestClient.cpp
sinTestClientService.cpp
)

target_link_libraries(sinTestClient   megatron-core pthread)

install(TARGETS sinTestClient DESTINATION ${PLUGIN_TARGET_DIR})


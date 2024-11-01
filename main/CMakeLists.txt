set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")

FILE(GLOB_RECURSE app_sources
        ${CMAKE_SOURCE_DIR}/main/*.cpp
        ${CMAKE_SOURCE_DIR}/main/led/*.cpp
)

idf_component_register(SRCS ${app_sources} INCLUDE_DIRS "." REQUIRES core-esp32 fmt exchange)

spiffs_create_partition_image(storage ../data)
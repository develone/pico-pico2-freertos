add_library(crc_crc16 INTERFACE)
target_include_directories(crc_crc16 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/crc_crc16_lib)
target_sources(crc_crc16 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/crc_crc16_lib/crc.c
    ${CMAKE_CURRENT_LIST_DIR}/crc_crc16_lib/crc.h
    ${CMAKE_CURRENT_LIST_DIR}/crc_crc16_lib/crc16.c
    ${CMAKE_CURRENT_LIST_DIR}/crc_crc16_lib/crc16.h
)
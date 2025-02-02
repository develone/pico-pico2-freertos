add_library(cir-buf INTERFACE)
target_include_directories(cir-buf INTERFACE ${CMAKE_CURRENT_LIST_DIR}/cir-buf-lib)
target_sources(cir-buf INTERFACE
 
    ${CMAKE_CURRENT_LIST_DIR}/cir-buf-lib/head-tail.c
    ${CMAKE_CURRENT_LIST_DIR}/cir-buf-lib/head-tail.h
 
 
)




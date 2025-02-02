add_library(klt INTERFACE)
target_include_directories(klt INTERFACE ${CMAKE_CURRENT_LIST_DIR}/klt-lib)
target_sources(klt INTERFACE
 
    ${CMAKE_CURRENT_LIST_DIR}/klt-lib/selectGoodFeatures.c
    ${CMAKE_CURRENT_LIST_DIR}/klt-lib/pyramid.c
    ${CMAKE_CURRENT_LIST_DIR}/klt-lib/pyramid.h
    ${CMAKE_CURRENT_LIST_DIR}/klt-lib/convolve.c
    ${CMAKE_CURRENT_LIST_DIR}/klt-lib/convolve.h
    ${CMAKE_CURRENT_LIST_DIR}/klt-lib/error.c
    ${CMAKE_CURRENT_LIST_DIR}/klt-lib/error.h
    ${CMAKE_CURRENT_LIST_DIR}/klt-lib/klt.c
    ${CMAKE_CURRENT_LIST_DIR}/klt-lib/klt.h
    ${CMAKE_CURRENT_LIST_DIR}/klt-lib/klt_util.c
    ${CMAKE_CURRENT_LIST_DIR}/klt-lib/klt_util.h
    #${CMAKE_CURRENT_LIST_DIR}/klt-lib/pnmio.c
    #${CMAKE_CURRENT_LIST_DIR}/klt-lib/pnmio.h
    
 
)




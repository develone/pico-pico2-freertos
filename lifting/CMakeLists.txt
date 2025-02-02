add_library(lifting INTERFACE)
target_include_directories(lifting INTERFACE ${CMAKE_CURRENT_LIST_DIR}/lifting-lib)
target_sources(lifting INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/lifting-lib/lifting.c
 
	${CMAKE_CURRENT_LIST_DIR}/lifting-lib/lifting.h
 
	

)



add_library(xmodem INTERFACE)
target_include_directories(xmodem INTERFACE ${CMAKE_CURRENT_LIST_DIR}/xmodem-lib)
target_sources(xmodem INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/xmodem-lib/xtransmit.c
    ${CMAKE_CURRENT_LIST_DIR}/xmodem-lib/xreceive.c
    ${CMAKE_CURRENT_LIST_DIR}/xmodem-lib/xcommon.h
    ${CMAKE_CURRENT_LIST_DIR}/xmodem-lib/xreceive.h
		${CMAKE_CURRENT_LIST_DIR}/xmodem-lib/xtransmit.h
)


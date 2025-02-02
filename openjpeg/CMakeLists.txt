add_library(openjpeg INTERFACE)
target_include_directories(openjpeg INTERFACE ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib)
target_sources(openjpeg INTERFACE
 
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/bio.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/bio.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/cio.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/cio.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/dwt.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/dwt.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/event.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/event.h

    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/function_list.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/function_list.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/image.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/image.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/invert.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/invert.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/j2k.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/j2k.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/mct.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/mct.c 
    #${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/mct_inl.h 
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/openjpeg.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/openjpeg.h 
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/opj_clock.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/opj_clock.h

 
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/opj_codec.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/opj_common.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/opj_config.h

    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/opj_config_private.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/opj_includes.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/opj_intmath.h
    
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/opj_inttypes.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/opj_malloc.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/opj_malloc.h

    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/pi.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/pi.h

    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/rdwr.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/rd-wr-ops.c

    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/sparse_array.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/sparse_array.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/t1.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/t1.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/t1_luts.h

    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/tcd.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/tcd.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/tgt.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/tgt.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/thread.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/thread.h
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/tls_keys.h
		${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/dwtlift.c
    ${CMAKE_CURRENT_LIST_DIR}/openjpeg-lib/dwtlift.h


)




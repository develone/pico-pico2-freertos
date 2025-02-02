if (NOT FREERTOS_KERNEL_PATH AND NOT DEFINED ENV{FREERTOS_KERNEL_PATH})
    message("Skipping FreeRTOS examples as FREERTOS_KERNEL_PATH not defined")
    return()
endif()

include(FreeRTOS_Kernel_import.cmake)

add_subdirectory(hello_freertos)

add_subdirectory(2tasks)
add_subdirectory(Mutex)
add_subdirectory(Semaphore)
add_subdirectory(Scheduling)
add_subdirectory(test-read-crc16)

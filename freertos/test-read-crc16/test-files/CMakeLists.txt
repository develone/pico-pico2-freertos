add_executable(test-read-crc16
        main.c crc16.h xreceive.c xreceive.h xtransmit.c xtransmit.h lfs.h vi.c vi.h pshell.h pico_hal.h 
)

target_link_libraries(test-read-crc16 pico_stdlib freertos lifting crc16 klt littlefs-lib)

pico_enable_stdio_usb(test-read-crc16 1)
pico_enable_stdio_uart(test-read-crc16 1)

pico_add_extra_outputs(test-read-crc16)

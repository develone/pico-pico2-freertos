add_executable(klt-test
        main.c
)

target_link_libraries(klt-test pico_stdlib lifting crc_crc16 cir-buf klt)
pico_add_extra_outputs(klt-test)
pico_enable_stdio_usb(klt-test 1)
pico_enable_stdio_uart(klt-test 0)

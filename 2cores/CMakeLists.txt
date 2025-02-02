
add_executable(multicore
        multicore.c
)

target_link_libraries(multicore
        pico_multicore
        pico_stdlib
        hardware_adc
)

pico_enable_stdio_usb(multicore 1)
pico_enable_stdio_uart(multicore 0)

pico_add_extra_outputs(multicore)
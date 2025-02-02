add_executable(slave_i2c
        slave_i2c.c
        )

# pull in common dependencies and additional i2c hardware support
target_link_libraries(slave_i2c pico_stdlib hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(slave_i2c)
pico_enable_stdio_usb(slave_i2c 0)
pico_enable_stdio_uart(slave_i2c 1)
# add url via pico_set_program_url
#example_auto_set_url(slave_i2c)

add_executable(50_pwm
        main.c
        )

# pull in common dependencies and additional pwm hardware support
target_link_libraries(50_pwm pico_stdlib hardware_pwm)

# create map/bin/hex file etc.
pico_add_extra_outputs(50_pwm)

# add url via pico_set_program_url
#example_auto_set_url(50_pwm)

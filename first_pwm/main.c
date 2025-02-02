#include "pico/stdlib.h"
#include "hardware/pwm.h"

int main() {
    
    // Enable GPIO 0 port to PWM
    gpio_set_function(0, GPIO_FUNC_PWM);

		//get the PWM channel from the GPIO
    uint slice_num = pwm_gpio_to_slice_num(0);
		
		//turn it on
		pwm_set_enabled(slice_num, true);

		//set the wrap_point
		pwm_set_wrap(slice_num, 500);

		//set the set point
		pwm_set_chan_level(slice_num, PWM_CHAN_A, 250);

		while(1)
		{
			tight_loop_contents();
		}
}
#include <stdio.h>
#include "led.h"







void kernel_main(){
	





	while(1){
		led_on();
		delay();
		led_off();
		delay();
	}
}


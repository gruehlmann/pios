#include <stdio.h>
#include "list.h"
#include "led.h"


extern long __bss_start;
extern long __bss_end;

int *mu_io_reg = 0x3F215040;
void kernel_main() {

	clearbss();
	struct list_element c = {NULL, 0};
	struct list_element b = {&c, 0};
	struct list_element a = {&b, 1};
	struct list_element *head = &a;
	
	esp_printf(putc, "hello world");

	
	return;




//	while (1){
//		led_on();
//		delay();
//		led_off();
		delay();
	}
}

int clearbss() {
	long *begin_bss = &__bss_start;
	long *end_bss = &__bss_end;
	while(begin_bss != end_bss){
		*begin_bss = 0;
		begin_bss++;
	}
	return 0;
}

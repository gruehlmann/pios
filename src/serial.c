#include "serial.h"
#include <stdint.h>



void putc(int data) {
	uint32_t *mu_io_reg = (uint32_t *) 0x3F215040;
	*mu_io_reg = data;
}

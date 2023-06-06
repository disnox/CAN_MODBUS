#ifndef __PROTOCOL_REGISTER_MANAGEMENT_H__
#define __PROTOCOL_REGISTER_MANAGEMENT_H__


#include <stdint.h>





#define REGISTER_POINT 

#ifdef REGISTER_POINT
#define RegisterSize 0xff

int8_t register_map(uint16_t address, uint8_t *point, uint16_t size);

#endif


int8_t register_write(uint16_t address, uint8_t *buff, uint16_t size);
int8_t register_read(uint16_t address, uint16_t size, uint8_t *buff);


void register_test(void);





#endif





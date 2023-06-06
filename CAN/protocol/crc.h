/*
 * crc.h
 *
 *  Created on: 2020年4月21日
 *      Author: user
 */

#ifndef CRC_H_
#define CRC_H_
#define CRC16_INIT_VALUE        (0xFFFF)
#include "main.h"
//#include "TIM.h"

typedef struct
{
    uint16_t current;
    uint16_t crc16;
} crc16_t;
void crc16(uint8_t input, crc16_t *output);
uint16_t crc_ccitt(uint8_t *input, uint8_t len);
#endif /* CRC_H_ */

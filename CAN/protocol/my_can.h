#ifndef __MY_CAN_H__
#define __MY_CAN_H__

#include "main.h"
#include "para.h"
#include "protocol.h"
#include "crc.h"
#include "can.h"
#include "protocol_register_management.h"

#define CAN_SEND_ID 0X500
#define CAN_RECEIVE_ID 0X580
#define CAN_BUFFER_SIZE   (128)

typedef struct
{
    uint8_t *data;
    uint16_t read_index, write_index;
    uint16_t size;
} Can_fifo_buffer_t;

extern uint8_t Car_buff[CAN_BUFFER_SIZE];

uint16_t can_serial_available(void);
uint8_t can_serial_read_char(void);
uint16_t can_serial_write(uint8_t *buffer, uint16_t length); //∑¢ÀÕ
uint16_t can_serial_read(uint8_t *buffer, uint16_t length);  //Ω” ’

extern void can_filter_init(void);
extern void CAN_Para_Init(void);
extern uint8_t CANx_SendStdData(CAN_HandleTypeDef* hcan,uint16_t ID,uint8_t *pData,uint16_t Len);
extern void can_receive(void);
extern void can_transmit(void);


#endif



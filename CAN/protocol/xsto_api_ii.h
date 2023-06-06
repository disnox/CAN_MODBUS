/**********************************************************/
//@Autor:Raw
//@Version:0.0.1
//@End Time:2023/4/17
/**********************************************************/
#ifndef __XSTO_API_II_H__
#define __XSTO_API_II_H__
#include "para.h"
#include "crc.h"
#define VERSION(ver, sub,area, biuld) (((uint32_t)ver << 24)|((uint32_t)sub << 16)|((uint32_t)area << 8)|(biuld & 0xFF))
#define MODULE_STATUS_OFFLINE 1
#define MODULE_STATUS_ONLINE  0

#define HARDWAREVERSION  VERSION(1,0,0,2)
#define SOFTWAREVERSION  VERSION(1,0,0,3)


typedef struct
{
	uint16_t TYPE;
	uint8_t cmd;
	uint16_t Length;	
	uint8_t * Data;
}MsgData_t;

typedef struct
{
	uint16_t TYPE;
	uint8_t cmd;
	uint8_t msg_id;
	uint16_t Length;	
	uint8_t * Data;
}MasterMsgData_t;

typedef struct {
	uint16_t HEAD;
	MsgData_t msg_data;
} API_Message_t;
	
typedef struct {
	uint32_t ModuleID[3];
	uint16_t Type;		
	uint8_t  Status; 
	uint32_t SoftwareVersion;
	uint32_t HardwareVersion;	
	uint16_t ModuleTimeout;	
} API_Config_t;
void api_init(API_Config_t* config);
void api_message_send(API_Message_t *msg);
void api_port_received(uint16_t len, uint8_t * data);
void api_loop(uint32_t time);
//weak
void api_on_message_received(MasterMsgData_t message);
void api_port_send(uint16_t len, uint8_t * data) ;
void protocol_init(void);
#endif

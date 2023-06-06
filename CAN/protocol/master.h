#ifndef __MASTER_H
#define	__MASTER_H
#include "xsto_api_ii.h"
#include "alloc.h"
#include "protocol_api.h"
#include "protocol.h"
#include "protocol_register_management.h"


typedef struct
{
	uint32_t	SoftwareVersion; 		//	固件号						  R
	uint32_t	Pcbversion;					//	硬件号						  R
	uint8_t		A_MotorStatus;			//	A电机状态				  R
	uint8_t		B_MotorStatus;			//	B电机状态				  R
	uint8_t		A_MotorEnableStatus;//	A电机使能				  RW
	uint8_t		B_MotorEnableStatus;//	B电机使能					RW
	uint8_t 	A_MotorMode;				//	A电机模式 				  RW
	uint8_t		B_MotorMode;				//	B电机模式					RW		
	float			A_SpeedSetData;			//	A电机设定速度			RW
	float			B_SpeedSetData;			//	B电机设定速度			RW
	float			A_SpeedRealData;		//	A电机实际速度			R
	float			B_SpeedRealData;		//	B电机实际速度			R
}Register_Typedef;

void subscriber_protocol(void);
void read_protocol(void);
void write_status(uint16_t addr,uint8_t data);
void write_status_f(uint16_t addr, float data);
void write_status_two(uint16_t addr1,uint8_t data1,uint16_t addr2,float data2);

#endif 

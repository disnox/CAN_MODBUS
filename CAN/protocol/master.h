#ifndef __MASTER_H
#define	__MASTER_H
#include "xsto_api_ii.h"
#include "alloc.h"
#include "protocol_api.h"
#include "protocol.h"
#include "protocol_register_management.h"


typedef struct
{
	uint32_t	SoftwareVersion; 		//	�̼���						  R
	uint32_t	Pcbversion;					//	Ӳ����						  R
	uint8_t		A_MotorStatus;			//	A���״̬				  R
	uint8_t		B_MotorStatus;			//	B���״̬				  R
	uint8_t		A_MotorEnableStatus;//	A���ʹ��				  RW
	uint8_t		B_MotorEnableStatus;//	B���ʹ��					RW
	uint8_t 	A_MotorMode;				//	A���ģʽ 				  RW
	uint8_t		B_MotorMode;				//	B���ģʽ					RW		
	float			A_SpeedSetData;			//	A����趨�ٶ�			RW
	float			B_SpeedSetData;			//	B����趨�ٶ�			RW
	float			A_SpeedRealData;		//	A���ʵ���ٶ�			R
	float			B_SpeedRealData;		//	B���ʵ���ٶ�			R
}Register_Typedef;

void subscriber_protocol(void);
void read_protocol(void);
void write_status(uint16_t addr,uint8_t data);
void write_status_f(uint16_t addr, float data);
void write_status_two(uint16_t addr1,uint8_t data1,uint16_t addr2,float data2);

#endif 

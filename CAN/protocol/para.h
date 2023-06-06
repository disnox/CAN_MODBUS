/**********************************************************/
//@Autor:Raw
//@Version:0.0.1
//@End Time:2023/4/21
/**********************************************************/
#ifndef __PARA_H__
#define __PARA_H__
#include "stdio.h"
#include "stdint.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <stdbool.h>		//C++语法报错

//#define Master_Device 1		//手柄切换成主机
#define Slave_Device 1
#define TYPE_SLEF 0x0a
#define NODE_SELF (TYPE_SLEF<<8|0x01)


#ifdef Slave_Device
#define CANID_TX 0x500
#define CANID_RX 0x580
#else 
#define CANID_TX 0x580
#define CANID_RX 0x500
#endif

//////////////////////////////////////
/*115寄存器表
地址        类型            变量名       		描述         	      操作方式
0x0000			u32				Firmware_version			固件号						  	R
0x0004			u32				Hardware_version			硬件号						 		R
0x0008			u16				Motor1_state					电机状态				  	  R
0x000A			u8				Motor1_enable					电机使能				      RW
0x000B			u8				Motor1_mode						电机模式				      RW
0x000C			float			Motor1_setspeed				电机设定速度				  RW
0x0010   		float			Motor1_realspeed			电机真实速度				  R
...................
*/

////////////////////////////////////////


/*初始化

API_Config_t  API_Config;
API_Message_t API_Message;
uint8_t msg_buff[128]={0};//声明数组

void map_init(void)//注册寄存器表
	register_map(0x00, (uint8_t *)&firmwareV, 4);//5
	register_map(0x04, (uint8_t *)&hardwreV, 4);//5
{
	...
}

void protocol_init(void)
{
	
	map_init();
	xa_mem_init();

	API_Config.Status = MODULE_STATUS_OFFLINE;
	
	api_init(&API_Config);
	API_Message.msg_data.Data = msg_buff;
}
*/


///////////////////////////////////////////
/*订阅

void test_protocol(void)
{
	PDU_t req_PDU;
	MsgData_t request_msg_data;
	
	req_PDU.funC = FUNC_SUBSCRIBE;
	req_PDU.msgID = 0;
	req_PDU.length = 3+1*4;
	//uint16_t timing 
	req_PDU.data[0] = 0x64;
	req_PDU.data[1] = 0x00;
	//uint8_t combineCount
	req_PDU.data[2] = 0x02;
	//uint16_t address;	uint16_t size;
	req_PDU.data[3] = 0x08;
	req_PDU.data[4] = 0x00;
	req_PDU.data[5] = 0x0C;
	req_PDU.data[6] = 0x00;
	
	req_PDU.data[5] = 0x10;
	req_PDU.data[6] = 0x00;
	req_PDU.data[5] = 0x04;
	req_PDU.data[6] = 0x00;
	
	PDU_to_msg_data(NODE_SELF, &req_PDU, &request_msg_data);
	send_message_data(&request_msg_data);

}

*/

////////////////////////////////////////////
/*写操作

void test_protocol(void)
{
	PDU_t req_PDU;
	MsgData_t request_msg_data;	
	combine_write_t combine_write;
	uint8_t buf = 0x01;
	float buf2 = 1000.0f;
	combine_write.combineCount = 2;	//写以下两个组合
	combine_write.combine_data[0].combine.address = 0x000a;
	combine_write.combine_data[0].combine.size = 1;
	combine_write.combine_data[0].data = (uint8_t*)&buf;
	combine_write.combine_data[1].combine.address = 0x000c;
	combine_write.combine_data[1].combine.size = 4;
	combine_write.combine_data[1].data = (uint8_t*)&buf2;
	combine_write_to_PDU(0, &combine_write, &req_PDU);
	PDU_to_msg_data(NODE_SELF, &req_PDU, &request_msg_data);	
	//PDU_to_msg_data(NODE_SELF, &req_PDU, &request_msg_data);
	send_message_data(&request_msg_data);

}
*/

////////////////////////////////////////////
/*读操作

void test_protocol(void)
{
	PDU_t req_PDU;
	MsgData_t request_msg_data;
	combine_read_t combine_read;
	combine_read.combineCount = 2;	//读以下2个组合
	combine_read.combine[0].address = 0x0000;
	combine_read.combine[0].size = 4;
	combine_read.combine[1].address = 0x0008;
	combine_read.combine[1].size = 2;
	combine_read_to_PDU(0, &combine_read, &req_PDU);
	PDU_to_msg_data(NODE_SELF, &req_PDU, &request_msg_data);
	send_message_data(&request_msg_data);

}


*/

///////////////////////////////////////////////
/*

void can_receive(void)
{	
	uint16_t len = 0;

	len = can_serial_available();
	if(len > 0 && len < CAN_BUFFER_SIZE)
	{
		can_serial_read(Car_buff,len);
		api_port_received(len, Car_buff);
	}
}

void api_port_send(uint16_t len, uint8_t * data) 
{
	can_serial_write(data,len);
}

//MasterMsgData_t rxmessage;  //主机做的回调
//void api_on_message_received(MasterMsgData_t message)
//{
//	memcpy(&rxmessage,&message,sizeof(MasterMsgData_t));
//	
//}
*/
#endif

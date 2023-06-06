#include "master.h"

API_Config_t  API_Config;
API_Message_t API_Message;
uint8_t msg_buff[128]={0};//��������
/*
	��ַ        ����            ������       				����         	������ʽ
	0x0000		uint32_t				SoftwareVersion 			�̼���						  R
	0x0004		uint32_t				Pcbversion						Ӳ����						  R
	0x0008		uint8_t					A_MotorStatus					A���״̬				  R
	0x000A		uint8_t					B_MotorStatus					B���״̬				  R
	0x000C		uint8_t					A_MotorEnableStatus		A���ʹ��				  RW
	0x000E		uint8_t			 	 	B_MotorEnableStatus		B���ʹ��					RW
	0x0010   	uint8_t 			  A_MotorMode		       	A���ģʽ 				  RW
	0x0012		uint8_t					B_MotorMode						B���ģʽ					RW		
	0x0016		float						A_SpeedSetData				A����趨�ٶ�			RW
	0x001a		float						B_SpeedSetData				B����趨�ٶ�			RW
	0x001e		float						A_SpeedRealData				A���ʵ���ٶ�			R
	0x0022		float						B_SpeedRealData				B���ʵ���ٶ�			R
	0x0026		uint8_t					abc
*/

/*A:Left  B:Right*/

#define		MOTORASTATUS	0x0008
#define		MOTORBSTATUS	0x000A
#define		MOTORAENABLE	0x000C
#define		MOTORBENABLE	0x000E
#define		MOTORAMODE		0x0010
#define		MOTORBMODE		0x0012
#define		ASPEEDSET			0x0016
#define		BSPEEDSET			0x001A
#define		ASPEEDREAL		0x001E
#define		BSPEEDREAL		0x0022

Register_Typedef CanRegister;
void map_init(void)//ע��Ĵ�����
{
	register_map(0x0000, (uint8_t *)&CanRegister.SoftwareVersion,4);
	register_map(0x0004, (uint8_t *)&CanRegister.Pcbversion, 4);
	register_map(0x0008, (uint8_t *)&CanRegister.A_MotorStatus, 1);
	register_map(0x000A, (uint8_t *)&CanRegister.B_MotorStatus, 1);
	register_map(0x000C, (uint8_t *)&CanRegister.A_MotorEnableStatus, 1);
	register_map(0x000E, (uint8_t *)&CanRegister.B_MotorEnableStatus, 1);
	register_map(0x1000, (uint8_t *)&CanRegister.A_MotorMode, 1);
	register_map(0x0012, (uint8_t *)&CanRegister.B_MotorMode, 1);
	register_map(0x0016, (uint8_t *)&CanRegister.A_SpeedSetData, 4);
	register_map(0x001A, (uint8_t *)&CanRegister.B_SpeedSetData, 4);
	register_map(0x001E, (uint8_t *)&CanRegister.A_SpeedRealData, 4);
	register_map(0x0022, (uint8_t *)&CanRegister.B_SpeedRealData, 4);
}

void protocol_init(void)
{
	map_init();
	xa_mem_init();
	API_Config.Status = MODULE_STATUS_OFFLINE;
	api_init(&API_Config);
	API_Message.msg_data.Data = msg_buff;
}

void subscriber_protocol(void)
{
	PDU_t req_PDU;
	MsgData_t request_msg_data;
	
	req_PDU.funC = FUNC_SUBSCRIBE;
	req_PDU.msgID = 0;
	req_PDU.length = 3+1*4; /*2*addr+2*sizeof*/
	req_PDU.data[0] = 0x64;
	req_PDU.data[1] = 0x00;
	//uint8_t combineCount
	req_PDU.data[2] = 0x01;
	//uint16_t address;	uint16_t size;
	req_PDU.data[3] = 0x00;
	req_PDU.data[4] = 0x00;
	req_PDU.data[5] = 0x1E;
	req_PDU.data[6] = 0x00;
	
	PDU_to_msg_data(NODE_SELF, &req_PDU, &request_msg_data);
	send_message_data(&request_msg_data);
}

// ��������
void write_status(uint16_t addr, uint8_t data)
{
	PDU_t req_PDU;
	MsgData_t request_msg_data;	
	combine_write_t combine_write;
	combine_write.combineCount = 1;	//д1�����
	combine_write.combine_data[0].combine.address = addr;
	combine_write.combine_data[0].combine.size= sizeof(data);
	combine_write.combine_data[0].data = &data;
	combine_write_to_PDU(0, &combine_write, &req_PDU);
	PDU_to_msg_data(NODE_SELF, &req_PDU, &request_msg_data);	
	send_message_data(&request_msg_data);
}
// ��������
void write_status_f(uint16_t addr, float data)
{
	PDU_t req_PDU;
	MsgData_t request_msg_data;	
	combine_write_t combine_write;
	combine_write.combineCount = 1;	//д1�����
	combine_write.combine_data[0].combine.address = addr;
	combine_write.combine_data[0].combine.size= sizeof(data);
	combine_write.combine_data[0].data = (uint8_t*)&data;
	combine_write_to_PDU(0, &combine_write, &req_PDU);
	PDU_to_msg_data(NODE_SELF, &req_PDU, &request_msg_data);	
	send_message_data(&request_msg_data);
}
// ��������
void write_status_two(uint16_t addr1, uint8_t data1, uint16_t addr2, float data2)
{
	PDU_t req_PDU;
	MsgData_t request_msg_data;	
	combine_write_t combine_write;
	combine_write.combineCount = 2;	//д�����������
	combine_write.combine_data[0].combine.address = addr1;
	combine_write.combine_data[0].combine.size= 1;
	combine_write.combine_data[0].data = &data1;	
	combine_write.combine_data[1].combine.address = addr2;
	combine_write.combine_data[1].combine.size= 4;
	combine_write.combine_data[1].data = (uint8_t*)&data2;
	combine_write_to_PDU(0, &combine_write, &req_PDU);
	PDU_to_msg_data(NODE_SELF, &req_PDU, &request_msg_data);	
	send_message_data(&request_msg_data);
}

void read_protocol(void)
{
	PDU_t req_PDU;
	MsgData_t request_msg_data;
	combine_read_t combine_read;
	combine_read.combineCount = 2;	//������2�����
	combine_read.combine[0].address = 0x0000;
	combine_read.combine[0].size = 4; 
	combine_read.combine[1].address = 0x0008;
	combine_read.combine[1].size = 2;
	combine_read_to_PDU(0, &combine_read, &req_PDU);
	PDU_to_msg_data(NODE_SELF, &req_PDU, &request_msg_data);
	send_message_data(&request_msg_data);
}


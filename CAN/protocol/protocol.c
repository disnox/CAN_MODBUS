#include "protocol.h"
#include "crc.h"
#include "can.h"
	
/**
 *@brief:      	protocol_received
 *@details:    	放在 api_on_message_received 接收函数中处理
 *@param[in]   	API_Message_t *message
				uint8_t is_device =1当前消息是对本设备进行数据寄存器操作
 *@param[out]  	void
 *@retval:     	void
 */

void protocol_received(API_Message_t *message, uint8_t is_device,MasterMsgData_t *req_Msg)
{
	if(is_device == 1)
	{
		//数据寄存器处理
		MsgData_t respond_msg_data;
		protocol_respond(&message->msg_data, &respond_msg_data);
		//回复
		API_Message_t msg;
		message_init(&msg, respond_msg_data.TYPE,respond_msg_data.Length,respond_msg_data.cmd, respond_msg_data.Data);
		api_message_send(&msg);
	}
	else if(is_device == 0)
	{
		req_Msg->TYPE = message->msg_data.TYPE;
		req_Msg->cmd= message->msg_data.cmd;
		req_Msg->msg_id = message->msg_data.Data[0];
		memcpy(&req_Msg->Length,&message->msg_data.Data[1],2);
		req_Msg->Data = &message->msg_data.Data[3];		
	}
//	for(uint8_t i=0;i<128;i++)
//	{
//		tCan.DataBuff[i] = 0;
//	}
//	tCan.DataSize = 0;
}

/**
 *@brief:      	message_init
 *@details:    	API_Message_t 消息结构体填充
 *@param[in]   	uint32_t From_Port, uint32_t To_Port, uint32_t To, uint8_t Request_Id, uint8_t Respond_Id, 
				uint8_t cmd, uint16_t Length, uint8_t * Data
 *@param[out]  	API_Message_t *message
 *@retval:     	void
 */
void message_init(API_Message_t *message, uint16_t TYPE, uint16_t Length,uint8_t cmd, uint8_t * Data)
{
	message->msg_data.cmd =  cmd;
	message->msg_data.TYPE = TYPE;
	message->msg_data.Length = Length;
	message->msg_data.Data = Data;
}

/**
 *@brief:      	send_message_data
 *@details:    	填充MsgData_t *respond_msg_data到API_Message_t msg发送
 *@param[in]   	MsgData_t *respond_msg_data
 *@param[out]  	void
 *@retval:     	void
 */
void send_message_data(MsgData_t *respond_msg_data)
{
	API_Message_t msg;
	uint16_t TYPE = respond_msg_data->TYPE;
	message_init(&msg, NODE_SELF,respond_msg_data->Length,respond_msg_data->cmd, respond_msg_data->Data);
	api_message_send(&msg);
}

/**
 *@brief:      	list_mem
 *@details:    	内存使用情况打印
 *@param[in]   	void
 *@param[out]  	void
 *@retval:     	void
 */
void list_mem(void)
{
	rt_uint32_t total, used, max_used;
	xa_mem_info(&total, &used, &max_used);
	xa_printf("--------------------------------------------\r\n");
    xa_printf("total memory: %d\r\n", total);//总大小
    xa_printf("used memory : %d\r\n", used);//已使用大小
    xa_printf("maximum allocated memory: %d\r\n", max_used);//使用过的最大大小
	xa_printf("--------------------------------------------\r\n");
}







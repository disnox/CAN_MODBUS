#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_
#include "xsto_api_ii.h"
#include "protocol_api.h"
#include "alloc.h"
#include "klist.h"
#include "protocol_register_management.h"


//内存使用打印
#define xa_printf printf
#ifndef xa_printf
#define xa_printf(...)
#endif

/**
 *@brief:      	node_on_router
 *@details:    	判断 node 是否是router上的
 *@param[in]   	uint32_t node     
 *@param[out]  	void
 *@retval:     	uint8_t 1=是router上，0=不是
 */
uint8_t node_on_router(uint32_t node);

/**
 *@brief:      	node_on_router
 *@details:    	放在 api_on_message_received 接收函数中处理
 *@param[in]   	API_Message_t *message
				uint8_t is_device =1当前消息是对本设备进行数据寄存器操作
 *@param[out]  	void
 *@retval:     	void
 */
void protocol_received(API_Message_t *message, uint8_t is_device,MasterMsgData_t *req_Msg);

/**
 *@brief:      	message_init
 *@details:    	API_Message_t 消息结构体填充
 *@param[in]   	uint32_t From_Port, uint32_t To_Port, uint32_t To, uint8_t Request_Id, uint8_t Respond_Id, 
				uint8_t cmd, uint16_t Length, uint8_t * Data
 *@param[out]  	API_Message_t *message
 *@retval:     	void
 */
void message_init(API_Message_t *message, uint16_t TYPE, uint16_t Length,uint8_t cmd, uint8_t * Data);
/**
 *@brief:      	send_message_data
 *@details:    	填充MsgData_t *respond_msg_data到API_Message_t msg发送
 *@param[in]   	MsgData_t *respond_msg_data
 *@param[out]  	void
 *@retval:     	void
 */
void send_message_data(MsgData_t *respond_msg_data);

/**
 *@brief:      	list_mem
 *@details:    	内存使用情况打印
 *@param[in]   	void
 *@param[out]  	void
 *@retval:     	void
 */
void list_mem(void);
void map_init(void);
void protocol_init(void);
#endif




/**
 * @file            protocol_api.c
 * @brief           新协议封帧解帧 api
 * @author          
 * @date            
 * @version         
 * @par             
 * @par History:
*/
#include "protocol_api.h"
#include "protocol_register_management.h"
#include <string.h>
#include "alloc.h"
#include "klist.h"
#include "protocol_api.h"
#include <stdlib.h>

//#define PROTOCOL_API_TEST

#ifdef PROTOCOL_API_TEST
#include "log.h"

#define PROTOCOL_API_DEBUG

#ifdef PROTOCOL_API_DEBUG
#define PROTOCOL_DEBUG	xa_log
#else
#define PROTOCOL_DEBUG(a, ...)
#endif
#endif

/**
 *@brief:      	PDU_to_msg_data
 *@details:    	将PDU_t PDU的内容填充到 MsgData_t 里
 *@param[in]   	uint32_t to_or_from, PDU_t *PDU
 *@param[out]  	MsgData_t *msg_data
 *@retval:     	void
 */
void PDU_to_msg_data(uint16_t TYPE, PDU_t *PDU, MsgData_t *msg_data)
{
	msg_data->TYPE = TYPE;
	msg_data->cmd = PDU->funC;
	msg_data->Length = PDU->length + 6 +2+1;
	msg_data->Data = (uint8_t*)(&PDU->msgID);
}

/**
 *@brief:      	PDU_err_to_msg_data
 *@details:    	将PDU_err_t 的内容填充到 MsgData_t 里
 *@param[in]   	uint32_t to_or_from, PDU_err_t *PDU_err
 *@param[out]  	MsgData_t *msg_data
 *@retval:     	void
 */
void PDU_err_to_msg_data(uint16_t TYPE, PDU_err_t *PDU_err, MsgData_t *msg_data)
{
	msg_data->TYPE = TYPE;
	msg_data->cmd = PDU_err->errC;
	msg_data->Length = PDU_err->length + 6;
	msg_data->Data = (uint8_t*)&PDU_err->msgID;
}

/**
 *@brief:      	combine_read_to_PDU
 *@details:    	填充读数据组合到 PDU 用于发送
 *@param[in]   	uint8_t msgID, combine_read_t *combine_read
 *@param[out]  	PDU_t *PDU
 *@retval:     	void
 */
void combine_read_to_PDU(uint8_t msgID, combine_read_t *combine_read, PDU_t *PDU)
{
	PDU->funC = FUNC_READ;
	PDU->msgID = msgID;
	PDU->length = 1 + sizeof(combine_t)*combine_read->combineCount;
	if(PDU->length > DATA_MXA) PDU->length = 0; //防止数组越界
	memcpy(PDU->data, combine_read, PDU->length);
}

/**
 *@brief:      	combine_write_to_PDU
 *@details:    	填充写数据组合到 PDU 用于发送
 *@param[in]   	uint8_t msgID, combine_write_t *combine_write
 *@param[out]  	PDU_t *PDU
 *@retval:     	void
 */
void combine_write_to_PDU(uint8_t msgID, combine_write_t *combine_write, PDU_t *PDU)
{
	PDU->funC = FUNC_WRITE;
	PDU->msgID = msgID;
	PDU->length = 1;
	PDU->data[0] = combine_write->combineCount;
	for(uint8_t i=0; i<combine_write->combineCount; i++)
	{
		//防止数组越界
		if(PDU->length+4+combine_write->combine_data[i].combine.size > DATA_MXA) {PDU->length = 0; break;}
		memcpy(PDU->data + PDU->length, &combine_write->combine_data[i].combine, 4);
		PDU->length += 4;
		memcpy(PDU->data + PDU->length, combine_write->combine_data[i].data, combine_write->combine_data[i].combine.size);
		PDU->length += combine_write->combine_data[i].combine.size;
	}
}

/**
 *@brief:      	msg_data_to_PDU
 *@details:    	将MsgData_t.Data的内容填充到 PDU_t 里
				当 MsgData_t.cmd == 0xFF && MsgData_t.Data[0] < 0x80
 *@param[in]   	uint8_t *data, uint16_t length
 *@param[out]  	PDU_t *PDU
 *@retval:     	void
 */
void msg_data_to_PDU(uint8_t *data, uint16_t length, PDU_t *PDU)
{
	if(length > 512 || length < 5) return;
	memcpy(&(PDU->msgID), data, length);
}

/**
 *@brief:      	msg_data_to_PDU_err
 *@details:    	将MsgData_t.Data的内容填充到 PDU_err_t 里
				当 MsgData_t.cmd == 0xFF && MsgData_t.Data[0] >= 0x80
 *@param[in]   	uint8_t *data, uint16_t length == 5
 *@param[out]  	PDU_t *PDU
 *@retval:     	void
 */
void msg_data_to_PDU_err(uint8_t *data, uint16_t length, PDU_err_t *PDU_err)
{
	if(length > 512 || length < 5) return;
	memcpy(&(PDU_err->errC), data, length);
}

/**
 *@brief:      	PDU_to_combine_read
 *@details:    	将 PDU_t.data的内容填充到 combine_read_t 里
				当 PDU_t.funC == FUNC_READ
 *@param[in]   	PDU_t *PDU
 *@param[out]  	combine_read_t *combine_read
 *@retval:     	void
 */

void PDU_to_combine_read(PDU_t *PDU, combine_read_t *combine_read)
{
	if(PDU->length > sizeof(combine_read_t))
		return;
	memcpy(combine_read, PDU->data, PDU->length);
}

/**
 *@brief:      	combine_subscribe_to_PDU
 *@details:    	填充订阅数据组合到 PDU 用于发送
 *@param[in]   	uint8_t msgID, combine_read_t *combine_read
 *@param[out]  	PDU_t *PDU
 *@retval:     	void
 */
void combine_subscribe_to_PDU(uint8_t msgID, combine_subscribe_t *combine_subscribe, PDU_t *PDU)
{
	PDU->funC = FUNC_SUBSCRIBE;
	PDU->msgID = msgID;
	PDU->length = 1 + 2 + sizeof(combine_t)*combine_subscribe->combine_read.combineCount;
	if(PDU->length > DATA_MXA) PDU->length = 0; //防止数组越界
	memcpy(PDU->data, combine_subscribe, PDU->length);
}
/**
 *@brief:      	PDU_to_combine_read
 *@details:    	将 PDU_t.data的内容填充到 combine_read_t 里
				当 PDU_t.funC == FUNC_READ
 *@param[in]   	PDU_t *PDU
 *@param[out]  	combine_read_t *combine_read
 *@retval:     	void
 */
void PDU_to_combine_write(PDU_t *PDU, combine_write_t *combine_write)
{
	uint16_t len = 0;
	
	combine_write->combineCount = PDU->data[0];
	len += 1;
	for(uint8_t i=0; i<combine_write->combineCount; i++)
	{
		if(i >= COMBINE_COUNT_MAX)
			break;
		memcpy(&combine_write->combine_data[i], PDU->data+len, 4);
		len += 4;
		combine_write->combine_data[i].data = PDU->data+len;
		len += combine_write->combine_data[i].combine.size;
	}
}

typedef struct{
	uint16_t address;		//地址
	uint16_t size;			//字节大小
} list_combine_t;	//组合

typedef struct {
	uint8_t msgID;
	uint8_t combineCount; 	// 组合项个数
	uint16_t timing; 	// 定时发 /ms；填 0 是以触发方式返回数据，有外部触发才返回数据
	list_combine_t *combine;
	uint8_t *timing_data;
	struct list_head subscribe_list;
}subscribe_list_node;
	
typedef struct {
	uint16_t TYPE;
	struct list_head subscribe_head;
	struct list_head fcnode_list;
}fcnode_list_node;

//第一层以订阅数据的node管理
//第二层以订阅数据的node里的订阅组合msgID管理
LIST_HEAD(fcnode_head);


//添加订阅组合，包括相同msgID替换
//返回值0=订阅地址或组合数超出范围;1=新建 fcnode_list_node ；2=新建 subscribe_list_node ；3=替换
/**
 *@brief:      	add_subscribe_combine_node
 *@details:    	添加订阅数据组合
				当 PDU_t.funC == FUNC_SUBSCRIBE
 *@param[in]   	uint32_t fcnode	订阅的node号
				PDU_t *PDU
 *@param[out]  	void
 *@retval:     	uint8_t 0=订阅地址或组合数超出范围;1=新建 fcnode_list_node ；2=新建 subscribe_list_node ；3=替换
 */
uint8_t add_subscribe_combine_node(uint16_t TYPE, PDU_t *PDU)
{
	if(PDU->funC == FUNC_SUBSCRIBE)
	{
		//第一步、查看组合是否超出范围
		combine_subscribe_t *combine_subscribe;
		combine_subscribe = (combine_subscribe_t *)PDU->data;
		if(combine_subscribe->combine_read.combineCount > COMBINE_COUNT_MAX)
			return 0;
		for(uint8_t i=0; i<combine_subscribe->combine_read.combineCount; i++)
		{
			if(combine_subscribe->combine_read.combine[i].address+combine_subscribe->combine_read.combine[i].size > RegisterSize)
				return 0;
		}
		
	
		//第二步、查找 fcnode 是否已经存在，涉及是否需要申请 fcnode_list_node 空间
		subscribe_list_node *sn;
		struct list_head *p;
		list_for_each(p, &fcnode_head)
		{
			fcnode_list_node *fn = list_entry(p, fcnode_list_node, fcnode_list);
//			PROTOCOL_DEBUG(LOG_DEBUG, "fcnode_list_node: %08x\r\n", fn->fcnode);
			if(TYPE == fn->TYPE)// fcnode_list_node 存在
			{
				//第三步、查找 msgID 是否存在，不存在则申请 subscribe_list 内存；存在则替换
				struct list_head *q;
				list_for_each(q, &fn->subscribe_head)
				{
					sn = list_entry(q, subscribe_list_node, subscribe_list);
//					PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node: %d\r\n", sn->msgID);
					if(PDU->msgID == sn->msgID)// msgID 存在 
					{
						if(sn->timing == 0)//旧的 timing == 0
						{
							if(combine_subscribe->timing != 0)
							{
								//旧 timing==0，新的 != 0，释放内存
								xa_free(sn->timing_data);
							}
							else//新的 timing == 0
							{
								//旧的 timing == 0，新的也 timing == 0，比较大小，相等则直接覆盖，不相等则释放内存重新申请
								uint16_t old_size = 0;
								uint16_t new_size = 0;
								for(uint8_t i=0; i<sn->combineCount; i++)
								{
									old_size += sn->combine[i].size;
								}
								for(uint8_t i=0; i<combine_subscribe->combine_read.combineCount; i++)
								{
									new_size += combine_subscribe->combine_read.combine[i].size;
								}
								
								if(old_size != new_size)
								{
//									xa_free(sn->timing_data);
//									sn->timing_data = xa_malloc(new_size);
									sn->timing_data = xa_realloc(sn->timing_data, new_size);
									if(sn->timing_data == RT_NULL)
									{
										return 0;
									}
								}
								uint16_t len = 0;
								for(uint8_t i=0; i<combine_subscribe->combine_read.combineCount; i++)
								{
									if(register_read(combine_subscribe->combine_read.combine[i].address, combine_subscribe->combine_read.combine[i].size, sn->timing_data+len) == -1)
									{
										//读取异常
										break;
									}
									len += combine_subscribe->combine_read.combine[i].size;
								}
							}//新的 timing == 0
						}//旧的 timing == 0
						else if(combine_subscribe->timing == 0)// 旧的 timing != 0
						{
							//if 新 timing == 0，申请内存
							uint16_t new_size = 0;
							for(uint8_t i=0; i<combine_subscribe->combine_read.combineCount; i++)
							{
								new_size += combine_subscribe->combine_read.combine[i].size;
							}
							sn->timing_data = xa_malloc(new_size);
							if(sn->timing_data == RT_NULL)
							{
								return 0;
							}
							
							uint16_t len = 0;
							for(uint8_t i=0; i<combine_subscribe->combine_read.combineCount; i++)
							{
								if(register_read(combine_subscribe->combine_read.combine[i].address, combine_subscribe->combine_read.combine[i].size, sn->timing_data+len) == -1)
								{
									//读取异常
									break;
								}
								len += combine_subscribe->combine_read.combine[i].size;
							}
						}// 旧的 timing != 0
						//替换
						if(combine_subscribe->combine_read.combineCount != sn->combineCount)//判断新组合数是否与旧组合相等
						{
							//不相等，则释放内存，重新申请
//							xa_free(sn->combine);//释放内存
							sn->combineCount = combine_subscribe->combine_read.combineCount;
//							list_combine_t *list_combine = xa_malloc(sn->combineCount*sizeof(list_combine_t));//申请新空间储存组合
//							sn->combine = list_combine;
							sn->combine = xa_realloc(sn->combine, sn->combineCount*sizeof(list_combine_t));
							if(sn->combine == RT_NULL)
							{
								return 0;
							}
						}
						//相等，则直接改时间和组合
						sn->timing = combine_subscribe->timing;
						memcpy(sn->combine, combine_subscribe->combine_read.combine, sn->combineCount*sizeof(list_combine_t));
						return 3;//替换
					}
				}
				// 新建 subscribe_list_node 
				sn = xa_malloc(sizeof(subscribe_list_node));//申请订阅节点空间
				if(sn == RT_NULL)
				{
					return 0;
				}
				sn->msgID = PDU->msgID;
				sn->timing = combine_subscribe->timing;
				sn->combineCount = combine_subscribe->combine_read.combineCount;
				
//				list_combine_t *list_combine = xa_malloc(sn->combineCount*sizeof(list_combine_t));//申请新空间储存组合
//				sn->combine = list_combine;
				sn->combine = xa_malloc(sn->combineCount*sizeof(list_combine_t));//申请新空间储存组合
				if(sn->combine == RT_NULL)
				{
					return 0;
				}
				memcpy(sn->combine, combine_subscribe->combine_read.combine, sn->combineCount*sizeof(list_combine_t));
				
				list_add_tail(&sn->subscribe_list, &fn->subscribe_head);//插入链表subscribe_head
				
				//if 新 timing == 0，申请内存
				if(combine_subscribe->timing == 0)
				{
					//if 新 timing == 0，申请内存
					uint16_t new_size = 0;
					for(uint8_t i=0; i<combine_subscribe->combine_read.combineCount; i++)
					{
						new_size += combine_subscribe->combine_read.combine[i].size;
					}
					sn->timing_data = xa_malloc(new_size);
					if(sn->timing_data == RT_NULL)
					{
						return 0;
					}
					
					uint16_t len = 0;
					for(uint8_t i=0; i<combine_subscribe->combine_read.combineCount; i++)
					{
						if(register_read(combine_subscribe->combine_read.combine[i].address, combine_subscribe->combine_read.combine[i].size, sn->timing_data+len) == -1)
						{
							//读取异常
							break;
						}
						len += combine_subscribe->combine_read.combine[i].size;
					}
				}
				
				return 2;//新建 subscribe_list_node
			}
		}
		//fcnode_list_node 不存在
		fcnode_list_node *fn = xa_malloc(sizeof(fcnode_list_node));
		if(fn == RT_NULL)
		{
			return 0;
		}
		INIT_LIST_HEAD(&fn->subscribe_head);
		fn->TYPE = TYPE;
		list_add_tail(&fn->fcnode_list, &fcnode_head);//插入链表fcnode_head
		
		// 新建 subscribe_list_node 
		sn = xa_malloc(sizeof(subscribe_list_node));//申请订阅节点空间
		if(sn == RT_NULL)
		{
			return 0;
		}
		sn->msgID = PDU->msgID;
		sn->timing = combine_subscribe->timing;
		sn->combineCount = combine_subscribe->combine_read.combineCount;
		
//		list_combine_t *list_combine = xa_malloc(sn->combineCount*sizeof(list_combine_t));//申请新空间储存组合
//		sn->combine = list_combine;
		sn->combine = xa_malloc(sn->combineCount*sizeof(list_combine_t));//申请新空间储存组合
		if(sn->combine == RT_NULL)
		{
			return 0;
		}
		memcpy(sn->combine, combine_subscribe->combine_read.combine, sn->combineCount*sizeof(list_combine_t));
		
		list_add_tail(&sn->subscribe_list, &fn->subscribe_head);//插入链表subscribe_head
		
		//if 新 timing == 0，申请内存
		if(combine_subscribe->timing == 0)
		{
			//if 新 timing == 0，申请内存
			uint16_t new_size = 0;
			for(uint8_t i=0; i<combine_subscribe->combine_read.combineCount; i++)
			{
				new_size += combine_subscribe->combine_read.combine[i].size;
			}
			sn->timing_data = xa_malloc(new_size);
			if(sn->timing_data == RT_NULL)
			{
				return 0;
			}
			
			uint16_t len = 0;
			for(uint8_t i=0; i<combine_subscribe->combine_read.combineCount; i++)
			{
				if(register_read(combine_subscribe->combine_read.combine[i].address, combine_subscribe->combine_read.combine[i].size, sn->timing_data+len) == -1)
				{
					//读取异常
					break;
				}
				len += combine_subscribe->combine_read.combine[i].size;
			}
		}
		
		return 1;//新建 fcnode_list_node

	}
	
	return 0;
}

/**
 *@brief:      	del_subscribe_combine_node
 *@details:    	删除订阅数据组合
				当 PDU_t.funC == FUNC_UNSUBSCRIBE
 *@param[in]   	uint32_t fcnode	取消订阅的node号
				PDU_t *PDU
 *@param[out]  	void
 *@retval:     	uint8_t 取消的个数
 */
uint8_t del_subscribe_combine_node(uint16_t TYPE, PDU_t *PDU)
{
	uint8_t count = 0;
	if(PDU->funC == FUNC_UNSUBSCRIBE)
	{
		//第一步、找到订阅fcnode
		struct list_head *p;
		list_for_each(p, &fcnode_head)
		{
			fcnode_list_node *fn = list_entry(p, fcnode_list_node, fcnode_list);
//			PROTOCOL_DEBUG(LOG_DEBUG, "fcnode_list_node: %08x\r\n", fn->fcnode);
			if(TYPE == fn->TYPE)// fcnode_list_node 存在
			{
				//第二步、循环删除订阅组合
				struct list_head *q;
				for(uint8_t i=0; i<PDU->length; i++)
				{
					list_for_each(q, &fn->subscribe_head)
					{
						subscribe_list_node *sn = list_entry(q, subscribe_list_node, subscribe_list);
//						PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node: %d\r\n", sn->msgID);
						if(PDU->data[i] == sn->msgID)
						{
							++count;
							//找到就先在链表中删除再释放空间
							list_del(q);
							xa_free(sn->combine);
							if(sn->timing == 0)
							{
								xa_free(sn->timing_data);
							}
							xa_free(sn);
							break; //list_for_each(q, &fn->subscribe_head)
						}
					}
					
				}
				//判断删除的订阅fcnode是否为空，为空则本身也删除，释放空间
				if(list_empty(&fn->subscribe_head) == 1)
				{
					//为空
					list_del(p);
					xa_free(fn);
				}
				break;//list_for_each(p, &fcnode_head)
			}
		}
	}
	return count;
}

//获取订阅列表
/**
 *@brief:      	get_subscribe_combine_node
 *@details:    	获取订阅数据组合列表
 *@param[in]   	uint32_t fcnode	订阅的node号
				uint8_t msgID
 *@param[out]  	PDU_t *res_PDU	响应PDU
 *@retval:     	void
 */
void get_subscribe_combine_node(uint16_t TYPE, uint8_t msgID, PDU_t *res_PDU)
{
	res_PDU->funC = FUNC_GETSUBSCRIBE;
	res_PDU->msgID = msgID;
	res_PDU->length = 1;
	//listCount
	res_PDU->data[0] = 0;
	
	//第一步、找到订阅fcnode
	struct list_head *p;
	list_for_each(p, &fcnode_head)
	{
		fcnode_list_node *fn = list_entry(p, fcnode_list_node, fcnode_list);
		struct list_head *q;
		if(TYPE == fn->TYPE)// fcnode_list_node 存在
		{
			uint16_t index = 1;
			//第二步、循环添加订阅组合到 res_PDU 
			list_for_each(q, &fn->subscribe_head)
			{
				subscribe_list_node *sn = list_entry(q, subscribe_list_node, subscribe_list);
				//listCount
				++res_PDU->data[0];
				//ID
				res_PDU->data[index] = sn->msgID;
				++index;
				//timing
				uint16_t *timing = (uint16_t*)&res_PDU->data[index];
				*timing = sn->timing;
				index += 2;
				//combineCount
				res_PDU->data[index] = sn->combineCount;
				++index;
				//combine
				memcpy(&res_PDU->data[index], sn->combine, 4*sn->combineCount);
				index += 4*sn->combineCount;
			}
			res_PDU->length = index;
			break;//list_for_each(p, &fcnode_head)
		}
	}
	//没找到 fcnode	
}




//直接将接收的msgdata变为回复的msgdata
/**
 *@brief:      	protocol_respond
 *@details:    	cmd == 0xFF 时
				直接将接收的 MsgData_t 变为回复的 MsgData_t
 *@param[in]   	MsgData_t *request_msg_data	请求消息
 *@param[out]  	MsgData_t *respond_msg_data	响应消息
 *@retval:     	void
 */
bool write_Data_flag=false;
void protocol_respond(MsgData_t *request_msg_data, MsgData_t *respond_msg_data)
{
	static PDU_t res_PDU;
	PDU_t req_PDU;
	combine_read_t combine_read;
	combine_write_t combine_write;
	switch(request_msg_data->cmd)
	{ 
	case FUNC_READ:	//功能码为读
		res_PDU.funC = FUNC_READ;
		res_PDU.length = 0;
		msg_data_to_PDU(request_msg_data->Data, request_msg_data->Length-6, &req_PDU);
		PDU_to_combine_read(&req_PDU, &combine_read);
		for(uint8_t i=0; i<combine_read.combineCount; i++)
		{
			if(register_read(combine_read.combine[i].address, combine_read.combine[i].size, res_PDU.data+res_PDU.length) == -1)
			{
				//读取异常
				res_PDU.funC = ERRC_READ;
				res_PDU.length = 1;
				res_PDU.data[0] = EXCC_ADDRESS;
				break;
			}
			res_PDU.length += combine_read.combine[i].size;
		}
		res_PDU.msgID = req_PDU.msgID;
		PDU_to_msg_data(request_msg_data->TYPE, &res_PDU, respond_msg_data);
		break;
	case FUNC_WRITE:	//功能码为写
		res_PDU.funC = FUNC_WRITE;
		res_PDU.length = 2;
		*(uint16_t *)res_PDU.data = 0;
		msg_data_to_PDU(request_msg_data->Data, request_msg_data->Length - 6, &req_PDU);
		PDU_to_combine_write(&req_PDU, &combine_write);
		for(uint8_t i=0; i<combine_write.combineCount; i++)
		{
			if(register_write(combine_write.combine_data[i].combine.address, combine_write.combine_data[i].data, combine_write.combine_data[i].combine.size) == -1)
			{
				//写入异常
				res_PDU.funC = ERRC_WRITE;
				res_PDU.length = 1;
				res_PDU.data[0] = EXCC_ADDRESS;
				break;
			}
			if(res_PDU.funC != ERRC_WRITE)
			{
				write_Data_flag=true;
			}
			*(uint16_t *)res_PDU.data += combine_write.combine_data[i].combine.size;
		}
		res_PDU.msgID = req_PDU.msgID;
		PDU_to_msg_data(request_msg_data->TYPE, &res_PDU, respond_msg_data);
		break;
	case FUNC_SUBSCRIBE:	//订阅
		req_PDU.funC = FUNC_SUBSCRIBE;
		msg_data_to_PDU(request_msg_data->Data, request_msg_data->Length- 6, &req_PDU);
		if(add_subscribe_combine_node(request_msg_data->TYPE, &req_PDU) == 0)
		{
			//订阅异常
			res_PDU.funC = ERRC_SUBSCRIBE;
			res_PDU.length = 1;
			res_PDU.data[0] = EXCC_ADDRESS;
		}
		else
		{
			//订阅成功，返回一次订阅数据
			res_PDU.funC = FUNC_SUBSCRIBE;
			res_PDU.length = 0;
			combine_subscribe_t *combine_subscribe;
			combine_subscribe = (combine_subscribe_t *)req_PDU.data;
			for(uint8_t i=0; i<combine_subscribe->combine_read.combineCount; i++)
			{
				if(register_read(combine_subscribe->combine_read.combine[i].address, combine_subscribe->combine_read.combine[i].size, res_PDU.data+res_PDU.length) == -1)
				{//上面订阅数据检查了一遍，按道理是进不来这里的
					//读取异常
					res_PDU.funC = ERRC_SUBSCRIBE;
					res_PDU.length = 1;
					res_PDU.data[0] = EXCC_ADDRESS;
					break;
				}
				res_PDU.length += combine_subscribe->combine_read.combine[i].size;
			}
		}
		res_PDU.msgID = req_PDU.msgID;
		PDU_to_msg_data(request_msg_data->TYPE, &res_PDU, respond_msg_data);
		break;
	case FUNC_UNSUBSCRIBE:	//取消订阅
		//uint8_t del_subscribe_combine_node(uint32_t fcnode, PDU_t *PDU)
		msg_data_to_PDU(request_msg_data->Data, request_msg_data->Length - 6, &req_PDU);
		res_PDU.funC = FUNC_UNSUBSCRIBE;
		res_PDU.length = 1;
		res_PDU.data[0] = del_subscribe_combine_node(request_msg_data->TYPE, &req_PDU);
		res_PDU.msgID = req_PDU.msgID;
		PDU_to_msg_data(request_msg_data->TYPE, &res_PDU, respond_msg_data);
		break;
	case FUNC_GETSUBSCRIBE:	//获取订阅数据列表
		//get_subscribe_combine_node(uint32_t fcnode, uint8_t msgID, PDU_t *res_PDU)
		get_subscribe_combine_node(request_msg_data->TYPE, request_msg_data->Data[1], &res_PDU);
		PDU_to_msg_data(request_msg_data->TYPE, &res_PDU, respond_msg_data);
		break;
	default:
		//功能码异常
		res_PDU.funC = 0x80 + request_msg_data->Data[0];
		res_PDU.length = 1;
		res_PDU.data[0] = EXCC_NO_FUNC;
		res_PDU.msgID = req_PDU.msgID;
		PDU_to_msg_data(request_msg_data->TYPE, &res_PDU, respond_msg_data);
		break;
	}
}


//typedef void (*fun_send_message)(API_Message_t *msg);
extern void send_message_data(MsgData_t *respond_msg_data);

//定时循环发送函数，定时10ms调用，
/**
 *@brief:      	protocol_subscribe_loop
 *@details:    	使用了订阅功能10ms定时调用一次这个函数
 *@param[in]   	void
 *@param[out]  	void
 *@retval:     	void
 */
void protocol_subscribe_loop(void)
{
	static uint32_t subscribe_timer = 0;
	++subscribe_timer;
	
	MsgData_t respond_msg_data;
	PDU_t res_PDU;
	
	struct list_head *p;
	list_for_each(p, &fcnode_head)
	{
		fcnode_list_node *n = list_entry(p, fcnode_list_node, fcnode_list);
		struct list_head *q;
		list_for_each(q, &n->subscribe_head)
		{
			subscribe_list_node *sn = list_entry(q, subscribe_list_node, subscribe_list);
			if(sn->timing == 0)//触发方式
			{
				res_PDU.funC = FUNC_SUBSCRIBE;
				res_PDU.length = 0;
				for(uint8_t i=0; i<sn->combineCount; i++)
				{
					if(register_read(sn->combine[i].address, sn->combine[i].size, res_PDU.data+res_PDU.length) == -1)
					{
						//读取异常
						break;
					}
					res_PDU.length += sn->combine[i].size;
				}
				if(memcmp(sn->timing_data, res_PDU.data, res_PDU.length) != 0)
				{
					//内容改变，发送数据
					res_PDU.msgID = sn->msgID;
					PDU_to_msg_data(n->TYPE, &res_PDU, &respond_msg_data);
					send_message_data(&respond_msg_data);
//					PROTOCOL_DEBUG(LOG_DEBUG, "protocol_subscribe_loop :timing == 0 \r\n");
//					PrintFormat(respond_msg_data.Data, respond_msg_data.Length);
					memcpy(sn->timing_data, res_PDU.data, res_PDU.length);
				}
				else
				{
					res_PDU.length = 0;
				}
			}
			else if(subscribe_timer%((sn->timing+9)/10) == 0)
			{
				//时间到，取数据，发送数据
				res_PDU.funC = FUNC_SUBSCRIBE;
				res_PDU.length = 0;
				for(uint8_t i=0; i<sn->combineCount; i++)
				{
					if(register_read(sn->combine[i].address, sn->combine[i].size, res_PDU.data+res_PDU.length) == -1)
					{//上面订阅数据检查了一遍，按道理是进不来这里的
						//读取异常
						res_PDU.funC = ERRC_SUBSCRIBE;
						res_PDU.length = 1;
						res_PDU.data[0] = EXCC_ADDRESS;
						break;
					}
					res_PDU.length += sn->combine[i].size;
				}
				res_PDU.msgID = sn->msgID;
				PDU_to_msg_data(n->TYPE, &res_PDU, &respond_msg_data);
				send_message_data(&respond_msg_data);
//				PROTOCOL_DEBUG(LOG_DEBUG, "protocol_subscribe_loop : \r\n");
//				PrintFormat(respond_msg_data.Data, respond_msg_data.Length);
			}
		}
	}
}

#ifdef PROTOCOL_API_TEST

static uint8_t map[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
void protocol_respond_test(void)
{
	PDU_t req_PDU;
	MsgData_t request_msg_data;
	MsgData_t respond_msg_data;
	
	//寄存器地址映射
	register_map(0x00, map, sizeof(map));
	
	//模拟请求读数据
	combine_read_t combine_read;
	combine_read.combineCount = 2;	//读以下2个组合
	combine_read.combine[0].address = 0x0000;
	combine_read.combine[0].size = 2;
	combine_read.combine[1].address = 0x0006;
	combine_read.combine[1].size = 2;
	combine_read_to_PDU(1, &combine_read, &req_PDU);
	PDU_to_msg_data(0x00010001, &req_PDU, &request_msg_data);
	
	//响应
	protocol_respond(&request_msg_data, &respond_msg_data);
	PROTOCOL_DEBUG(LOG_DEBUG, "read: \r\n");
	PROTOCOL_DEBUG(LOG_DEBUG, ".cmd: %02x\r\n", respond_msg_data.cmd);
	PROTOCOL_DEBUG(LOG_DEBUG, ".to_or_from: %08x\r\n", respond_msg_data.to_or_from);
	PROTOCOL_DEBUG(LOG_DEBUG, ".Length: %04x\r\n", respond_msg_data.Length);
	PrintFormat(respond_msg_data.Data, respond_msg_data.Length);
	
	//模拟请求写
	combine_write_t combine_write;
	uint16_t buf = 0x4444;
	uint16_t buf2 = 0x8888;
	combine_write.combineCount = 2;	//写以下两个组合
	combine_write.combine_data[0].combine.address = 0x0004;
	combine_write.combine_data[0].combine.size = 2;
	combine_write.combine_data[0].data = (uint8_t*)&buf;
	combine_write.combine_data[1].combine.address = 0x0008;
	combine_write.combine_data[1].combine.size = 2;
	combine_write.combine_data[1].data = (uint8_t*)&buf2;
	combine_write_to_PDU(1, &combine_write, &req_PDU);
	PDU_to_msg_data(0x00010001, &req_PDU, &request_msg_data);
	
	//响应
	protocol_respond(&request_msg_data, &respond_msg_data);
	PROTOCOL_DEBUG(LOG_DEBUG, "write: \r\n");
	PROTOCOL_DEBUG(LOG_DEBUG, ".cmd: %02x\r\n", respond_msg_data.cmd);
	PROTOCOL_DEBUG(LOG_DEBUG, ".to_or_from: %08x\r\n", respond_msg_data.to_or_from);
	PROTOCOL_DEBUG(LOG_DEBUG, ".Length: %04x\r\n", respond_msg_data.Length);
	PrintFormat(respond_msg_data.Data, respond_msg_data.Length);
	
	PROTOCOL_DEBUG(LOG_DEBUG, "map: \r\n");
	PrintFormat(map, sizeof(map));
	
	//模拟订阅
	req_PDU.funC = FUNC_SUBSCRIBE;
	req_PDU.msgID = 1;
	req_PDU.length = 3+2*4;
	//uint16_t timing 
	req_PDU.data[0] = 0x00;
	req_PDU.data[1] = 0x00;
	//uint8_t combineCount
	req_PDU.data[2] = 0x02;
	//uint16_t address;	uint16_t size;
	req_PDU.data[3] = 0x00;
	req_PDU.data[4] = 0x00;
	req_PDU.data[5] = 0x02;
	req_PDU.data[6] = 0x00;
	
	req_PDU.data[7] = 0x0A;
	req_PDU.data[8] = 0x00;
	req_PDU.data[9] = 0x02;
	req_PDU.data[10] = 0x00;
	PDU_to_msg_data(0x00010001, &req_PDU, &request_msg_data);
	
	//响应
	protocol_respond(&request_msg_data, &respond_msg_data);
	PROTOCOL_DEBUG(LOG_DEBUG, "subscribe: \r\n");
	PROTOCOL_DEBUG(LOG_DEBUG, ".cmd: %02x\r\n", respond_msg_data.cmd);
	PROTOCOL_DEBUG(LOG_DEBUG, ".to_or_from: %08x\r\n", respond_msg_data.to_or_from);
	PROTOCOL_DEBUG(LOG_DEBUG, ".Length: %04x\r\n", respond_msg_data.Length);
	PrintFormat(respond_msg_data.Data, respond_msg_data.Length);
	
	struct list_head *p;
	list_for_each(p, &fcnode_head)
	{
		fcnode_list_node *n = list_entry(p, fcnode_list_node, fcnode_list);
		PROTOCOL_DEBUG(LOG_DEBUG, "fcnode_list_node: %08x\r\n", n->fcnode);
		struct list_head *q;
		list_for_each(q, &n->subscribe_head)
		{
			subscribe_list_node *sn = list_entry(q, subscribe_list_node, subscribe_list);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node msgID: %d\r\n", sn->msgID);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node timing: %d\r\n", sn->timing);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node combineCount: %d\r\n", sn->combineCount);
			PrintFormatU16((uint16_t*)sn->combine, 2*sn->combineCount);
			if(sn->timing == 0)
			{
				uint16_t size = 0;
				for(uint8_t i=0; i<sn->combineCount; i++)
				{
					size += sn->combine[i].size;
				}
				PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node timing_data size: %d\r\n", size);
				PrintFormat(sn->timing_data, size);
			}
		}
	}
	
	//模拟获取订阅列表
	req_PDU.funC = FUNC_GETSUBSCRIBE;
	req_PDU.msgID = 1;
	req_PDU.length = 0;
	PDU_to_msg_data(0x00010001, &req_PDU, &request_msg_data);
	
	//响应
	protocol_respond(&request_msg_data, &respond_msg_data);
	PROTOCOL_DEBUG(LOG_DEBUG, "get subscribe list: \r\n");
	PROTOCOL_DEBUG(LOG_DEBUG, ".cmd: %02x\r\n", respond_msg_data.cmd);
	PROTOCOL_DEBUG(LOG_DEBUG, ".to_or_from: %08x\r\n", respond_msg_data.to_or_from);
	PROTOCOL_DEBUG(LOG_DEBUG, ".Length: %04x\r\n", respond_msg_data.Length);
	PrintFormat(respond_msg_data.Data, respond_msg_data.Length);
	
	
	//模拟取消订阅
	req_PDU.funC = FUNC_UNSUBSCRIBE;
	req_PDU.msgID = 1;
	req_PDU.length = 1;
	//msgID[]
	req_PDU.data[0] = 0x01;
	PDU_to_msg_data(0x00010001, &req_PDU, &request_msg_data);
	
	//响应
	protocol_respond(&request_msg_data, &respond_msg_data);
	PROTOCOL_DEBUG(LOG_DEBUG, "un subscribe: \r\n");
	PROTOCOL_DEBUG(LOG_DEBUG, ".cmd: %02x\r\n", respond_msg_data.cmd);
	PROTOCOL_DEBUG(LOG_DEBUG, ".to_or_from: %08x\r\n", respond_msg_data.to_or_from);
	PROTOCOL_DEBUG(LOG_DEBUG, ".Length: %04x\r\n", respond_msg_data.Length);
	PrintFormat(respond_msg_data.Data, respond_msg_data.Length);
	
//	struct list_head *p;
	list_for_each(p, &fcnode_head)
	{
		fcnode_list_node *n = list_entry(p, fcnode_list_node, fcnode_list);
		PROTOCOL_DEBUG(LOG_DEBUG, "fcnode_list_node: %08x\r\n", n->fcnode);
		struct list_head *q;
		list_for_each(q, &n->subscribe_head)
		{
			subscribe_list_node *sn = list_entry(q, subscribe_list_node, subscribe_list);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node msgID: %d\r\n", sn->msgID);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node timing: %d\r\n", sn->timing);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node combineCount: %d\r\n", sn->combineCount);
			PrintFormatU16((uint16_t*)sn->combine, 2*sn->combineCount);
			if(sn->timing == 0)
			{
				uint16_t size = 0;
				for(uint8_t i=0; i<sn->combineCount; i++)
				{
					size += sn->combine[i].size;
				}
				PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node timing_data size: %d\r\n", size);
				PrintFormat(sn->timing_data, size);
			}
		}
	}
	
	
}

void add_subscribe_test(void)
{
	//寄存器地址映射
	register_map(0x00, map, sizeof(map));
	
	PDU_t PDU;
	PDU.funC = FUNC_SUBSCRIBE;
	PDU.msgID = 1;
	PDU.length = 3+2*4;
	//uint16_t timing 
	PDU.data[0] = 0x00;
	PDU.data[1] = 0x00;
	//uint8_t combineCount
	PDU.data[2] = 0x02;
	//uint16_t address;	uint16_t size;
	PDU.data[3] = 0x00;
	PDU.data[4] = 0x00;
	PDU.data[5] = 0x02;
	PDU.data[6] = 0x00;
	
	PDU.data[7] = 0x08;
	PDU.data[8] = 0x00;
	PDU.data[9] = 0x02;
	PDU.data[10] = 0x00;
	
	//新建 fcnode_list_node return 1
	uint8_t res = add_subscribe_combine_node(0x00010001, &PDU);
	PROTOCOL_DEBUG(LOG_DEBUG, "add_subscribe_combine_node: %d\r\n", res);
	
	struct list_head *p;
	list_for_each(p, &fcnode_head)
	{
		fcnode_list_node *n = list_entry(p, fcnode_list_node, fcnode_list);
		PROTOCOL_DEBUG(LOG_DEBUG, "fcnode_list_node: %08x\r\n", n->fcnode);
		struct list_head *q;
		list_for_each(q, &n->subscribe_head)
		{
			subscribe_list_node *sn = list_entry(q, subscribe_list_node, subscribe_list);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node: %d\r\n", sn->msgID);
		}
	}
	
	//覆盖 return 3
	res = add_subscribe_combine_node(0x00010001, &PDU);
	PROTOCOL_DEBUG(LOG_DEBUG, "add_subscribe_combine_node: %d\r\n", res);
	
//	struct list_head *p;
	list_for_each(p, &fcnode_head)
	{
		fcnode_list_node *n = list_entry(p, fcnode_list_node, fcnode_list);
		PROTOCOL_DEBUG(LOG_DEBUG, "fcnode_list_node: %08x\r\n", n->fcnode);
		struct list_head *q;
		list_for_each(q, &n->subscribe_head)
		{
			subscribe_list_node *sn = list_entry(q, subscribe_list_node, subscribe_list);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node: %d\r\n", sn->msgID);
		}
	}
	
	//新建 subscribe_list_node return 2
	PDU.msgID = 2;
	//uint16_t timing 
	PDU.data[0] = 10;
	PDU.data[1] = 0x00;
	
	res = add_subscribe_combine_node(0x00010001, &PDU);
	PROTOCOL_DEBUG(LOG_DEBUG, "add_subscribe_combine_node 2: %d\r\n", res);
	
	PDU.msgID = 3;
	PDU.length = 3+1*4;
	//uint16_t timing 
	PDU.data[0] = 10;
	PDU.data[1] = 0x00;
	//uint8_t combineCount
	PDU.data[2] = 0x01;
	//uint16_t address;	uint16_t size;
	PDU.data[3] = 0x02;
	PDU.data[4] = 0x00;
	PDU.data[5] = 0x02;
	PDU.data[6] = 0x00;
	res = add_subscribe_combine_node(0x00010001, &PDU);
	PROTOCOL_DEBUG(LOG_DEBUG, "add_subscribe_combine_node 3: %d\r\n", res);
	
//	struct list_head *p;
	list_for_each(p, &fcnode_head)
	{
		fcnode_list_node *n = list_entry(p, fcnode_list_node, fcnode_list);
		PROTOCOL_DEBUG(LOG_DEBUG, "fcnode_list_node: %08x\r\n", n->fcnode);
		struct list_head *q;
		list_for_each(q, &n->subscribe_head)
		{
			subscribe_list_node *sn = list_entry(q, subscribe_list_node, subscribe_list);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node msgID: %d\r\n", sn->msgID);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node timing: %d\r\n", sn->timing);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node combineCount: %d\r\n", sn->combineCount);
			PrintFormatU16((uint16_t*)sn->combine, 2*sn->combineCount);
			if(sn->timing == 0)
			{
				uint16_t size = 0;
				for(uint8_t i=0; i<sn->combineCount; i++)
				{
					size += sn->combine[i].size;
				}
				PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node timing_data size: %d\r\n", size);
				PrintFormat(sn->timing_data, size);
			}
		}
	}
}

void get_subscribe_test(void)
{
	PDU_t res_PDU;
	get_subscribe_combine_node(0x00010001, 0x01, &res_PDU);
	PROTOCOL_DEBUG(LOG_DEBUG, "get_subscribe_combine_node : \r\n");
	PROTOCOL_DEBUG(LOG_DEBUG, ".funC : %02x\r\n", res_PDU.funC);
	PROTOCOL_DEBUG(LOG_DEBUG, ".msgID : %02x\r\n", res_PDU.msgID);
	PROTOCOL_DEBUG(LOG_DEBUG, ".length : %d\r\n", res_PDU.length);
	PROTOCOL_DEBUG(LOG_DEBUG, ".data : \r\n");
	PrintFormat(res_PDU.data, res_PDU.length);
	
	get_subscribe_combine_node(0x00010002, 0x01, &res_PDU);
	PROTOCOL_DEBUG(LOG_DEBUG, "get_subscribe_combine_node : \r\n");
	PROTOCOL_DEBUG(LOG_DEBUG, ".funC : %02x\r\n", res_PDU.funC);
	PROTOCOL_DEBUG(LOG_DEBUG, ".msgID : %02x\r\n", res_PDU.msgID);
	PROTOCOL_DEBUG(LOG_DEBUG, ".length : %d\r\n", res_PDU.length);
	PROTOCOL_DEBUG(LOG_DEBUG, ".data : \r\n");
	PrintFormat(res_PDU.data, res_PDU.length);
}


void del_subscribe_test(void)
{
	PDU_t PDU;
	PDU.funC = FUNC_UNSUBSCRIBE;
	PDU.msgID = 1;
	PDU.length = 1;
	//uint16_t timing 
	PDU.data[0] = 0x01;
	
	
	uint8_t res = del_subscribe_combine_node(0x00010001, &PDU);
	PROTOCOL_DEBUG(LOG_DEBUG, "del_subscribe_combine_node: %d\r\n", res);
	
	struct list_head *p;
	list_for_each(p, &fcnode_head)
	{
		fcnode_list_node *n = list_entry(p, fcnode_list_node, fcnode_list);
		PROTOCOL_DEBUG(LOG_DEBUG, "fcnode_list_node: %08x\r\n", n->fcnode);
		struct list_head *q;
		list_for_each(q, &n->subscribe_head)
		{
			subscribe_list_node *sn = list_entry(q, subscribe_list_node, subscribe_list);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node msgID: %d\r\n", sn->msgID);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node timing: %d\r\n", sn->timing);
			PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node combineCount: %d\r\n", sn->combineCount);
			PrintFormatU16((uint16_t*)sn->combine, 2*sn->combineCount);
		}
	}
}



void list_test(void)
{
	fcnode_list_node fcn1, fcn2, fcn3;
	fcn1.fcnode = 0x00010001;
	fcn2.fcnode = 0x00020002;
	fcn3.fcnode = 0x00030003;
	
	list_add_tail(&fcn1.fcnode_list, &fcnode_head);
	list_add_tail(&fcn2.fcnode_list, &fcnode_head);
	list_add_tail(&fcn3.fcnode_list, &fcnode_head);
	
	struct list_head *p;
	list_for_each(p, &fcnode_head)
	{
		fcnode_list_node *n = list_entry(p, fcnode_list_node, fcnode_list);
		PROTOCOL_DEBUG(LOG_DEBUG, "fcnode_list_node: %08x\r\n", n->fcnode);
	}
	
//	INIT_LIST_HEAD(struct list_head *list)
	fcnode_list_node *n = list_entry(fcnode_head.next, fcnode_list_node, fcnode_list);//获取第一个node
//	list_first_entry(ptr, type, member)
	PROTOCOL_DEBUG(LOG_DEBUG, "first fcnode_list_node: %08x\r\n", n->fcnode);
	
	INIT_LIST_HEAD(&n->subscribe_head);
	
	subscribe_list_node sn1, sn2, sn3;
	sn1.msgID = 1;
	sn2.msgID = 2;
	sn3.msgID = 3;
	list_add_tail(&sn1.subscribe_list, &n->subscribe_head);
	list_add_tail(&sn2.subscribe_list, &n->subscribe_head);
	list_add_tail(&sn3.subscribe_list, &n->subscribe_head);
	
	subscribe_list_node *sn4 = (subscribe_list_node*)malloc(sizeof(subscribe_list_node));
	sn4->msgID = 4;
	list_add_tail(&sn4->subscribe_list, &n->subscribe_head);
	
	list_for_each(p, &n->subscribe_head)
	{
		subscribe_list_node *sn = list_entry(p, subscribe_list_node, subscribe_list);
		PROTOCOL_DEBUG(LOG_DEBUG, "subscribe_list_node: %d\r\n", sn->msgID);
	}
}




void PDU_test(void)
{
	PDU_t PDU;
	
	//combine_read_t 到 PDU
	combine_read_t combine_read;
	combine_read.combineCount = 2;	//读以下2个组合
	combine_read.combine[0].address = 0x0002;
	combine_read.combine[0].size = 2;
	combine_read.combine[1].address = 0x0006;
	combine_read.combine[1].size = 2;
	combine_read_to_PDU(1, &combine_read, &PDU);
	PROTOCOL_DEBUG(LOG_DEBUG, "read PDU.data: \r\n");
	PrintFormat(PDU.data, PDU.length);
	
	//PDU到MsgData_t
	MsgData_t msg_data;
	PDU_to_msg_data(0x00010001, &PDU, &msg_data);
	PROTOCOL_DEBUG(LOG_DEBUG, "read msg_data.data: \r\n");
	PrintFormat(msg_data.Data, msg_data.Length);
	
	//MsgData_t到PDU
	PDU_t PDU2;
	msg_data_to_PDU(msg_data.Data, msg_data.Length, &PDU2);
	PROTOCOL_DEBUG(LOG_DEBUG, "MsgData_t to PDU2: \r\n");
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU2.funC:%02x \r\n", PDU2.funC);
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU2.msgID:%02x \r\n", PDU2.msgID);
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU2.length:%04x \r\n", PDU2.length);
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU2.data: \r\n");
	PrintFormat(PDU2.data, PDU2.length);
	
	//PDU_t 到 combine_read_t
	combine_read_t combine_read2;
	PDU_to_combine_read(&PDU2, &combine_read2);
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU2 to combine_read2: \r\n");
	for(uint8_t i=0; i<combine_read2.combineCount; i++)
	{
		PROTOCOL_DEBUG(LOG_DEBUG, "combine_read2.combine[%d].address:%04x \r\n", i, combine_read2.combine[i].address);
		PROTOCOL_DEBUG(LOG_DEBUG, "combine_read2.combine[%d].size:%04x \r\n", i, combine_read2.combine[i].size);
	}
	
	
	
	//combine_write_t到PDU
	combine_write_t combine_write;
	uint16_t buf = 0x0005;
	uint16_t buf2 = 0x0006;
	combine_write.combineCount = 2;	//写以下两个组合
	combine_write.combine_data[0].combine.address = 0x0004;
	combine_write.combine_data[0].combine.size = 2;
	combine_write.combine_data[0].data = (uint8_t*)&buf;
	combine_write.combine_data[1].combine.address = 0x0008;
	combine_write.combine_data[1].combine.size = 2;
	combine_write.combine_data[1].data = (uint8_t*)&buf2;
	combine_write_to_PDU(1, &combine_write, &PDU);
	PROTOCOL_DEBUG(LOG_DEBUG, "write PDU.data: \r\n");
	PrintFormat(PDU.data, PDU.length);
	
	//PDU到MsgData_t
	PDU_to_msg_data(0x00010001, &PDU, &msg_data);
	PROTOCOL_DEBUG(LOG_DEBUG, "write msg_data.data: \r\n");
	PrintFormat(msg_data.Data, msg_data.Length);
	
	//MsgData_t到PDU
	msg_data_to_PDU(msg_data.Data, msg_data.Length, &PDU2);
	PROTOCOL_DEBUG(LOG_DEBUG, "MsgData_t to PDU2: \r\n");
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU2.funC:%02x \r\n", PDU2.funC);
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU2.msgID:%02x \r\n", PDU2.msgID);
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU2.length:%04x \r\n", PDU2.length);
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU2.data: \r\n");
	PrintFormat(PDU2.data, PDU2.length);
	
	//PDU_t 到 combine_read_t
	combine_write_t combine_write2;
	PDU_to_combine_write(&PDU2, &combine_write2);
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU2 to combine_write2: \r\n");
	for(uint8_t i=0; i<combine_write2.combineCount; i++)
	{
		PROTOCOL_DEBUG(LOG_DEBUG, "combine_write2.combine_data[%d].combine.address:%04x \r\n", i, combine_write2.combine_data[i].combine.address);
		PROTOCOL_DEBUG(LOG_DEBUG, "combine_write2.combine_data[%d].combine.size:%04x \r\n", i, combine_write2.combine_data[i].combine.size);
		PrintFormat(combine_write2.combine_data[i].data, combine_write2.combine_data[i].combine.size);
	}
	
	
	//PDU_err_t to MsgData_t
	PDU_err_t PDU_err;
	PDU_err.errC = 0x83;
	PDU_err.msgID = 0x01;
	PDU_err.length = 0x0001;
	PDU_err.excC = 0x02;
	PDU_err_to_msg_data(0x00010001, &PDU_err, &msg_data);
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU_err to msg_data.Data: \r\n");
	PrintFormat(msg_data.Data, msg_data.Length);
	
	//MsgData_t to PDU_err_t
	PDU_err_t PDU_err2;
	msg_data_to_PDU_err(msg_data.Data, msg_data.Length, &PDU_err2);
	PROTOCOL_DEBUG(LOG_DEBUG, "MsgData_t to PDU_err2: \r\n");
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU_err2.errC:%02x \r\n", PDU_err2.errC);
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU_err2.msgID:%02x \r\n", PDU_err2.msgID);
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU_err2.length:%04x \r\n", PDU_err2.length);
	PROTOCOL_DEBUG(LOG_DEBUG, "PDU_err2.excC:%02x \r\n", PDU_err2.excC);
}

void write_test(void)
{
	map[0] = 0xff;
	
//	PDU_t PDU;
//	MsgData_t msg_data;
//	
//	//combine_write_t到PDU
//	combine_write_t combine_write;
//	uint16_t buf = 0x0005;
//	uint16_t buf2 = 0x0006;
//	combine_write.combineCount = 2;	//写以下两个组合
//	combine_write.combine_data[0].combine.address = 0x0004;
//	combine_write.combine_data[0].combine.size = 2;
//	combine_write.combine_data[0].data = (uint8_t*)&buf;
//	combine_write.combine_data[1].combine.address = 0x0008;
//	combine_write.combine_data[1].combine.size = 2;
//	combine_write.combine_data[1].data = (uint8_t*)&buf2;
//	combine_write_to_PDU(1, &combine_write, &PDU);
//	
//	//PDU到MsgData_t
//	PDU_to_msg_data(0x00010001, &PDU, &msg_data);
//	
//	MsgData_t respond_msg_data;
//	protocol_respond(&msg_data, &respond_msg_data);
}

#endif











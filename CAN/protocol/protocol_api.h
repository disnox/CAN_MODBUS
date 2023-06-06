#ifndef _PROTOCOL_API_H_
#define _PROTOCOL_API_H_
#include "xsto_api_ii.h"

#define COMBINE_COUNT_MAX 	8
#define DATA_MXA			128

typedef enum
{
	FUNC_READ 			= 0x03,
	FUNC_WRITE			= 0x04,
	FUNC_SUBSCRIBE 		= 0x05,
	FUNC_UNSUBSCRIBE 	= 0x06,
	FUNC_GETSUBSCRIBE	= 0x07,
}funC_e;	//功能码

typedef enum
{
	ERRC_READ 			= 0x83,
	ERRC_WRITE			= 0x84,
	ERRC_SUBSCRIBE 		= 0x85,
	ERRC_UNSUBSCRIBE 	= 0x86,
	ERRC_GETSUBSCRIBE	= 0x87,
}errC_e;	//差错码

typedef enum
{
	EXCC_NO_FUNC = 0x01,	//不支持的功能码
	EXCC_ADDRESS = 0x02,	//地址超出范围
	EXCC_NUMBER	 = 0x03,	//数量超出范围
	EXCC_ELSE	 = 0x04,	//其他错误
}excC_e;	//异常码

//length <= 508
typedef struct
{
	uint8_t funC;			//功能码
	uint8_t msgID;      	// msgid标记消息
	uint16_t length;		//数据长度
	uint8_t data[DATA_MXA];		//数据内容	
} __attribute__((packed)) PDU_t;	//协议数据单元

typedef struct
{
	uint8_t errC;			//差错码
	uint8_t msgID;      	// msgid标记消息
	uint16_t length;		//数据长度 = 1
	uint8_t excC;			//异常码
} __attribute__((packed)) PDU_err_t;	//协议数据单元

//读寄存器时的组合
typedef struct
{
	uint16_t address;		//地址
	uint16_t size;			//字节大小
} __attribute__((packed)) combine_t;	//组合

typedef struct
{
	uint8_t combineCount; 	// 组合项个数
	combine_t combine[COMBINE_COUNT_MAX];
} __attribute__((packed)) combine_read_t;

//组合+数据
typedef struct
{
	combine_t combine;
	uint8_t *data;		//数据，长度为combine组合中size大小
} __attribute__((packed)) combine_data_t;

typedef struct
{
	uint8_t combineCount; 	// 组合项个数
	combine_data_t combine_data[COMBINE_COUNT_MAX];
} __attribute__((packed)) combine_write_t;

//订阅组合加时间参数
typedef struct
{
	uint16_t timing; 	// 定时发 /ms；填 0 是以触发方式返回数据，有外部触发才返回数据
	combine_read_t combine_read;
} __attribute__((packed)) combine_subscribe_t;


void PDU_to_msg_data(uint16_t TYPE, PDU_t *PDU, MsgData_t *msg_data);
void PDU_err_to_msg_data(uint16_t TYPE, PDU_err_t *PDU_err, MsgData_t *msg_data);
void combine_read_to_PDU(uint8_t msgID, combine_read_t *combine_read, PDU_t *PDU);
void combine_subscribe_to_PDU(uint8_t msgID, combine_subscribe_t *combine_subscribe, PDU_t *PDU);
void combine_write_to_PDU(uint8_t msgID, combine_write_t *combine_write, PDU_t *PDU);
void msg_data_to_PDU(uint8_t *data, uint16_t length, PDU_t *PDU);
void msg_data_to_PDU_err(uint8_t *data, uint16_t length, PDU_err_t *PDU_err);
void PDU_to_combine_read(PDU_t *PDU, combine_read_t *combine_read);
void PDU_to_combine_write(PDU_t *PDU, combine_write_t *combine_write);


void protocol_respond(MsgData_t *request_msg_data, MsgData_t *respond_msg_data);//使用
void protocol_respond_test(void);


uint8_t add_subscribe_combine_node(uint16_t TYPE, PDU_t *PDU);
uint8_t del_subscribe_combine_node(uint16_t TYPE, PDU_t *PDU);
void get_subscribe_combine_node(uint16_t TYPE, uint8_t msgID, PDU_t *res_PDU);
void protocol_subscribe_loop(void);//使用

void add_subscribe_test(void);
void del_subscribe_test(void);
void get_subscribe_test(void);

void list_test(void);


void PDU_test(void);
void write_test(void);


#endif


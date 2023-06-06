/**********************************************************/
//@Autor:Raw
//@Version:0.0.1
//@End Time:2023/4/17
//@Function:xsto_api_ii.c
/**********************************************************/
#include "xsto_api_ii.h"
#include "protocol.h"
#define FRAME_BUFFER_SIZE       (128+2+2+2+1+1+2) //len(2), cmd(1), to(1)
// frame state
#define FRAME_SYNC     0
#define FRAME_HEADER   1
#define FRAME_DATA     2


enum Command
{
  CMD_REGISTER = 0x01,
  CMD_HEARTBEAT = 0x02,
	CMD_READ = 0x03,
	CMD_WRITE = 0X04,
	CMD_SUBSCRIBE = 0X05,
	CMD_USUBSCRIBE = 0X06,
	CMD_GETSUBSCRIBE = 0X07,
  CMD_UPGRADE = 0xf0,
  CMD_UPGRADE_REQUEST = 0xf1,
  CMD_UPGRADE_PK = 0xf2,
  CMD_UPGRADE_FINISH = 0xf3,
	CMD_UPGRADE_STATUS = 0xf4,
};
static uint8_t SendBuffer[FRAME_BUFFER_SIZE];
typedef struct
{
  uint16_t Start;
	uint16_t type;
	uint8_t cmd;
	uint8_t reserve;
	uint16_t Length;
} __attribute__((packed)) header_t;
typedef struct
{
    union
    {
        uint8_t RawData[FRAME_BUFFER_SIZE];
        header_t Header;
    } DataFrame;
    uint16_t Count;
    uint8_t Status;
    uint8_t Version;
    crc16_t crc16;
} DataFrameBuffer_t;
static DataFrameBuffer_t Recv_Buffer;
static API_Config_t* api_config = NULL;
static uint32_t heartbeat = 0;
static uint32_t api_time = 0;

void api_init(API_Config_t* config)
{
    api_config = config;
    api_config->Status = MODULE_STATUS_OFFLINE;
}


static void module_register(void)
{
  if (api_config == NULL) return;

	uint16_t frame_len=0;
	uint16_t crc16 =0;
	uint16_t len = 6+23;

	SendBuffer[0] = 0x5a;
	SendBuffer[1] = 0x5a;
	frame_len=frame_len+2;
	memcpy(&SendBuffer[frame_len],&api_config->Type,2);
	frame_len = frame_len+2;
	SendBuffer[frame_len] = CMD_REGISTER;
	frame_len =frame_len+1;
	SendBuffer[frame_len] = 0x00;
	frame_len =frame_len+1;
	memcpy(&SendBuffer[frame_len],&len,2);
	frame_len = frame_len+2;
	memcpy(&SendBuffer[frame_len],&api_config->SoftwareVersion,4);
	frame_len = frame_len+4;
	memcpy(&SendBuffer[frame_len],&api_config->HardwareVersion,4);
	frame_len = frame_len+4;
	memcpy(&SendBuffer[frame_len],&api_config->ModuleID,12);
	frame_len = frame_len+12;
	memcpy(&SendBuffer[frame_len],&api_config->ModuleTimeout,2);
	frame_len = frame_len+2;
	memcpy(&SendBuffer[frame_len],&api_config->Status,1);
	frame_len = frame_len+1;

	crc16 = crc_ccitt(&SendBuffer[2],frame_len-2);
	memcpy(&SendBuffer[frame_len],&crc16,2);
	frame_len = frame_len+2;
	api_port_send(frame_len, SendBuffer);
}

static void master_register_ack(void)
{
  if (api_config == NULL) return;

	uint16_t frame_len=0;
	uint16_t crc16 =0;
	uint16_t len = 6+1;

	SendBuffer[0] = 0x5a;
	SendBuffer[1] = 0x5a;
	frame_len=frame_len+2;
	memcpy(&SendBuffer[frame_len],&api_config->Type,2);
	frame_len = frame_len+2;
	SendBuffer[frame_len] = CMD_REGISTER;
	frame_len =frame_len+1;
	SendBuffer[frame_len] = 0x00;
	frame_len =frame_len+1;
	memcpy(&SendBuffer[frame_len],&len,2);
	frame_len = frame_len+2;
	SendBuffer[frame_len] = 0x00;
	frame_len =frame_len+1;
	crc16 = crc_ccitt(&SendBuffer[2],frame_len-2);
	memcpy(&SendBuffer[frame_len],&crc16,2);
	frame_len = frame_len+2;
	api_port_send(frame_len, SendBuffer);
}

static void master_heartbeat_ack(uint32_t time)
{
  if (api_config == NULL) return;

	uint16_t frame_len=0;
	uint16_t crc16 =0;
	uint16_t len = 6+4;

	SendBuffer[0] = 0x5a;
	SendBuffer[1] = 0x5a;
	frame_len=frame_len+2;
	memcpy(&SendBuffer[frame_len],&api_config->Type,2);
	frame_len = frame_len+2;
	SendBuffer[frame_len] = CMD_HEARTBEAT;
	frame_len =frame_len+1;
	SendBuffer[frame_len] = 0x00;
	frame_len =frame_len+1;
	memcpy(&SendBuffer[frame_len],&len,2);
	frame_len = frame_len+2;
	memcpy(&SendBuffer[frame_len],&time,4);
	frame_len = frame_len+4;

	crc16 = crc_ccitt(&SendBuffer[2],frame_len-2);
	memcpy(&SendBuffer[frame_len],&crc16,2);
	frame_len = frame_len+2;
	api_port_send(frame_len, SendBuffer);
}

uint8_t module_heartbeat(uint32_t time)
{
  uint16_t frame_len=0;
	uint16_t crc16 =0;
	uint16_t len = 6+4;
	
	SendBuffer[0] = 0x5a;
	SendBuffer[1] = 0x5a;
	frame_len=frame_len+2;
	memcpy(&SendBuffer[frame_len],&api_config->Type,2);
	frame_len = frame_len+2;
	SendBuffer[frame_len] = CMD_HEARTBEAT;
	frame_len =frame_len+1;
	SendBuffer[frame_len] = 0x00;
	frame_len =frame_len+1;
	memcpy(&SendBuffer[frame_len],&len,2);
	frame_len = frame_len+2;
	
	memcpy(&SendBuffer[frame_len],&time,4);
	frame_len = frame_len+4;
	
	crc16 = crc_ccitt(&SendBuffer[2],frame_len-2);
	memcpy(&SendBuffer[frame_len],&crc16,2);
	frame_len = frame_len+2;
	api_port_send(frame_len, SendBuffer);
  return 0;
}
static void module_response(void)
{
    uint8_t cmd;
    uint16_t length;
    uint8_t* data;
	uint32_t timer_heat;
	uint8_t version=Recv_Buffer.Version;
	API_Message_t api_msg;
	MasterMsgData_t master_msg;
	cmd = Recv_Buffer.DataFrame.Header.cmd;
	length = Recv_Buffer.DataFrame.Header.Length;
	data = &Recv_Buffer.DataFrame.RawData[8];

	if(version==1)
	{
		api_msg.msg_data.cmd=cmd;
		api_msg.msg_data.TYPE=Recv_Buffer.DataFrame.Header.type;
		api_msg.msg_data.Length = length;
		api_msg.msg_data.Data=data;
	}
	switch(api_msg.msg_data.cmd)
	{
		case CMD_REGISTER:
			#if Slave_Device
			api_config->Type = api_msg.msg_data.TYPE;
		  heartbeat = api_time;
			api_config->Status = MODULE_STATUS_ONLINE;
			#endif
			#if Master_Device
			master_register_ack();
			#endif
			break;
	case CMD_HEARTBEAT:
			memcpy((void *)&timer_heat,api_msg.msg_data.Data,4);
			#if Slave_Device
			heartbeat = timer_heat;
			#endif
			#if Master_Device
			master_heartbeat_ack(timer_heat);
			#endif
			break;
	case CMD_READ:
			#if Slave_Device
			protocol_received(&api_msg,1,&master_msg);
			#endif
			#if Master_Device
			protocol_received(&api_msg,0,&master_msg);
			api_on_message_received(master_msg);
			#endif
		break;
	case CMD_WRITE:
			#if Slave_Device
			protocol_received(&api_msg,1,&master_msg);
			#endif
			#if Master_Device
			protocol_received(&api_msg,0,&master_msg);
			api_on_message_received(master_msg);
			#endif
		break;
	case CMD_SUBSCRIBE:
			#if Slave_Device
			protocol_received(&api_msg,1,&master_msg);
			#endif
			#if Master_Device
			protocol_received(&api_msg,0,&master_msg);
			api_on_message_received(master_msg);
			#endif
		break;
	case CMD_USUBSCRIBE:
			#if Slave_Device
			protocol_received(&api_msg,1,&master_msg);
			#endif
			#if Master_Device
			protocol_received(&api_msg,0,&master_msg);
			api_on_message_received(master_msg);
			#endif
		break;
	case CMD_GETSUBSCRIBE:
			#if Slave_Device
			protocol_received(&api_msg,1,&master_msg);
			#endif
			#if Master_Device
			protocol_received(&api_msg,0,&master_msg);
			api_on_message_received(master_msg);
			#endif
		break;
	}
}

static uint8_t module_parse_frame(uint8_t data)
{
    switch (Recv_Buffer.Status)
    {
        case FRAME_SYNC:
            if (data == 0x5A)
            {
                Recv_Buffer.DataFrame.RawData[Recv_Buffer.Count++] = data;
                Recv_Buffer.Version = 1;
                Recv_Buffer.Status = FRAME_HEADER;
                break;
            }
            goto API_FRAME_RESET;

        case FRAME_HEADER:
            Recv_Buffer.DataFrame.RawData[Recv_Buffer.Count++] = data;
            if (Recv_Buffer.Count == 8)//sizeof(header_t))
            {
                if ((Recv_Buffer.Version==1)&&Recv_Buffer.DataFrame.Header.Length <= (FRAME_BUFFER_SIZE - 10))
                {
                    Recv_Buffer.crc16.current = CRC16_INIT_VALUE;
                    Recv_Buffer.crc16.crc16 = CRC16_INIT_VALUE;
                    Recv_Buffer.Status = FRAME_DATA;
                }
                else    //帧长度异常
                {
                    goto API_FRAME_RESET;
                }
            }
            break;

        case FRAME_DATA: //from to cmd & data
            Recv_Buffer.DataFrame.RawData[Recv_Buffer.Count++] = data;
 //           crc16(data, &Recv_Buffer.crc16);
            if (((Recv_Buffer.Version==1)&&(Recv_Buffer.Count >= Recv_Buffer.DataFrame.Header.Length + 4)))
            {
								Recv_Buffer.crc16.crc16 = crc_ccitt(&Recv_Buffer.DataFrame.RawData[2],Recv_Buffer.DataFrame.Header.Length);
                if ((Recv_Buffer.Version == 1 && Recv_Buffer.crc16.crc16 == (Recv_Buffer.DataFrame.RawData[Recv_Buffer.Count-1]<<8|Recv_Buffer.DataFrame.RawData[Recv_Buffer.Count-2])))
                {//帧数据校验通过
                    //处理模块帧数据
                    module_response();
                }
                else
                {
                    ;//帧数据校验错误
                }
                //接收一帧数据完成
                goto API_FRAME_RESET;
            }
            break;

        default:
            goto API_FRAME_RESET;
    }

    return 0;
    
    API_FRAME_RESET:
    Recv_Buffer.Count = 0;
    Recv_Buffer.Status = FRAME_SYNC;
    return 0;
}
/**
 *@brief:      	api_port_received
 *@details:    	数据接收解析器
 *@param[in]   	uint16_t len，uint8_t * data
 *@param[out]  	void
 *@retval:     	void
 */
void api_port_received(uint16_t len, uint8_t * data)//weak
{
    while (len--)
    {
        module_parse_frame(*data++);
    }
}
	
/**
 *@brief:      	api_port_send
 *@details:    	数据发送
 *@param[in]   	uint16_t len，uint8_t * data
 *@param[out]  	void
 *@retval:     	void
 */
__weak void api_port_send(uint16_t len, uint8_t * data) 
{
	
	
}

/**
 *@brief:      	api_on_message_received
 *@details:    	数据发送
 *@param[in]   	API_Message_t message
 *@param[out]  	void
 *@retval:     	void
 */
__weak void api_on_message_received(MasterMsgData_t message)
{
	
}

/**
 *@brief:      	api_message_send
 *@details:    	发送帧组帧
 *@param[in]   	API_Message_t *msg
 *@param[out]  	void
 *@retval:     	void
 */

__weak void protocol_init(void)
{

}

void api_message_send(API_Message_t *msg)
{
	uint16_t msg_len = 0;
	uint16_t frame_len=0;
	uint16_t crc16 =0;
	msg_len = msg->msg_data.Length + 4;		
	if(msg_len>(FRAME_BUFFER_SIZE+10))return;	


	msg->HEAD = 0x5A5A;
	memcpy(&SendBuffer[frame_len],&msg->HEAD,2);
	frame_len = frame_len+2;
	memcpy(&SendBuffer[frame_len],&msg->msg_data.TYPE,2);
	frame_len = frame_len+2;
	SendBuffer[frame_len] = msg->msg_data.cmd;
	frame_len = frame_len+1;
	SendBuffer[frame_len] = 0;
	frame_len = frame_len+1;
	memcpy(&SendBuffer[frame_len],&msg->msg_data.Length,2);
	frame_len = frame_len+2;

	if((msg->msg_data.Length-6)<=(FRAME_BUFFER_SIZE-frame_len))
	{
		memcpy(&SendBuffer[frame_len],msg->msg_data.Data, msg->msg_data.Length-6);
	}
	frame_len+=msg->msg_data.Length-6;
	crc16 = crc_ccitt(&SendBuffer[2],frame_len-2);
	memcpy(&SendBuffer[frame_len],&crc16,2);
	frame_len = frame_len+2;
	api_port_send(frame_len, SendBuffer);
}

void api_loop(uint32_t time)
{
    static uint32_t last_tick = 0;
    static uint16_t heartbeat_interval = 1000;
    api_time = time;
    if (api_config == NULL) return;

    if (api_config->Status == MODULE_STATUS_OFFLINE)
    {
        if (last_tick + 500 < time || last_tick == 0)
        {
            last_tick = time;
            heartbeat_interval = api_config->ModuleTimeout <= 1500 ? 500 : api_config->ModuleTimeout / 3;
            module_register();
        }
        return;
    }
		else
		{
			//if ((last_tick + api_config->ModuleTimeout - 1000) <= time)
			if (time - last_tick >= heartbeat_interval)
			{
					last_tick = time;
					module_heartbeat(time);
			}
			else if (time - heartbeat >= api_config->ModuleTimeout)
			{
					api_config->Status = MODULE_STATUS_OFFLINE;
			}
		}

}


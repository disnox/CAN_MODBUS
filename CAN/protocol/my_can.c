#include "my_can.h"

uint8_t Car_buff[CAN_BUFFER_SIZE] = {0};
Can_fifo_buffer_t CANRx_Buffer;
uint8_t CAN_Rx_buffer[CAN_BUFFER_SIZE];

Can_fifo_buffer_t CANTx_Buffer;
uint8_t CAN_Tx_buffer[CAN_BUFFER_SIZE];

/**
************************************************************************
* @brief:      	can_filter_init(void)
* @param[in]:   void
* @retval:     	void
* @details:    	CAN滤波器初始化
************************************************************************
**/
void can_filter_init(void)
{
    CAN_FilterTypeDef can_filter_st;
    can_filter_st.FilterActivation = ENABLE;
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_st.FilterIdHigh = 0x0000;
    can_filter_st.FilterIdLow = 0x0000;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterBank = 0;
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}
/**
************************************************************************
* @brief:      	CAN_Para_Init(void)
* @param[in]:   void
* @retval:     	void
* @details:    	数据结构体初始化
************************************************************************
**/
void CAN_Para_Init(void)
{
	CANTx_Buffer.data = CAN_Tx_buffer;
	CANTx_Buffer.size = CAN_BUFFER_SIZE;
	CANTx_Buffer.read_index = 0;
	CANTx_Buffer.write_index = 0;
	
	CANRx_Buffer.data = CAN_Rx_buffer;
	CANRx_Buffer.size = CAN_BUFFER_SIZE;
	CANRx_Buffer.read_index = 0;
	CANRx_Buffer.write_index = 0;
}

/**
************************************************************************
* @brief:      	CANx_SendStdData
* @param[in]:   hcan     CAN的句柄
* @param[in]:  	ID       数据帧ID
* @param[in]:  	pData    数组指针
* @param[in]:  	Len      字节数0~8
* @retval:     	void
* @details:    	数据发送
************************************************************************
**/
uint8_t CANx_SendStdData(CAN_HandleTypeDef* hcan,uint16_t ID,uint8_t *pData,uint16_t Len)
{
  static CAN_TxHeaderTypeDef   Tx_Header;
	
	Tx_Header.StdId=ID;
	Tx_Header.ExtId=0;
	Tx_Header.IDE=0;
	Tx_Header.RTR=0;
	Tx_Header.DLC=Len;
  /*找到空的发送邮箱，把数据发送出去*/
	if(HAL_CAN_AddTxMessage(hcan, &Tx_Header, pData, (uint32_t*)CAN_TX_MAILBOX0) != HAL_OK) {
		if(HAL_CAN_AddTxMessage(hcan, &Tx_Header, pData, (uint32_t*)CAN_TX_MAILBOX1) != HAL_OK) {
			HAL_CAN_AddTxMessage(hcan, &Tx_Header, pData, (uint32_t*)CAN_TX_MAILBOX2);
    }
  }
  return 0;
}
/**
************************************************************************
* @brief:      	HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
* @param[in]:   hcan     CAN的句柄
* @retval:     	void
* @details:    	从缓存区读取数据长度
************************************************************************
**/
CAN_RxHeaderTypeDef rx_header;
uint8_t rx_data[8];
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan == &hcan1) {
		HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);
		for(int i = 0; i < 8; i++) {
			CANRx_Buffer.data[CANRx_Buffer.write_index] = rx_data[i];	
			CANRx_Buffer.write_index = (CANRx_Buffer.write_index + 1) % CANRx_Buffer.size;
		}	
	}
}
/**
************************************************************************
* @brief:      	can_serial_available(void)
* @param[in]:   void
* @retval:     	void
* @details:    	从缓存区读取数据长度
************************************************************************
**/
uint16_t can_serial_available(void)
{
	uint16_t len = 0;
	uint16_t CANRx_Buffer_read_index = CANRx_Buffer.read_index;
	uint16_t CANRx_Buffer_write_index = CANRx_Buffer.write_index;

	if (CANRx_Buffer_read_index > CANRx_Buffer_write_index) {
		len = CANRx_Buffer.size + CANRx_Buffer_write_index - CANRx_Buffer_read_index;
	}
	else if (CANRx_Buffer_read_index  < CANRx_Buffer_write_index) {
		len = CANRx_Buffer_write_index - CANRx_Buffer_read_index;
	}
	return len;
}
/**
************************************************************************
* @brief:      	can_serial_available(void)
* @param[in]:   void
* @retval:     	ch: 字节数据
* @details:    	从缓存区读取一个字节
************************************************************************
**/
uint8_t can_serial_read_char(void)
{
    uint8_t ch = 0;
    ch = CANRx_Buffer.data[CANRx_Buffer.read_index];
    CANRx_Buffer.read_index = (CANRx_Buffer.read_index + 1) % CANRx_Buffer.size;
    return ch;
}
/**
************************************************************************
* @brief:      	can_serial_read(uint8_t *buffer, uint16_t length)
* @param[in]:   buffer: 数据
* @param[in]:   length: 缓冲区数据长度
* @retval:     	i: 数据个数
* @details:    	从缓存区读取数据
************************************************************************
**/
uint16_t can_serial_read(uint8_t *buffer, uint16_t length)
{
	uint16_t i = 0;

	for (i = 0; i < length; i++) {
		buffer[i] = CANRx_Buffer.data[CANRx_Buffer.read_index];
		CANRx_Buffer.read_index = (CANRx_Buffer.read_index + 1) % CANRx_Buffer.size;
	}
	return i;
}
/**
************************************************************************
* @brief:      	can_tx_buf
* @param[in]:   buffer: 数据
* @param[in]:   length: 缓冲区数据长度
* @param[in]:   Tx_Buffer: 发送结构体句柄
* @retval:     	i: 数据个数
* @details:    	把待发送的数据写入缓冲区
************************************************************************
**/
uint16_t can_tx_buf(uint8_t *buffer, uint16_t length, Can_fifo_buffer_t* Tx_Buffer)
{
	uint16_t i = 0;

	for (i = 0; i < length; i++) {
		Tx_Buffer->data[Tx_Buffer->write_index] = buffer[i];
		Tx_Buffer->write_index = (Tx_Buffer->write_index + 1) % Tx_Buffer->size;
	}
	return i;
}
/**
************************************************************************
* @brief:      	can_serial_write(uint8_t *buffer, uint16_t length)
* @param[in]:   buffer: 数据
* @param[in]:   length: 缓冲区数据长度
* @retval:     	i: 数据个数
* @details:    	把待发送的数据写入缓冲区
************************************************************************
**/
uint16_t can_serial_write(uint8_t *buffer, uint16_t length)
{
    uint16_t i = 0;
	  can_tx_buf(buffer, length, &CANTx_Buffer);
    return i;
}
/**
************************************************************************
* @brief:      	can_tx_available(Can_fifo_buffer_t * Tx_Buffer)
* @param[in]:   Tx_Buffer: 发送结构体句柄
* @retval:     	len: 缓冲区数据长度
* @details:    	判断缓冲区数据长度
************************************************************************
**/
uint16_t can_tx_available(Can_fifo_buffer_t * Tx_Buffer)
{
	uint16_t len = 0;
	if (Tx_Buffer->read_index > Tx_Buffer->write_index) {
		len = Tx_Buffer->size + Tx_Buffer->write_index - Tx_Buffer->read_index;
	}
	else if (Tx_Buffer->read_index  < Tx_Buffer->write_index) {
		len = Tx_Buffer->write_index - Tx_Buffer->read_index;
	}
	return len;
}
/**
************************************************************************
* @brief:      	can_transmit(void)
* @param[in]:   void
* @retval:     	void
* @details:    	can总线数据发出
************************************************************************
**/
uint16_t tx_len1 = 0;
uint8_t TxMsg[8];
void can_transmit(void)
{  
	tx_len1 =  can_tx_available(&CANTx_Buffer);
	if(tx_len1 > 0) {
		uint16_t temp;
		if(CAN_SEND_ID != 0) {
			if(tx_len1 > 8) {
				temp = CANTx_Buffer.size - CANTx_Buffer.read_index;
				if(temp < 8) {
					memcpy(TxMsg,&(CANTx_Buffer.data[CANTx_Buffer.read_index]), temp);
					memcpy(&TxMsg[temp],&(CANTx_Buffer.data[0]), 8-temp);
				}
				else {
					memcpy(TxMsg, &(CANTx_Buffer.data[CANTx_Buffer.read_index]), 8);
				}
				CANx_SendStdData(&hcan1,CAN_SEND_ID,TxMsg, 8);
				CANTx_Buffer.read_index = (CANTx_Buffer.read_index + 8)%CANTx_Buffer.size;
			}
			else {
				temp = CANTx_Buffer.size - CANTx_Buffer.read_index;
				if(temp < tx_len1) {
					memcpy(TxMsg,&(CANTx_Buffer.data[CANTx_Buffer.read_index]), temp);
					memcpy(&TxMsg[temp],&(CANTx_Buffer.data[0]), tx_len1-temp);
				}
				else {
					memcpy(TxMsg,&(CANTx_Buffer.data[CANTx_Buffer.read_index]), tx_len1);
				}
				CANx_SendStdData(&hcan1, CAN_SEND_ID, TxMsg, tx_len1);
				CANTx_Buffer.read_index = (CANTx_Buffer.read_index + tx_len1)%CANTx_Buffer.size;
			}
		}
	}
}
/**
************************************************************************
* @brief:      	can_receive(void)
* @param[in]:   void
* @retval:     	void
* @details:    	can总线数据接收存入队列
************************************************************************
**/
void can_receive(void)
{	
	uint16_t len = 0;
	len = can_serial_available();
	if(len > 0 && len < CAN_BUFFER_SIZE) {
		can_serial_read(Car_buff,len);
		api_port_received(len, Car_buff);
	}
}
/**
************************************************************************
* @brief:      	api_port_send(uint16_t len, uint8_t *data)
* @param[in]:   len: 数据长度
* @param[in]:   data: 数据
* @retval:     	void
* @details:    	can总线数据存入队列
************************************************************************
**/
void api_port_send(uint16_t len, uint8_t *data) 
{
	can_serial_write(data, len);
}

//MasterMsgData_t rxmessage;  //主机做的回调
//void api_on_message_received(MasterMsgData_t message)
//{
//	memcpy(&rxmessage,&message,sizeof(MasterMsgData_t));
//	
//}


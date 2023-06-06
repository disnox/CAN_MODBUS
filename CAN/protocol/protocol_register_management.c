/**
 * @file            protocol_register_management.c
 * @brief           通信协议使用的寄存器管理
 * @author          
 * @date            
 * @version         初稿
 * @par             
 * @par History:
*/

#include "protocol_register_management.h"
#include <string.h>

//#define PROTOCOL_REGISTER_TEST

#ifdef PROTOCOL_REGISTER_TEST
#include "log.h"
#endif

#ifdef REGISTER_POINT


uint8_t reg_init = 1;//未初始化为1，初始化后为0
//得到管理的指针
uint8_t *Register[RegisterSize];


//使用指针方式需要初始化地址与对应指针建立连接
/**
 *@brief:      	register_map
 *@details:    	将寄存器管理对应到实际数据地址
				1、需要调用此函数进行初始化，才能进行读写
 *@param[in]   	uint16_t address, uint8_t *point, uint16_t size
 *@param[out]  	void
 *@retval:     	int8_t	-1=超出范围；0=成功
 */
int8_t register_map(uint16_t address, uint8_t *point, uint16_t size)
{
	if(reg_init == 1)
	{
		for(uint16_t i=0; i<RegisterSize; i++)
		{
			Register[i] = &reg_init;
		}
		reg_init = 0;
	}
	if(address >= RegisterSize || address + size > RegisterSize) return -1;
	for(uint16_t i=0; i<size; i++)
	{
		Register[address + i] = point + i;
	}
	return 0;
}


/**
 *@brief:      	register_write
 *@details:    	向寄存器写入内容
 *@param[in]   	uint16_t address, uint8_t *buff, uint16_t size
 *@param[out]  	void
 *@retval:     	int8_t	OK = 0；超出范围=-1；
 */
int8_t register_write(uint16_t address, uint8_t *buff, uint16_t size)
{
	if(address >= RegisterSize || address + size > RegisterSize) return -1;
	for(uint16_t i=0; i<size; i++)
	{
		*Register[address + i] = *(buff + i);
	}
	return 0;
}

/**
 *@brief:      register_read
 *@details:    读出指定大小寄存器内容
 *@param[in]   uint16_t address, uint16_t size
 *@param[out]  uint8_t *buff, 
 *@retval:     int8_t	OK = 0；超出范围=-1；
 */
int8_t register_read(uint16_t address, uint16_t size, uint8_t *buff)
{
	if(address >= RegisterSize || address + size > RegisterSize) return -1;
	for(uint16_t i=0; i<size; i++)
	{
		*(buff + i) = *Register[address + i];
	}
	return 0;
}

#ifdef PROTOCOL_REGISTER_TEST
static uint8_t a[8] = {1,2,3,4,5,6,7,8};
void register_test(void)
{
	//初始化
	register_map(0x10, a, 8);
	
	//读
	uint8_t a2[8];
	register_read(0x14, 4, a2);
	xa_log(LOG_DEBUG, "register_read a2");
	PrintFormat(a2, 4);
	
	//写
	a[0] = 9;//直接通过原数组修改
	a2[0] = 10;
	register_write(0x11, a2, 1);//通过寄存器修改
	register_read(0x10, 8, a2);
	xa_log(LOG_DEBUG, "register_write ");
	PrintFormat(a2, 8);
}
#endif


#elif REGISTER_ARRAY

/* 定义协议管理的寄存器映射大小 */
#define RegisterArraySize 0x30

//定义寄存器表
__attribute__((section(".RAM_D1")))__attribute((aligned(4))) uint8_t RegisterArray[RegisterArraySize];

//定义互斥量管理共享寄存器
osMutexId_t regMemMutexHandle;
const osMutexAttr_t regMemMutex_attributes = {
  .name = "regMemMutex"
};

/**
 *@brief:      register_write
 *@details:    向寄存器写入内容
 *@param[in]   uint16_t address, uint8_t *buff, uint16_t size
 *@param[out]  void
 *@retval:     int8_t	OK = 0；超出范围=-1；地址或大小不是偶数=-2
 */
int8_t register_write(uint16_t address, uint8_t *buff, uint16_t size)
{
	if(address + size > RegisterArraySize) return -1;
	if(address % 2 || size % 2) return -2;
	if(regMemMutexHandle == NULL)
		regMemMutexHandle = system_mutex_create(&regMemMutex_attributes);
	system_mutex_acquire(regMemMutexHandle, osWaitForever);
	memcpy(RegisterArray+address, buff, size);
	system_mutex_release(regMemMutexHandle);
	return 0;
}

/**
 *@brief:      register_read
 *@details:    读出指定大小寄存器内容
 *@param[in]   uint16_t address, uint16_t size
 *@param[out]  uint8_t *buff, 
 *@retval:     int8_t	OK = 0；超出范围=-1；地址或大小不是偶数=-2
 */
int8_t register_read(uint16_t address, uint16_t size, uint8_t *buff)
{
	if(address + size > RegisterArraySize) return -1;
	if(address % 2 || size % 2) return -2;
	if(regMemMutexHandle == NULL)
		regMemMutexHandle = system_mutex_create(&regMemMutex_attributes);
	system_mutex_acquire(regMemMutexHandle, osWaitForever);
	memcpy(buff, RegisterArray+address, size);
	system_mutex_release(regMemMutexHandle);
	return 0;
}


void register_test(void)
{
	uint8_t buff[] = {0,1,2,3,4,5,6,7};
	register_write(0x00, buff, 8);
	uint8_t buff2[8];
	register_read(0x04, 4, buff2);
	xa_log(LOG_DEBUG, "register_read buff2");
	PrintFormat(buff2, 4);
	
	uint32_t count = 0x00010002;
	register_write(0x08, (uint8_t*)&count, 4);
	uint32_t count2;
	register_read(0x08, 4, (uint8_t*)&count2);
	xa_log(LOG_DEBUG, "count2: %08x", count2);
	PrintFormat((uint8_t*)&count2, 4);
	
}

#endif


















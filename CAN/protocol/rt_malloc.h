#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
//#include "log.h"

typedef int                             rt_bool_t;      /**< boolean type */
typedef long                            rt_base_t;      /**< Nbit CPU related date type */
typedef unsigned long                   rt_ubase_t;     /**< Nbit unsigned CPU related data type */

typedef signed   char                   rt_int8_t;      /**<  8bit integer type */
typedef signed   short                  rt_int16_t;     /**< 16bit integer type */
typedef signed   int                    rt_int32_t;     /**< 32bit integer type */
typedef unsigned char                   rt_uint8_t;     /**<  8bit unsigned integer type */
typedef unsigned short                  rt_uint16_t;    /**< 16bit unsigned integer type */
typedef unsigned int                    rt_uint32_t;    /**< 32bit unsigned integer type */

typedef rt_ubase_t                      rt_size_t;      /**< Type for size number */

/* Internal SRAM memory size[Kbytes] <8-64>, Default: 64*/
#define STM32_SRAM_SIZE      1024
#define STM32_SRAM_END       (0x20000000 + STM32_SRAM_SIZE * 1024)

extern int Image$$RW_IRAM1$$ZI$$Limit;
#define HEAP_BEGIN      ((void *)&Image$$RW_IRAM1$$ZI$$Limit)

//#define HEAP_END        (void*)((char*)HEAP_BEGIN + 4*1024)
#define HEAP_END        ((void*)STM32_SRAM_END)

//#define RT_DEBUG_MEM                   1
#ifndef RT_DEBUG_MEM
#define RT_DEBUG_MEM                   0
#endif


#define RT_ALIGN_SIZE 4
#define RT_NULL                         (0)

//#define RT_DEBUG_LOG(type, message)                                           \
//do                                                                            \
//{                                                                             \
//    if (type)                                                                 \
//        uart_printf message;                                                  \
//}                                                                             \
//while (0)
#define RT_DEBUG_LOG(...)


#define RT_ASSERT(EX)                                                         \
if (!(EX))                                                                    \
{                                                                             \
}

#define RTM_EXPORT(symbol)

/**
 * The hook function call macro
 */
#ifdef RT_USING_HOOK
#define RT_OBJECT_HOOK_CALL(func, argv) \
    do { if ((func) != RT_NULL) func argv; } while (0)
#else
#define RT_OBJECT_HOOK_CALL(func, argv)
#endif

/**
 * @ingroup BasicDef
 *
 * @def RT_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. RT_ALIGN(13, 4)
 * would return 16.
 */
#define RT_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))



/**
 * @ingroup BasicDef
 *
 * @def RT_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. RT_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define RT_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

void rt_system_heap_init(void *begin_addr, void *end_addr);

void *rt_malloc(rt_size_t size); 

void *rt_realloc(void *rmem, rt_size_t newsize);

void *rt_calloc(rt_size_t count, rt_size_t size);

void rt_free(void *rmem);

void rt_memory_info(rt_uint32_t *total,
                    rt_uint32_t *used,
                    rt_uint32_t *max_used);


#ifdef __cplusplus
}
#endif

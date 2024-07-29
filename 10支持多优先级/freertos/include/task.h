#ifndef __INC_TASK_H__
#define __INC_TASK_H__


#include "list.h"

/* 空闲任务优先级宏定义 */
#define tskIDLE_PRIORITY                  ((UBaseType_t) 0U)
#define taskYIELD()                       portYIELD()


#define taskENTER_CRITICAL()		           portENTER_CRITICAL()
#define taskENTER_CRITICAL_FROM_ISR()      portSET_INTERRUPT_MASK_FROM_ISR()

#define taskEXIT_CRITICAL()			           portEXIT_CRITICAL()
#define taskEXIT_CRITICAL_FROM_ISR( x )    portCLEAR_INTERRUPT_MASK_FROM_ISR( x )



/* 任务句柄 */
typedef void* TaskHandle_t;


#if(configSUPPORT_STATIC_ALLOCATION == 1)
TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,          /* 任务入口 */
																 const char* const pcName,           /* 任务名称，字符串形式 */
																 const uint32_t ulStackDepth,        /* 任务栈大小，单位为字 */
																 void * const pvParameters,          /* 任务形参 */
																 UBaseType_t uxPriority,
																 StackType_t* const puxStackBuffer,  /* 任务栈起始地址 */
																 TCB_t* const pxTaskBuffer );         /* 任务控制块指针 */
#endif /* configSUPPORT_STATIC_ALLOCATION */

void prvInitialiseTaskLists( void );                                
void vTaskStartScheduler( void );
void vTaskSwitchContext( void );
void vTaskDelay( const TickType_t xTicksToDelay );
void xTaskIncrementTick( void );												 

#endif /* __INC_TASK_H__ */


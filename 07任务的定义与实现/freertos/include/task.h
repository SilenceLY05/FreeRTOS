#ifndef __INC_TASK_H__
#define __INC_TASK_H__


#include "list.h"

#define taskYIELD()     portYIELD()


/* 任务句柄 */
typedef void* TaskHandle_t;


#if(configSUPPORT_STATIC_ALLOCATION == 1)
TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,          /* 任务入口 */
																 const char* const pcName,           /* 任务名称，字符串形式 */
																 const uint32_t ulStackDepth,        /* 任务栈大小，单位为字 */
																 void * const pvParameters,          /* 任务形参 */
																 StackType_t* const puxStackBuffer,  /* 任务栈起始地址 */
																 TCB_t* const pxTaskBuffer );         /* 任务控制块指针 */
#endif /* configSUPPORT_STATIC_ALLOCATION */

void prvInitialiseTaskLists( void );                                
void vTaskStartScheduler( void );
void vTaskSwitchContext( void );
																 

#endif /* __INC_TASK_H__ */


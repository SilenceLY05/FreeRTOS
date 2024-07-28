#ifndef __INC_TASK_H__
#define __INC_TASK_H__


#include "list.h"

#define tskIDLE_PRIORITY         ((UBaseType_t) 0U)

#define taskYIELD()     portYIELD()


/* ������ */
typedef void* TaskHandle_t;


#if(configSUPPORT_STATIC_ALLOCATION == 1)
TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,          /* ������� */
																 const char* const pcName,           /* �������ƣ��ַ�����ʽ */
																 const uint32_t ulStackDepth,        /* ����ջ��С����λΪ�� */
																 void * const pvParameters,          /* �����β� */
																 StackType_t* const puxStackBuffer,  /* ����ջ��ʼ��ַ */
																 TCB_t* const pxTaskBuffer );         /* ������ƿ�ָ�� */
#endif /* configSUPPORT_STATIC_ALLOCATION */

void prvInitialiseTaskLists( void );                                
void vTaskStartScheduler( void );
void vTaskSwitchContext( void );
void vTaskDelay( const TickType_t xTicksToDelay );
void xTaskIncrementTick( void );												 

#endif /* __INC_TASK_H__ */


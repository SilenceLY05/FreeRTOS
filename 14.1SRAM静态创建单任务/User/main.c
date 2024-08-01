/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "bsp_led.h"
#include "FreeRTOS.h"
#include "task.h"


/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
 static TaskHandle_t AppTaskCreate_Handle;
 /* LED������ */
 static TaskHandle_t LED_Task_Handle;

/* AppTaskCreate�����ջ */
static StackType_t AppTaskCreate_Stack[128];
/* LED�����ջ */
static StackType_t LED_Task_Stack[128];

/* AppTaskCreate������ƿ� */
static StaticTask_t AppTaskCreate_TCB;
/* LED������ƿ� */
static StaticTask_t LED_Task_TCB;


/* ���������ջ */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* ��ʱ�������ջ */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/* ����������ƿ� */
static StaticTask_t Idle_Task_TCB;
/* ��ʱ��������ƿ� */
static StaticTask_t Timer_Task_TCB;



/* �������� */
static void BSP_Init(void);
static void LED_Task(void *paramter);
static void AppTaskCreate(void);

/**
	* ʹ���˾�̬�����ڴ棬�������������������û�ʵ�֣�������task.c�ļ���������
	*	���ҽ��� configSUPPORT_STATIC_ALLOCATION ����궨��Ϊ 1 ��ʱ�����Ч
	*/
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
																	StackType_t **ppxIdleTaskStackBuffer,
																	uint32_t *pulIdleTaskStackSize);
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
																	StackType_t **ppxTimerTaskStackBuffer,
																	uint32_t *pulTimerTaskStackSize);




/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* ������Ӳ����ʼ�� */
	BSP_Init();
	printf("����һ��FreeRTOS-��̬���񴴽�������\r\n");
	AppTaskCreate_Handle = xTaskCreateStatic((TaskFunction_t) AppTaskCreate,
																			(const char*)"AppTaskCreate",
																			(uint32_t)128,
																			(void*)NULL,
																			(UBaseType_t)3,
																			(StackType_t*)AppTaskCreate_Stack,
																			(StaticTask_t*)&AppTaskCreate_TCB);
																			
	if(NULL != AppTaskCreate_Handle)  /* �����ɹ� */
		vTaskStartScheduler();   /* �������񣬿������� */
	
	while(1);
}

/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
	taskENTER_CRITICAL();    //�����ٽ���
	
	/* ����LED_Task���� */
	LED_Task_Handle = xTaskCreateStatic((TaskFunction_t) LED_Task,
																			(const char*)"LED_Task",
																			(uint32_t)128,
																			(void*)NULL,
																			(UBaseType_t)4,
																			(StackType_t*)LED_Task_Stack,
																			(StaticTask_t*)&LED_Task_TCB);
																			
	if(NULL != LED_Task_Handle) /* ��������ɹ� */
	{
		printf("LED_Task���񴴽��ɹ�");
	}
	else
	{
		printf("LED_Task���񴴽�ʧ��");
	}
	
	vTaskDelete(AppTaskCreate_Handle);   //ɾ��AppTaskCreate����
	
	taskEXIT_CRITICAL();  //�˳��ٽ���
}


/**********************************************************************
  * @ ������  �� LED_Task
  * @ ����˵���� LED_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void LED_Task(void *paramter)
{
	while(1)
	{
		LED1_ON;
		vTaskDelay(500);    /* ��ʱ500ticks */
		printf("LED_Task Running ,LED1_ON\r\n");
		
		LED1_OFF;
		vTaskDelay(500);    /* ��ʱ500ticks */
		printf("LED_Task Running ,LED1_OFF\r\n");
	}
}


/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* LED��ʼ�� */
	LED_GPIO_Config();
	
	/* ����Ӳ���Ƿ����� */
	//LED1_ON;
	
	/* ����Ӳ����ʼ���Ͳ��� */
	
	
	/* ���ڳ�ʼ�� */
	USART_Config();
}


/**
  **********************************************************************
  * @brief  ��ȡ��������������ջ��������ƿ��ڴ�
	*					ppxTimerTaskTCBBuffer	:		������ƿ��ڴ�
	*					ppxTimerTaskStackBuffer	:	�����ջ�ڴ�
	*					pulTimerTaskStackSize	:		�����ջ��С
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  **********************************************************************
  */ 
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
																	 StackType_t **ppxIdleTaskStackBuffer,
																	 uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &Idle_Task_TCB;
	*ppxIdleTaskStackBuffer = Idle_Task_Stack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


/**
  *********************************************************************
  * @brief  ��ȡ��ʱ������������ջ��������ƿ��ڴ�
	*					ppxTimerTaskTCBBuffer	:		������ƿ��ڴ�
	*					ppxTimerTaskStackBuffer	:	�����ջ�ڴ�
	*					pulTimerTaskStackSize	:		�����ջ��С
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  **********************************************************************
  */ 
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
																  	StackType_t **ppxTimerTaskStackBuffer,
																	  uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer = &Timer_Task_TCB;
	*ppxTimerTaskStackBuffer = Timer_Task_Stack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}


/*********************************************END OF FILE**********************/

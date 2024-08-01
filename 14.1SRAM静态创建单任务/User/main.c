/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "bsp_led.h"
#include "FreeRTOS.h"
#include "task.h"


/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
 /* 创建任务句柄 */
 static TaskHandle_t AppTaskCreate_Handle;
 /* LED任务句柄 */
 static TaskHandle_t LED_Task_Handle;

/* AppTaskCreate任务堆栈 */
static StackType_t AppTaskCreate_Stack[128];
/* LED任务堆栈 */
static StackType_t LED_Task_Stack[128];

/* AppTaskCreate任务控制块 */
static StaticTask_t AppTaskCreate_TCB;
/* LED任务控制块 */
static StaticTask_t LED_Task_TCB;


/* 空闲任务堆栈 */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* 定时器任务堆栈 */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/* 空闲任务控制块 */
static StaticTask_t Idle_Task_TCB;
/* 定时器任务控制块 */
static StaticTask_t Timer_Task_TCB;



/* 函数声明 */
static void BSP_Init(void);
static void LED_Task(void *paramter);
static void AppTaskCreate(void);

/**
	* 使用了静态分配内存，以下这两个函数是由用户实现，函数在task.c文件中有引用
	*	当且仅当 configSUPPORT_STATIC_ALLOCATION 这个宏定义为 1 的时候才有效
	*/
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
																	StackType_t **ppxIdleTaskStackBuffer,
																	uint32_t *pulIdleTaskStackSize);
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
																	StackType_t **ppxTimerTaskStackBuffer,
																	uint32_t *pulTimerTaskStackSize);




/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* 开发板硬件初始化 */
	BSP_Init();
	printf("这是一个FreeRTOS-静态任务创建单任务！\r\n");
	AppTaskCreate_Handle = xTaskCreateStatic((TaskFunction_t) AppTaskCreate,
																			(const char*)"AppTaskCreate",
																			(uint32_t)128,
																			(void*)NULL,
																			(UBaseType_t)3,
																			(StackType_t*)AppTaskCreate_Stack,
																			(StaticTask_t*)&AppTaskCreate_TCB);
																			
	if(NULL != AppTaskCreate_Handle)  /* 创建成功 */
		vTaskStartScheduler();   /* 启动任务，开启调度 */
	
	while(1);
}

/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
	taskENTER_CRITICAL();    //进入临界区
	
	/* 创建LED_Task任务 */
	LED_Task_Handle = xTaskCreateStatic((TaskFunction_t) LED_Task,
																			(const char*)"LED_Task",
																			(uint32_t)128,
																			(void*)NULL,
																			(UBaseType_t)4,
																			(StackType_t*)LED_Task_Stack,
																			(StaticTask_t*)&LED_Task_TCB);
																			
	if(NULL != LED_Task_Handle) /* 创建任务成功 */
	{
		printf("LED_Task任务创建成功");
	}
	else
	{
		printf("LED_Task任务创建失败");
	}
	
	vTaskDelete(AppTaskCreate_Handle);   //删除AppTaskCreate任务
	
	taskEXIT_CRITICAL();  //退出临界区
}


/**********************************************************************
  * @ 函数名  ： LED_Task
  * @ 功能说明： LED_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void LED_Task(void *paramter)
{
	while(1)
	{
		LED1_ON;
		vTaskDelay(500);    /* 延时500ticks */
		printf("LED_Task Running ,LED1_ON\r\n");
		
		LED1_OFF;
		vTaskDelay(500);    /* 延时500ticks */
		printf("LED_Task Running ,LED1_OFF\r\n");
	}
}


/***********************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* LED初始化 */
	LED_GPIO_Config();
	
	/* 测试硬件是否正常 */
	//LED1_ON;
	
	/* 其他硬件初始化和测试 */
	
	
	/* 串口初始化 */
	USART_Config();
}


/**
  **********************************************************************
  * @brief  获取空闲任务的任务堆栈和任务控制块内存
	*					ppxTimerTaskTCBBuffer	:		任务控制块内存
	*					ppxTimerTaskStackBuffer	:	任务堆栈内存
	*					pulTimerTaskStackSize	:		任务堆栈大小
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
  * @brief  获取定时器任务的任务堆栈和任务控制块内存
	*					ppxTimerTaskTCBBuffer	:		任务控制块内存
	*					ppxTimerTaskStackBuffer	:	任务堆栈内存
	*					pulTimerTaskStackSize	:		任务堆栈大小
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

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
//FreeRTOS头文件
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
	
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"


/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
 /* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LowPriority任务句柄 */
static TaskHandle_t LowPriority_Task_Handle = NULL;
/* MidPriority任务句柄 */
static TaskHandle_t MidPriority_Task_Handle = NULL;
/* HighPriority任务句柄 */
static TaskHandle_t HighPriority_Task_Handle = NULL;


/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */
SemaphoreHandle_t BinarySem_Handle = NULL;

/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
 



/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void BSP_Init(void);
static void LowPriority_Task(void* parameter);
static void MidPriority_Task(void* parameter);
static void HighPriority_Task(void* parameter);
static void AppTaskCreate(void);/* 用于创建任务 */





/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	BaseType_t xReturn = pdPASS;
	
	/* 开发板硬件初始化 */
	BSP_Init();
	printf("这是一个FreeRTOS-优先级翻转任务！\r\n");
	
	xReturn = xTaskCreate((TaskFunction_t) AppTaskCreate,
												(const char*)"AppTaskCreate",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)1,
												(TaskHandle_t*)&AppTaskCreate_Handle);
																			
	if(pdPASS == xReturn)  /* 创建成功 */
		vTaskStartScheduler();   /* 启动任务，开启调度 */
	else
		return -1;
	
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
	BaseType_t xReturn = pdPASS;  //定义一个创建信息返回值，默认为pdPASS
	
	taskENTER_CRITICAL();    //进入临界区
	
	/* 创建BinarySem_Handle */
	BinarySem_Handle = xSemaphoreCreateBinary();
	
	if(NULL != BinarySem_Handle)
		printf("创建BinarySem_Handle计数信号量创建成功！\r\n");
	
	xReturn = xSemaphoreGive(BinarySem_Handle); //给出二值信号量
	
	/* 创建LowPriority任务 */
	xReturn = xTaskCreate((TaskFunction_t) LowPriority_Task,
												(const char*)"LowPriority_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)2,
												(TaskHandle_t*)&LowPriority_Task_Handle); // 任务控制块指针
																			
	if(pdPASS == xReturn) /* 创建任务成功 */
		printf("创建LowPriority_Task成功\r\n");
	
	/* 创建LED1_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t) MidPriority_Task,
												(const char*)"MidPriority_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)3,
												(TaskHandle_t*)MidPriority_Task_Handle); // 任务控制块指针
																			
	if(pdPASS == xReturn) /* 创建任务成功 */
		printf("MidPriority_Task任务创建成功");
	
	/* 创建LED1_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t) HighPriority_Task,
												(const char*)"HighPriority_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)4,
												(TaskHandle_t*)HighPriority_Task_Handle); // 任务控制块指针
																			
	if(pdPASS == xReturn) /* 创建任务成功 */
		printf("HighPriority_Task任务创建成功");
	
	vTaskDelete(AppTaskCreate_Handle);   //删除AppTaskCreate任务
	
	taskEXIT_CRITICAL();  //退出临界区
}


/**********************************************************************
  * @ 函数名  ： LowPriority_Task
  * @ 功能说明： LowPriority_Task 任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void LowPriority_Task(void* parameter)
{
	static uint32_t i;
	BaseType_t xReturn = pdPASS ;   /* 定义一个创建信息返回值，默认为pdTRUE */

	
	while(1)
	{
		printf("LowPriority_Task 获取信号量\n");
		
		//获取二值信号量xSemphore，没获得则一直等待
		xReturn = xSemaphoreTake(BinarySem_Handle,portMAX_DELAY);
		
		if(xReturn == pdTRUE)
			printf("LowPriority_Task Running \n");
		
		for( i = 0;i<2000000;i++)
		{
			taskYIELD();  //发起任务调度
		}
		
		printf("LowPriority_task 释放信号量\r\n");
		xReturn = xSemaphoreGive(BinarySem_Handle);
		
		LED1_TOGGLE;
		
			
		vTaskDelay(500);  //延时20个tick
	}
}


/**********************************************************************
  * @ 函数名  ： LowPriority_Task
  * @ 功能说明： LowPriority_Task 任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void MidPriority_Task(void* parameter)
{
	while(1)
	{
		printf("MidPriority_Task Running\n");
		vTaskDelay(500);
	}
}


/**********************************************************************
  * @ 函数名  ： HighPriority_Task
  * @ 功能说明： HighPriority_Task 任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void HighPriority_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE ;   /* 定义一个创建信息返回值，默认为pdTRUE */

	
	while(1)
	{
		printf("HighPriority_Task 获取信号量\n");
		
		//获取二值信号量xSemphore，没获得则一直等待
		xReturn = xSemaphoreTake(BinarySem_Handle,portMAX_DELAY);
		
		if(xReturn == pdTRUE)
			printf("HighPriority_Task Running \n");
		
		LED1_TOGGLE;
		
		xReturn = xSemaphoreGive(BinarySem_Handle);  //给出二值信号量
		
			
		vTaskDelay(500);  //延时20个tick
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
	
	/* 按键初始化 */
	Key_GPIO_Config();
}



/*********************************************END OF FILE**********************/

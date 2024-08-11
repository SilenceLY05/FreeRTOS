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
#include "event_groups.h"
	
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
/* LED_Task任务句柄 */
static TaskHandle_t LED_Task_Handle = NULL;
/* KEY_Task任务句柄 */
static TaskHandle_t KEY_Task_Handle = NULL;


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
static EventGroupHandle_t Event_Handle = NULL;

/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
 
 
 /******************************* 宏定义 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些宏定义。
 */
 #define KEY1_EVENT  (0x01 << 0)  //设置事件掩码的位0
 #define KEY2_EVENT  (0x01 << 1)  //设置事件掩码的位1


/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void BSP_Init(void);
static void LED_Task(void* parameter);
static void KEY_Task(void* parameter);
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
	printf("这是一个FreeRTOS-事件标志组实验！\r\n");
	
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
	Event_Handle = xEventGroupCreate();
	
	if(NULL != Event_Handle)
		printf("创建Event_Handle事件量创建成功！\r\n");

	
	/* 创建LowPriority任务 */
	xReturn = xTaskCreate((TaskFunction_t) LED_Task,
												(const char*)"LED_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)2,
												(TaskHandle_t*)&LED_Task_Handle); // 任务控制块指针
																			
	if(pdPASS == xReturn) /* 创建任务成功 */
		printf("创建LED_Task成功\r\n");
	
	/* 创建LowPriority任务 */
	xReturn = xTaskCreate((TaskFunction_t) KEY_Task,
												(const char*)"KEY_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)3,
												(TaskHandle_t*)&KEY_Task_Handle); // 任务控制块指针
																			
	if(pdPASS == xReturn) /* 创建任务成功 */
		printf("创建KEY_Task成功\r\n");
	
	
	
	vTaskDelete(AppTaskCreate_Handle);   //删除AppTaskCreate任务
	
	taskEXIT_CRITICAL();  //退出临界区
}

/**********************************************************************
  * @ 函数名  ： LED_Task
  * @ 功能说明： LED_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void LED_Task(void* parameter)
{
	EventBits_t r_event;  //定义一个事件接收变量
	
	/* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		/*******************************************************************
     * 等待接收事件标志 
     * 
     * 如果xClearOnExit设置为pdTRUE，那么在xEventGroupWaitBits()返回之前，
     * 如果满足等待条件（如果函数返回的原因不是超时），那么在事件组中设置
     * 的uxBitsToWaitFor中的任何位都将被清除。 
     * 如果xClearOnExit设置为pdFALSE，
     * 则在调用xEventGroupWaitBits()时，不会更改事件组中设置的位。
     *
     * xWaitForAllBits如果xWaitForAllBits设置为pdTRUE，则当uxBitsToWaitFor中
     * 的所有位都设置或指定的块时间到期时，xEventGroupWaitBits()才返回。 
     * 如果xWaitForAllBits设置为pdFALSE，则当设置uxBitsToWaitFor中设置的任何
     * 一个位置1 或指定的块时间到期时，xEventGroupWaitBits()都会返回。 
     * 阻塞时间由xTicksToWait参数指定。          
      *********************************************************/
		r_event = xEventGroupWaitBits(Event_Handle,                     //事件对象句柄
																  KEY1_EVENT | KEY2_EVENT,          //接收线程感兴趣的事件
																	pdTRUE,                           //退出时清除事件位
																	pdTRUE,                           //满足感兴趣的所有事件
																	portMAX_DELAY);                   //指定超时时间，一直等待
		
		if((r_event & (KEY1_EVENT | KEY2_EVENT)) == (KEY1_EVENT | KEY2_EVENT))
		{
			//如果接收完成并且正确
			printf("KEY1和KEY2都按下\n");
			LED1_TOGGLE;
		}
		else
			printf("事件错误\n");
	}
}


/**********************************************************************
  * @ 函数名  ： KEY_Task
  * @ 功能说明： KEY_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void KEY_Task(void* parameter)
{
	/* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			printf("KEY1被按下\n");
			/* 触发一个事件1 */
			xEventGroupSetBits(Event_Handle,KEY1_EVENT);
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			printf("KEY2被按下\n");
			/* 触发一个事件1 */
			xEventGroupSetBits(Event_Handle,KEY2_EVENT);
		}
		
		vTaskDelay(20);   //每20ms扫描一次
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

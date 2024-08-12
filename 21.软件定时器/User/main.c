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
static TimerHandle_t Swtmr1_Handle = NULL;
static TimerHandle_t Swtmr2_Handle = NULL;

/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
static uint32_t TmrCb_Count1 = 0;   /* 记录软件定时器1回调函数执行次数 */
static uint32_t TmrCb_Count2 = 0;   /* 记录软件定时器2回调函数执行次数 */
 
 
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
static void Swtmr1_Callback(void* parameter);
static void Swtmr2_Callback(void* parameter);
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
	printf("这是一个FreeRTOS-软件定时器实验！\r\n");
	
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
	
	taskENTER_CRITICAL();    //进入临界区
	
	/************************************************************************************
   * 创建软件周期定时器
   * 函数原型
   * TimerHandle_t xTimerCreate(	const char * const pcTimerName,
								const TickType_t xTimerPeriodInTicks,
								const UBaseType_t uxAutoReload,
								void * const pvTimerID,
                TimerCallbackFunction_t pxCallbackFunction )
    * @uxAutoReload : pdTRUE为周期模式，pdFALS为单次模式
   * 单次定时器，周期(1000个时钟节拍)，周期模式
   *************************************************************************************/

	
	/* 创建LowPriority任务 */
	Swtmr1_Handle = xTimerCreate((const char*) "AutoReloadTimer",
												(TickType_t)1000,/* 定时器周期 1000(tick) */
												(UBaseType_t)pdTRUE,/* 周期模式 */
												(void*)1,/* 为每个计时器分配一个索引的唯一ID */
												(TimerCallbackFunction_t)Swtmr1_Callback); // 任务控制块指针
																			
	if(Swtmr1_Handle != NULL)
	{
		/***********************************************************************************
     * xTicksToWait:如果在调用xTimerStart()时队列已满，则以tick为单位指定调用任务应保持
     * 在Blocked(阻塞)状态以等待start命令成功发送到timer命令队列的时间。 
     * 如果在启动调度程序之前调用xTimerStart()，则忽略xTicksToWait。在这里设置等待时间为0.
     **********************************************************************************/
		xTimerStart(Swtmr1_Handle,0);   //开启周期定时器
	}
	
	/************************************************************************************
   * 创建软件周期定时器
   * 函数原型
   * TimerHandle_t xTimerCreate(	const char * const pcTimerName,
								const TickType_t xTimerPeriodInTicks,
								const UBaseType_t uxAutoReload,
								void * const pvTimerID,
                TimerCallbackFunction_t pxCallbackFunction )
    * @uxAutoReload : pdTRUE为周期模式，pdFALS为单次模式
   * 单次定时器，周期(1000个时钟节拍)，周期模式
   *************************************************************************************/

	
	/* 创建LowPriority任务 */
	Swtmr2_Handle = xTimerCreate((const char*) "OneShotTimer",
												(TickType_t)5000,/* 定时器周期 5000(tick) */
												(UBaseType_t)pdFALSE,/* 单次模式 */
												(void*)2,/* 为每个计时器分配一个索引的唯一ID */
												(TimerCallbackFunction_t)Swtmr2_Callback); // 任务控制块指针
																			
	if(Swtmr2_Handle != NULL)
	{
		/***********************************************************************************
     * xTicksToWait:如果在调用xTimerStart()时队列已满，则以tick为单位指定调用任务应保持
     * 在Blocked(阻塞)状态以等待start命令成功发送到timer命令队列的时间。 
     * 如果在启动调度程序之前调用xTimerStart()，则忽略xTicksToWait。在这里设置等待时间为0.
     **********************************************************************************/
		xTimerStart(Swtmr2_Handle,0);   //开启周期定时器
	}
	
	
	vTaskDelete(AppTaskCreate_Handle);   //删除AppTaskCreate任务
	
	taskEXIT_CRITICAL();  //退出临界区
}

/***********************************************************************
  * @ 函数名  ： Swtmr1_Callback
  * @ 功能说明： 软件定时器1 回调函数，打印回调函数信息&当前系统时间
  *              软件定时器请不要调用阻塞函数，也不要进行死循环，应快进快出
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void Swtmr1_Callback(void* parameter)
{
	TickType_t tick_num1;
	
	TmrCb_Count1++;     /* 每回调一次加一 */
	
	tick_num1 = xTaskGetTickCount();   //获取滴答定时器的计数值
	
	printf("Swtmr1_Callback函数执行 %d 次 \n",TmrCb_Count1);
	printf("滴答定时器数值 = %d\n",tick_num1);
}


/***********************************************************************
  * @ 函数名  ： Swtmr2_Callback
  * @ 功能说明： 软件定时器2 回调函数，打印回调函数信息&当前系统时间
  *              软件定时器请不要调用阻塞函数，也不要进行死循环，应快进快出
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void Swtmr2_Callback(void* parameter)
{
	TickType_t tick_num2;
	
	TmrCb_Count2++;     /* 每回调一次加一 */
	
	tick_num2 = xTaskGetTickCount();   //获取滴答定时器的计数值
	
	printf("Swtmr2_Callback函数执行 %d 次 \n",TmrCb_Count2);
	printf("滴答定时器数值 = %d\n",tick_num2);
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

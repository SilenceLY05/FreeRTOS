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
#include "limits.h"


/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
 /* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t LED_Task_Handle = NULL;/* Take_Task任务句柄 */
static TaskHandle_t KEY_Task_Handle = NULL;/* Give_Task任务句柄 */



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


/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */

 
 
 /******************************* 宏定义 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些宏定义。
 */
#define KEY1_EVENT  (0x01 << 0)
#define KEY2_EVENT  (0x01 << 1)

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void BSP_Init(void);
static void AppTaskCreate(void);/* 用于创建任务 */
static void LED_Task(void* parameter);
static void KEY_Task(void* parameter);





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
	printf("这是一个[野火]-STM32全系列开发板-FreeRTOS任务通知代替计数信号量实验！\n");
  printf("车位默认值为0个，按下KEY1申请车位，按下KEY2释放车位！\n\n");
	
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
	BaseType_t xReturn = pdPASS;
	
	taskENTER_CRITICAL();    //进入临界区
	
	xReturn = xTaskCreate((TaskFunction_t)LED_Task,/* 任务入口函数 */
												(const char*)"LED_Task",/* 任务名字 */
												(uint16_t)512,/* 任务栈大小 */
												(void*)NULL,/* 任务入口函数参数 */
												(UBaseType_t)2,/* 任务的优先级 */
												(TaskHandle_t*)&LED_Task_Handle);/* 任务控制块指针 */
												
	if(xReturn == pdPASS)
		printf("创建LED_Task 任务成功!\r\n");
	
	xReturn = xTaskCreate((TaskFunction_t)KEY_Task,/* 任务入口函数 */
												(const char*)"KEY_Task",/* 任务名字 */
												(uint16_t)512,/* 任务栈大小 */
												(void*)NULL,/* 任务入口函数参数 */
												(UBaseType_t)3,/* 任务的优先级 */
												(TaskHandle_t*)&KEY_Task_Handle);/* 任务控制块指针 */
												
	if(xReturn == pdPASS)
		printf("创建KEY_Task 任务成功!\r\n");
	
	
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
	
	
}
	
/**********************************************************************
  * @ 函数名  ： Receive_Task
  * @ 功能说明： Receive_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/

static void LED_Task(void* parameter)
{
	uint32_t r_event = 0;
	uint32_t last_event = 0;
	BaseType_t xReturn = pdTRUE;
	
	while(1)
	{
		/* BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry, 
                                  uint32_t ulBitsToClearOnExit, 
                                  uint32_t *pulNotificationValue, 
                                  TickType_t xTicksToWait ); 
     * ulBitsToClearOnEntry：当没有接收到任务通知的时候将任务通知值与此参数的取
       反值进行按位与运算，当此参数为Oxfffff或者ULONG_MAX的时候就会将任务通知值清零。
     * ulBits ToClearOnExit：如果接收到了任务通知，在做完相应的处理退出函数之前将
       任务通知值与此参数的取反值进行按位与运算，当此参数为0xfffff或者ULONG MAX的时候
       就会将任务通知值清零。
     * pulNotification Value：此参数用来保存任务通知值。
     * xTick ToWait：阻塞时间。
     *
     * 返回值：pdTRUE：获取到了任务通知。pdFALSE：任务通知获取失败。
     */
    //获取任务通知 ,没获取到则一直等待
		xReturn = xTaskNotifyWait(0x0,ULONG_MAX,&r_event,portMAX_DELAY);
		
		if(pdTRUE == xReturn)
		{
			last_event |= r_event;
			
			/* 如果接收完成并且正确 */
			if(last_event == (KEY1_EVENT | KEY2_EVENT))
			{
				last_event = 0;
				printf("KEY1与KEY2都按下\r\n");
				LED1_TOGGLE;
			}
			else
				last_event = r_event;
			
		}
		
	}
}

/**********************************************************************
  * @ 函数名  ： Receive_Task
  * @ 功能说明： Receive_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void KEY_Task(void* parameter)
{

	while(1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			printf("KEY1被按下\n");
			
			xTaskNotify((TaskHandle_t)LED_Task_Handle,(uint32_t)KEY1_EVENT,(eNotifyAction)eSetBits);
			
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			printf("KEY2被按下\n");
			
			xTaskNotify((TaskHandle_t)LED_Task_Handle,(uint32_t)KEY2_EVENT,(eNotifyAction)eSetBits);
			
		}
		vTaskDelay(20);
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

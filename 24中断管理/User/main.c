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
#include "bsp_exti.h"


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
QueueHandle_t Test_Queue = NULL;
SemaphoreHandle_t BinarySem_Handle = NULL;

/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
extern char Usart_Rx_Buf[USART_RBUFF_SIZE];
 
 
 /******************************* 宏定义 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些宏定义。
 */
 #define QUEUE_LEN     4
 #define QUEUE_SIZE    4

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
	printf("这是一个[野火]-STM32全系列开发板-FreeRTOS内存管理实验\n");
  printf("按下KEY1申请内存，按下KEY2释放内存\n");
	
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
	
	Test_Queue = xQueueCreate((UBaseType_t)QUEUE_LEN,(UBaseType_t)QUEUE_SIZE);
	
	if(Test_Queue!= NULL)
		printf("Test_Queue消息队列创建成功！\n");
	
	BinarySem_Handle = xSemaphoreCreateBinary();
	
	if(BinarySem_Handle != NULL)
		printf("BinarySem_Handle 二值信号量创建成功！\n");
	
	xReturn = xTaskCreate((TaskFunction_t)LED_Task,/* 任务入口函数 */
												(const char*)"LED_Task",/* 任务名字 */
												(uint16_t)512,/* 任务栈大小 */
												(void*)NULL,/* 任务入口函数参数 */
												(UBaseType_t)2,/* 任务的优先级 */
												(TaskHandle_t*)&LED_Task_Handle);/* 任务控制块指针 */
												
	if(xReturn == pdPASS)
		printf("创建LED_Task 任务成功!\r\n");
	
	xReturn = xTaskCreate((TaskFunction_t)KEY_Task,/* 任务入口函数 */
												(const char*)"Test_Task",/* 任务名字 */
												(uint16_t)512,/* 任务栈大小 */
												(void*)NULL,/* 任务入口函数参数 */
												(UBaseType_t)3,/* 任务的优先级 */
												(TaskHandle_t*)&KEY_Task_Handle);/* 任务控制块指针 */
												
	if(xReturn == pdPASS)
		printf("创建Test_Task 任务成功!\r\n");
	
	
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
	BaseType_t xReturn = pdPASS;
	uint32_t r_queue;
	while(1)
	{
		xReturn = xQueueReceive(Test_Queue,&r_queue,portMAX_DELAY);
		
		if(xReturn == pdPASS)
		{
			printf("触发中断的是 KEY %d\n",r_queue);
		}
		else
		{
			printf("数据接收出错\n");
		}
		LED1_TOGGLE;
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
	BaseType_t xReturn = pdPASS;  
	
	while(1)
	{
		//获取二值信号量 xSemaphore，没获取到则一直等待
		xReturn = xSemaphoreTake(BinarySem_Handle,portMAX_DELAY);
		
		if(xReturn == pdPASS)
		{
			LED2_TOGGLE;
			printf("收到数据：%s \n",Usart_Rx_Buf);
			memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);
		}
		
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
	USARTx_DMA_Config();
	
	
	/* 串口初始化 */
	USART_Config();
	
	/* 按键初始化 */
	Key_GPIO_Config();
	
	EXTI_Key_Config();
}



/*********************************************END OF FILE**********************/

#include "FreeRTOS.h"
#include "task.h"

//定义全局变量
portCHAR flag1;
portCHAR flag2;
portCHAR flag3;

extern List_t pxReadyTasksLists[configMAX_PRIORITIES];

/*
*************************************************************************
*                        任务控制块 & STACK 
*************************************************************************
*/
TaskHandle_t Task1_Handle;
#define TASK1_STACK_SIZE            128
StackType_t  Task1Stack[TASK1_STACK_SIZE];
TCB_t Task1TCB;

TaskHandle_t Task2_Handle;
#define TASK2_STACK_SIZE            128
StackType_t  Task2Stack[TASK2_STACK_SIZE];
TCB_t Task2TCB;

TaskHandle_t Task3_Handle;
#define TASK3_STACK_SIZE            128
StackType_t  Task3Stack[TASK3_STACK_SIZE];
TCB_t Task3TCB;




void Delay(uint32_t count);
void Task1_Entry(void* p_arg);
void Task2_Entry(void* p_arg);
void Task3_Entry(void* p_arg);


/*
* 注意事项：1、该工程使用软件仿真，debug需选择 Ude Simulator
*           2、在Target选项卡里面把晶振Xtal(Mhz)的值改为25，默认是12，
*              改成25是为了跟system_ARMCM3.c中定义的__SYSTEM_CLOCK相同，确保仿真的时候时钟一致
*/


int main(void)
{
	/* 硬件初始化，将硬件相关的初始化放在这里，如果是软件仿真则没有相关初始化代码 */
	
	/* 初始化与任务相关的列表，如就绪列表 */
	//prvInitialiseTaskLists();
	
	/* 创建任务 */
	Task1_Handle = xTaskCreateStatic((TaskFunction_t)Task1_Entry,
																	 (char*)"Task1",
																	 (uint32_t)TASK1_STACK_SIZE,
																	 (void*)NULL,
																	 (UBaseType_t)2,
																	 (StackType_t*)Task1Stack,
																	 (TCB_t*)&Task1TCB);
	
	/* 将任务添加到就绪列表 */
	//vListInsertEnd(&(pxReadyTasksLists[1]),&(((TCB_t*)(&Task1TCB))->xStateListItem));
	
	/* 创建任务 */
	Task2_Handle = xTaskCreateStatic((TaskFunction_t)Task2_Entry,
																	 (char*)"Task2",
																	 (uint32_t)TASK2_STACK_SIZE,
																	 (void*)NULL,
																	 (UBaseType_t)2,
																	 (StackType_t*)Task2Stack,
																	 (TCB_t*)&Task2TCB);
																	 
																	 
	Task3_Handle = xTaskCreateStatic((TaskFunction_t)Task3_Entry,
																	 (char*)"Task3",
																	 (uint32_t)TASK3_STACK_SIZE,
																	 (void*)NULL,
																	 (UBaseType_t)3,
																	 (StackType_t*)Task3Stack,
																	 (TCB_t*)&Task3TCB);
	
	/* 将任务添加到就绪列表 */
	//vListInsertEnd(&(pxReadyTasksLists[2]),&(((TCB_t*)(&Task2TCB))->xStateListItem));
																	 
	portDISABLE_INTERRUPTS();
	
	/* 开启调度器，开始多任务调度，成功则不返回 */
	vTaskStartScheduler();
	
	for(;;)
	{}
}


void Delay(uint32_t count)
{
	for(;count != 0;count --)
	{}
}


//定义一个任务函数 
void Task1_Entry(void* p_arg)
{
	for(;;)
	{
		flag1 = 1;
			Delay(100);
		flag1 = 0;
			Delay(100);
	}
}

void Task2_Entry(void* p_arg)
{
	for(;;)
	{
		flag2 = 1;
			Delay(100);
		flag2 = 0;
			Delay(100);
	}
}

void Task3_Entry(void* p_arg)
{
	for(;;)
	{
		flag3 = 1;
		vTaskDelay( 1 );
		//Delay(100);
		flag3 = 0;
		vTaskDelay( 1 );
		//Delay(100);
	}
}


/* 获取空闲任务的内存 */
StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
TCB_t IdleTaskTCB;

void vApplicationGetIdleTaskMemory(TCB_t **ppxIdleTaskTCBBuffer,
																	 StackType_t **ppxIdleTaskStackBuffer,
															     uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &IdleTaskTCB;
	*ppxIdleTaskStackBuffer = IdleTaskStack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


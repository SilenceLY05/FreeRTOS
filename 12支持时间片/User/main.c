#include "FreeRTOS.h"
#include "task.h"

//����ȫ�ֱ���
portCHAR flag1;
portCHAR flag2;
portCHAR flag3;

extern List_t pxReadyTasksLists[configMAX_PRIORITIES];

/*
*************************************************************************
*                        ������ƿ� & STACK 
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
* ע�����1���ù���ʹ��������棬debug��ѡ�� Ude Simulator
*           2����Targetѡ�����Ѿ���Xtal(Mhz)��ֵ��Ϊ25��Ĭ����12��
*              �ĳ�25��Ϊ�˸�system_ARMCM3.c�ж����__SYSTEM_CLOCK��ͬ��ȷ�������ʱ��ʱ��һ��
*/


int main(void)
{
	/* Ӳ����ʼ������Ӳ����صĳ�ʼ�����������������������û����س�ʼ������ */
	
	/* ��ʼ����������ص��б�������б� */
	//prvInitialiseTaskLists();
	
	/* �������� */
	Task1_Handle = xTaskCreateStatic((TaskFunction_t)Task1_Entry,
																	 (char*)"Task1",
																	 (uint32_t)TASK1_STACK_SIZE,
																	 (void*)NULL,
																	 (UBaseType_t)2,
																	 (StackType_t*)Task1Stack,
																	 (TCB_t*)&Task1TCB);
	
	/* ��������ӵ������б� */
	//vListInsertEnd(&(pxReadyTasksLists[1]),&(((TCB_t*)(&Task1TCB))->xStateListItem));
	
	/* �������� */
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
	
	/* ��������ӵ������б� */
	//vListInsertEnd(&(pxReadyTasksLists[2]),&(((TCB_t*)(&Task2TCB))->xStateListItem));
																	 
	portDISABLE_INTERRUPTS();
	
	/* ��������������ʼ��������ȣ��ɹ��򲻷��� */
	vTaskStartScheduler();
	
	for(;;)
	{}
}


void Delay(uint32_t count)
{
	for(;count != 0;count --)
	{}
}


//����һ�������� 
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


/* ��ȡ����������ڴ� */
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


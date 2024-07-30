#include "FreeRTOS.h"
#include "task.h"

//����ȫ�ֱ���
portCHAR flag1;
portCHAR flag2;

extern List_t pxReadyTasksLists[configMAX_PRIORITIES];

/*
*************************************************************************
*                        ������ƿ� & STACK 
*************************************************************************
*/
TaskHandle_t Task1_Handle;
#define TASK1_STACK_SIZE            128
StackType_t  Task1Stack[TASK1_STACK_SIZE];

TaskHandle_t Task2_Handle;
#define TASK2_STACK_SIZE            128
StackType_t  Task2Stack[TASK2_STACK_SIZE];


//����������ƿ�
TCB_t Task1TCB;
TCB_t Task2TCB;



void Task2_Entry(void* p_arg);
void Task1_Entry(void* p_arg);
void Delay(uint32_t count);


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
																	 (UBaseType_t)1,
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
	
	/* ��������ӵ������б� */
	//vListInsertEnd(&(pxReadyTasksLists[2]),&(((TCB_t*)(&Task2TCB))->xStateListItem));
	
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
			vTaskDelay(2);
		flag1 = 0;
			vTaskDelay(2);
	}
}

void Task2_Entry(void* p_arg)
{
	for(;;)
	{
		flag2 = 1;
			vTaskDelay(2);
		flag2 = 0;
			vTaskDelay(2);
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


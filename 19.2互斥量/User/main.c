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
//FreeRTOSͷ�ļ�
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
	
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"


/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LowPriority������ */
static TaskHandle_t LowPriority_Task_Handle = NULL;
/* MidPriority������ */
static TaskHandle_t MidPriority_Task_Handle = NULL;
/* HighPriority������ */
static TaskHandle_t HighPriority_Task_Handle = NULL;


/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */
SemaphoreHandle_t MuxSem_Handle = NULL;

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
 



/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void BSP_Init(void);
static void LowPriority_Task(void* parameter);
static void MidPriority_Task(void* parameter);
static void HighPriority_Task(void* parameter);
static void AppTaskCreate(void);/* ���ڴ������� */





/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	BaseType_t xReturn = pdPASS;
	
	/* ������Ӳ����ʼ�� */
	BSP_Init();
	printf("����һ��FreeRTOS-����������\r\n");
	
	xReturn = xTaskCreate((TaskFunction_t) AppTaskCreate,
												(const char*)"AppTaskCreate",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)1,
												(TaskHandle_t*)&AppTaskCreate_Handle);
																			
	if(pdPASS == xReturn)  /* �����ɹ� */
		vTaskStartScheduler();   /* �������񣬿������� */
	else
		return -1;
	
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
	BaseType_t xReturn = pdPASS;  //����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS
	
	taskENTER_CRITICAL();    //�����ٽ���
	
	/* ����BinarySem_Handle */
	MuxSem_Handle = xSemaphoreCreateMutex();
	
	if(NULL != MuxSem_Handle)
		printf("����BMuxSem_Handle�����������ɹ���\r\n");
	
	xReturn = xSemaphoreGive(MuxSem_Handle); //������ֵ�ź���
	
	/* ����LowPriority���� */
	xReturn = xTaskCreate((TaskFunction_t) LowPriority_Task,
												(const char*)"LowPriority_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)2,
												(TaskHandle_t*)&LowPriority_Task_Handle); // ������ƿ�ָ��
																			
	if(pdPASS == xReturn) /* ��������ɹ� */
		printf("����LowPriority_Task�ɹ�\r\n");
	
	/* ����LED1_Task���� */
	xReturn = xTaskCreate((TaskFunction_t) MidPriority_Task,
												(const char*)"MidPriority_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)3,
												(TaskHandle_t*)MidPriority_Task_Handle); // ������ƿ�ָ��
																			
	if(pdPASS == xReturn) /* ��������ɹ� */
		printf("MidPriority_Task���񴴽��ɹ�");
	
	/* ����LED1_Task���� */
	xReturn = xTaskCreate((TaskFunction_t) HighPriority_Task,
												(const char*)"HighPriority_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)4,
												(TaskHandle_t*)HighPriority_Task_Handle); // ������ƿ�ָ��
																			
	if(pdPASS == xReturn) /* ��������ɹ� */
		printf("HighPriority_Task���񴴽��ɹ�");
	
	vTaskDelete(AppTaskCreate_Handle);   //ɾ��AppTaskCreate����
	
	taskEXIT_CRITICAL();  //�˳��ٽ���
}


/**********************************************************************
  * @ ������  �� LowPriority_Task
  * @ ����˵���� LowPriority_Task ��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void LowPriority_Task(void* parameter)
{
	static uint32_t i;
	BaseType_t xReturn = pdPASS ;   /* ����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE */

	
	while(1)
	{
		printf("LowPriority_Task ��ȡ������\n");
		
		//��ȡ��ֵ�ź���xSemphore��û�����һֱ�ȴ�
		xReturn = xSemaphoreTake(MuxSem_Handle,portMAX_DELAY);
		
		if(xReturn == pdTRUE)
			printf("LowPriority_Task Running \n");
		
		for( i = 0;i<2000000;i++)
		{
			taskYIELD();  //�����������
		}
		
		printf("LowPriority_task �ͷŻ�����\r\n");
		xReturn = xSemaphoreGive(MuxSem_Handle);
		
		LED1_TOGGLE;
		
			
		vTaskDelay(500);  //��ʱ20��tick
	}
}


/**********************************************************************
  * @ ������  �� LowPriority_Task
  * @ ����˵���� LowPriority_Task ��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void MidPriority_Task(void* parameter)
{
	while(1)
	{
		printf("MidPriority_Task Running\n");
		vTaskDelay(1000);
	}
}


/**********************************************************************
  * @ ������  �� HighPriority_Task
  * @ ����˵���� HighPriority_Task ��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void HighPriority_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE ;   /* ����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE */

	
	while(1)
	{
		printf("HighPriority_Task ��ȡ������\n");
		
		//��ȡ��ֵ�ź���xSemphore��û�����һֱ�ȴ�
		xReturn = xSemaphoreTake(MuxSem_Handle,portMAX_DELAY);
		
		if(xReturn == pdTRUE)
			printf("HighPriority_Task Running \n");
		
		LED1_TOGGLE;
		
		printf("HighPriority_Task�ͷŻ�����!\r\n");
		xReturn = xSemaphoreGive(MuxSem_Handle);  //������ֵ�ź���
		
			
		vTaskDelay(1000);  //��ʱ20��tick
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
	
	/* ������ʼ�� */
	Key_GPIO_Config();
}



/*********************************************END OF FILE**********************/

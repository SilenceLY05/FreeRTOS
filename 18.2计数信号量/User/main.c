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
/* LED1������ */
static TaskHandle_t Take_Task_Handle = NULL;
/* LED2������ */
static TaskHandle_t Give_Task_Handle = NULL;

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
SemaphoreHandle_t CountSem_Handle = NULL;

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
static void Take_Task(void *paramter);
static void Give_Task(void* parameter);
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
	printf("����һ��FreeRTOS-��̬���񴴽�������\r\n");
	printf("��λĬ����5��������KEY1���복λ������KEY2�ͷų�λ��\r\n");
	
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
	CountSem_Handle = xSemaphoreCreateCounting(5,5);
	
	if(NULL != CountSem_Handle)
		printf("����CountSem_Handle�����ź��������ɹ���\r\n");

	
	/* ����LED1_Task���� */
	xReturn = xTaskCreate((TaskFunction_t) Take_Task,
												(const char*)"Task_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)2,
												(TaskHandle_t*)&Take_Task_Handle); // ������ƿ�ָ��
																			
	if(pdPASS == xReturn) /* ��������ɹ� */
		printf("����Take_Task�ɹ�\r\n");
	
	/* ����LED1_Task���� */
	xReturn = xTaskCreate((TaskFunction_t) Give_Task,
												(const char*)"Give_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)3,
												(TaskHandle_t*)&Give_Task_Handle); // ������ƿ�ָ��
																			
	if(pdPASS == xReturn) /* ��������ɹ� */
		printf("Give_Task���񴴽��ɹ�");
	
	vTaskDelete(AppTaskCreate_Handle);   //ɾ��AppTaskCreate����
	
	taskEXIT_CRITICAL();  //�˳��ٽ���
}


/**********************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Take_Task(void *paramter)
{
	BaseType_t xReturn = pdTRUE;   //����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE
	
	while(1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			//��ȡһ�������ź���
			xReturn = xSemaphoreGive(CountSem_Handle);  //������ֵ�ź���
			
			if(pdPASS == xReturn)
			{
				printf("KEY1�����£��ɹ����뵽һ��ͣ��λ!\n\n");
			}
			else
			{
				printf("KEY1�����£�������˼������ͣ��������!\n\n");
			}
		}
		vTaskDelay(20);     //ÿ20msɨ��һ��	
	}
	
	
}




/**********************************************************************
  * @ ������  �� Send_Task
  * @ ����˵���� Send_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Give_Task(void* parameter)
{
	BaseType_t xReturn = pdPASS;   /* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

	
	while(1)
	{
		
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			//��ȡһ�������ź���
			xReturn = xSemaphoreGive(CountSem_Handle);  //������ֵ�ź���
			if(pdPASS == xReturn)
			{
				printf("KEY2�����£��ͷ�1��ͣ��λ!\n\n");
			}
			else
			{
				printf("KEY2�����£������޳�λ�����ͷ�\n\n");
			}
			
			
		}
		vTaskDelay(20);  //��ʱ20��tick
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

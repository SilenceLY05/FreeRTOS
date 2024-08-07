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
static TaskHandle_t Receive_Task_Handle = NULL;
/* LED2������ */
static TaskHandle_t Send_Task_Handle = NULL;

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
SemaphoreHandle_t BinarySem_Handle = NULL;

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
 
 #define QUEUE_LEN   4     /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
 #define QUEUE_SIZE  4     /* ������ÿ����Ϣ��С���ֽڣ� */


/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void BSP_Init(void);
static void Receive_Task(void *paramter);
static void Send_Task(void* parameter);
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
	printf("����KEY1����KEY2����������������ͬ����\r\n");
	
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
	BinarySem_Handle = xSemaphoreCreateBinary();
	
	if(NULL != BinarySem_Handle)
		printf("����BinarySem_Handle��ֵ�ź��������ɹ���\r\n");

	
	/* ����LED1_Task���� */
	xReturn = xTaskCreate((TaskFunction_t) Receive_Task,
												(const char*)"Receive_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)2,
												(TaskHandle_t*)&Receive_Task_Handle); // ������ƿ�ָ��
																			
	if(pdPASS == xReturn) /* ��������ɹ� */
		printf("Receive_Task_Handle");
	
	/* ����LED1_Task���� */
	xReturn = xTaskCreate((TaskFunction_t) Send_Task,
												(const char*)"Send_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)3,
												(TaskHandle_t*)&Send_Task_Handle); // ������ƿ�ָ��
																			
	if(pdPASS == xReturn) /* ��������ɹ� */
		printf("KEY_Task���񴴽��ɹ�");
	
	vTaskDelete(AppTaskCreate_Handle);   //ɾ��AppTaskCreate����
	
	taskEXIT_CRITICAL();  //�˳��ٽ���
}


/**********************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Receive_Task(void *paramter)
{
	BaseType_t xReturn = pdTRUE;   //����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE
	
	while(1)
	{
		xReturn = xSemaphoreTake(BinarySem_Handle,portMAX_DELAY);   //��ֵ�ź������  �ȴ�ʱ��
		
		if(xReturn == pdTRUE)
			printf("BinarySem_Handle��ֵ�ź�����ȡ�ɹ���\r\n");
		
		LED1_TOGGLE;
	}
	
}




/**********************************************************************
  * @ ������  �� Send_Task
  * @ ����˵���� Send_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Send_Task(void* parameter)
{
	BaseType_t xReturn = pdPASS;   /* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

	
	while(1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			//KEY1����
			printf("������Ϣsend_data1��\n");
			
			xReturn = xSemaphoreGive(BinarySem_Handle);  //������ֵ�ź���
			
			if(pdPASS == xReturn)
			{
				printf("BinarySem_Handle��ֵ�ź����ͷųɹ�!\n\n");
			}
			else
			{
				printf("BinarySem_Handle��ֵ�ź����ͷ�ʧ��!\n\n");
			}
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			//KEY1����
			printf("������Ϣsend_data2��\n");
			
			xReturn = xSemaphoreGive(BinarySem_Handle);  //������ֵ�ź���
			if(pdPASS == xReturn)
			{
				printf("BinarySem_Handle��ֵ�ź����ͷųɹ�!\n\n");
			}
			else
			{
				printf("BinarySem_Handle��ֵ�ź����ͷ�ʧ��!\n\n");
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

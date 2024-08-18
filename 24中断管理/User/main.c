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
#include "bsp_exti.h"


/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t LED_Task_Handle = NULL;/* Take_Task������ */
static TaskHandle_t KEY_Task_Handle = NULL;/* Give_Task������ */



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
QueueHandle_t Test_Queue = NULL;
SemaphoreHandle_t BinarySem_Handle = NULL;

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
extern char Usart_Rx_Buf[USART_RBUFF_SIZE];
 
 
 /******************************* �궨�� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */
 #define QUEUE_LEN     4
 #define QUEUE_SIZE    4

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void BSP_Init(void);
static void AppTaskCreate(void);/* ���ڴ������� */
static void LED_Task(void* parameter);
static void KEY_Task(void* parameter);





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
	printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS�ڴ����ʵ��\n");
  printf("����KEY1�����ڴ棬����KEY2�ͷ��ڴ�\n");
	
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
	BaseType_t xReturn = pdPASS;
	
	taskENTER_CRITICAL();    //�����ٽ���
	
	Test_Queue = xQueueCreate((UBaseType_t)QUEUE_LEN,(UBaseType_t)QUEUE_SIZE);
	
	if(Test_Queue!= NULL)
		printf("Test_Queue��Ϣ���д����ɹ���\n");
	
	BinarySem_Handle = xSemaphoreCreateBinary();
	
	if(BinarySem_Handle != NULL)
		printf("BinarySem_Handle ��ֵ�ź��������ɹ���\n");
	
	xReturn = xTaskCreate((TaskFunction_t)LED_Task,/* ������ں��� */
												(const char*)"LED_Task",/* �������� */
												(uint16_t)512,/* ����ջ��С */
												(void*)NULL,/* ������ں������� */
												(UBaseType_t)2,/* ��������ȼ� */
												(TaskHandle_t*)&LED_Task_Handle);/* ������ƿ�ָ�� */
												
	if(xReturn == pdPASS)
		printf("����LED_Task ����ɹ�!\r\n");
	
	xReturn = xTaskCreate((TaskFunction_t)KEY_Task,/* ������ں��� */
												(const char*)"Test_Task",/* �������� */
												(uint16_t)512,/* ����ջ��С */
												(void*)NULL,/* ������ں������� */
												(UBaseType_t)3,/* ��������ȼ� */
												(TaskHandle_t*)&KEY_Task_Handle);/* ������ƿ�ָ�� */
												
	if(xReturn == pdPASS)
		printf("����Test_Task ����ɹ�!\r\n");
	
	
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
	
	
}
	
/**********************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
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
			printf("�����жϵ��� KEY %d\n",r_queue);
		}
		else
		{
			printf("���ݽ��ճ���\n");
		}
		LED1_TOGGLE;
	}
}

/**********************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void KEY_Task(void* parameter)
{
	BaseType_t xReturn = pdPASS;  
	
	while(1)
	{
		//��ȡ��ֵ�ź��� xSemaphore��û��ȡ����һֱ�ȴ�
		xReturn = xSemaphoreTake(BinarySem_Handle,portMAX_DELAY);
		
		if(xReturn == pdPASS)
		{
			LED2_TOGGLE;
			printf("�յ����ݣ�%s \n",Usart_Rx_Buf);
			memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);
		}
		
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
	USARTx_DMA_Config();
	
	
	/* ���ڳ�ʼ�� */
	USART_Config();
	
	/* ������ʼ�� */
	Key_GPIO_Config();
	
	EXTI_Key_Config();
}



/*********************************************END OF FILE**********************/

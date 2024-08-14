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
	
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "limits.h"


/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t Receive1_Task_Handle = NULL;/* Receive1_Task������ */
static TaskHandle_t Receive2_Task_Handle = NULL;/* Receive2_Task������ */
static TaskHandle_t Send_Task_Handle = NULL;/* Send_Task������ */


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


/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */

 
 
 /******************************* �궨�� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */


/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void BSP_Init(void);
static void AppTaskCreate(void);/* ���ڴ������� */
static void Receive1_Task(void* parameter);
static void Receive2_Task(void* parameter);
static void Send_Task(void *parameter);




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
	printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS����֪ͨ������Ϣ����ʵ�飡\n");
  printf("����KEY1����KEY2����������Ϣ֪ͨ���� \n");
	
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
	
	xReturn = xTaskCreate((TaskFunction_t)Receive1_Task,/* ������ں��� */
												(const char*)"Receive1_Task",/* �������� */
												(uint16_t)512,/* ����ջ��С */
												(void*)NULL,/* ������ں������� */
												(UBaseType_t)2,/* ��������ȼ� */
												(TaskHandle_t*)&Receive1_Task_Handle);/* ������ƿ�ָ�� */
												
	if(xReturn == pdPASS)
		printf("����Received1_Task ����ɹ�!\r\n");
	
	xReturn = xTaskCreate((TaskFunction_t)Receive2_Task,/* ������ں��� */
												(const char*)"Receive2_Task",/* �������� */
												(uint16_t)512,/* ����ջ��С */
												(void*)NULL,/* ������ں������� */
												(UBaseType_t)3,/* ��������ȼ� */
												(TaskHandle_t*)&Receive2_Task_Handle);/* ������ƿ�ָ�� */
												
	if(xReturn == pdPASS)
		printf("����Received2_Task ����ɹ�!\r\n");
	
	xReturn = xTaskCreate((TaskFunction_t)Send_Task,/* ������ں��� */
												(const char*)"Send_Task",/* �������� */
												(uint16_t)512,/* ����ջ��С */
												(void*)NULL,/* ������ں������� */
												(UBaseType_t)4,/* ��������ȼ� */
												(TaskHandle_t*)&Send_Task_Handle);/* ������ƿ�ָ�� */
												
	if(xReturn == pdPASS)
		printf("����Send_Task ����ɹ�!\r\n");
	
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
	
	
}
	
/**********************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/

static void Receive1_Task(void* parameter)
{
	
	while(1)
	{
		/* uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait ); 
     * xClearCountOnExit��pdTRUE ���˳�������ʱ����������ֵ֪ͨ���㣬���ƶ�ֵ�ź���
     * pdFALSE ���˳�����ulTaskNotifyTakeO��ʱ������ֵ֪ͨ��һ�����Ƽ������ź�����
     */
    //��ȡ����֪ͨ ,û��ȡ����һֱ�ȴ�
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);							

		printf("Receive1_Task ����֪ͨ��Ϣ�ɹ�");
		LED1_TOGGLE;
	}
}

/**********************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Receive2_Task(void* parameter)
{
	
	while(1)
	{
		/* uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait ); 
     * xClearCountOnExit��pdTRUE ���˳�������ʱ����������ֵ֪ͨ���㣬���ƶ�ֵ�ź���
     * pdFALSE ���˳�����ulTaskNotifyTakeO��ʱ������ֵ֪ͨ��һ�����Ƽ������ź�����
     */
    //��ȡ����֪ͨ ,û��ȡ����һֱ�ȴ�
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);							

		printf("Receive2_Task ����֪ͨ��Ϣ�ɹ�");
		
		LED2_TOGGLE;
	}
}


/**********************************************************************
  * @ ������  �� Send_Task
  * @ ����˵���� Send_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Send_Task(void *parameter)
{
	BaseType_t xReturn = pdPASS;  //����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS
	
	while(1)
	{
		/* KEY1������ */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			xReturn = xTaskNotifyGive(Receive1_Task_Handle);
			
			/* �˺���ֻ�᷵��pdPASS */
			if(xReturn == pdTRUE)
			{
				printf("Receive1_Task_Handle ����֪ͨ��Ϣ���ͳɹ�!\r\n");
			}
		}
		
		/* KEY2������ */
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			xReturn = xTaskNotifyGive(Receive2_Task_Handle);
			
			if(xReturn == pdPASS)
			{
				printf("Receive2_Task_Handle ����֪ͨ��Ϣ���ͳɹ�!\r\n");
			}
		}
		
		vTaskDelay(20);
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

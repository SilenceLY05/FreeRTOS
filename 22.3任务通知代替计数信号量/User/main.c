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
#include "limits.h"


/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t Take_Task_Handle = NULL;/* Take_Task������ */
static TaskHandle_t Give_Task_Handle = NULL;/* Give_Task������ */



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
static void Take_Task(void* parameter);
static void Give_Task(void* parameter);





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
	printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS����֪ͨ��������ź���ʵ�飡\n");
  printf("��λĬ��ֵΪ0��������KEY1���복λ������KEY2�ͷų�λ��\n\n");
	
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
	
	xReturn = xTaskCreate((TaskFunction_t)Take_Task,/* ������ں��� */
												(const char*)"Take_Task",/* �������� */
												(uint16_t)512,/* ����ջ��С */
												(void*)NULL,/* ������ں������� */
												(UBaseType_t)2,/* ��������ȼ� */
												(TaskHandle_t*)&Take_Task_Handle);/* ������ƿ�ָ�� */
												
	if(xReturn == pdPASS)
		printf("����Take_Task ����ɹ�!\r\n");
	
	xReturn = xTaskCreate((TaskFunction_t)Give_Task,/* ������ں��� */
												(const char*)"Give_Task",/* �������� */
												(uint16_t)512,/* ����ջ��С */
												(void*)NULL,/* ������ں������� */
												(UBaseType_t)3,/* ��������ȼ� */
												(TaskHandle_t*)&Give_Task_Handle);/* ������ƿ�ָ�� */
												
	if(xReturn == pdPASS)
		printf("����Give_Task ����ɹ�!\r\n");
	
	
	vTaskDelete(AppTaskCreate_Handle);
	taskEXIT_CRITICAL();
	
	
}
	
/**********************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/

static void Take_Task(void* parameter)
{
	uint32_t take_num = pdTRUE;
	
	while(1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			/* uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait ); 
       * xClearCountOnExit��pdTRUE ���˳�������ʱ����������ֵ֪ͨ���㣬���ƶ�ֵ�ź���
       * pdFALSE ���˳�����ulTaskNotifyTakeO��ʱ������ֵ֪ͨ��һ�����Ƽ������ź�����
       */
      //��ȡ����֪ͨ ,û��ȡ���򲻵ȴ�
			take_num = ulTaskNotifyTake(pdFALSE,0);
			
			if(take_num > 0)
				printf( "KEY1�����£��ɹ����뵽ͣ��λ����ǰ��λΪ %d \n", take_num - 1);
			else
        printf( "KEY1�����£���λ�Ѿ�û���ˣ��밴KEY2�ͷų�λ\n" ); 
		}
		vTaskDelay(20);
	}
}

/**********************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Give_Task(void* parameter)
{
	BaseType_t xReturn = pdPASS;
	while(1)
	{
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			/* ԭ��:BaseType_t xTaskNotifyGive( TaskHandle_t xTaskToNotify ); */
			/* �ͷ�һ������֪ͨ */
			xTaskNotifyGive(Take_Task_Handle);//��������֪ͨ
			
			if(xReturn == pdPASS)
				printf("KEY2�����£��ͷ�һ��ͣ��λ!\n");
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

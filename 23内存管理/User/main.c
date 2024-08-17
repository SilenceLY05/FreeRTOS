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
static TaskHandle_t LED_Task_Handle = NULL;/* Take_Task������ */
static TaskHandle_t Test_Task_Handle = NULL;/* Give_Task������ */



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
uint8_t *Test_Ptr = NULL;

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
static void LED_Task(void* parameter);
static void Test_Task(void* parameter);





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
	
	xReturn = xTaskCreate((TaskFunction_t)LED_Task,/* ������ں��� */
												(const char*)"LED_Task",/* �������� */
												(uint16_t)512,/* ����ջ��С */
												(void*)NULL,/* ������ں������� */
												(UBaseType_t)2,/* ��������ȼ� */
												(TaskHandle_t*)&LED_Task_Handle);/* ������ƿ�ָ�� */
												
	if(xReturn == pdPASS)
		printf("����LED_Task ����ɹ�!\r\n");
	
	xReturn = xTaskCreate((TaskFunction_t)Test_Task,/* ������ں��� */
												(const char*)"Test_Task",/* �������� */
												(uint16_t)512,/* ����ջ��С */
												(void*)NULL,/* ������ں������� */
												(UBaseType_t)3,/* ��������ȼ� */
												(TaskHandle_t*)&Test_Task_Handle);/* ������ƿ�ָ�� */
												
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
	while(1)
	{
		LED1_TOGGLE;
		vTaskDelay(1000);
	}
}

/**********************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Test_Task(void* parameter)
{
	uint32_t g_memsize;
	
	while(1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			if(Test_Ptr == NULL)
			{
				/* ��ȡ��ǰ�ڴ��С */
				g_memsize = xPortGetFreeHeapSize();
				printf("ϵͳ��ǰ�ڴ��СΪ %d �ֽڣ���ʼ�����ڴ�\n",g_memsize);
				Test_Ptr = pvPortMalloc(1024);
				if(NULL != Test_Ptr)
				{
					printf("�ڴ�����ɹ�\n");
					printf("���뵽���ڴ��ַΪ%#x\n",(int)Test_Ptr);
					
					/* ��ȡ��ǰʣ���ڴ��С */
					g_memsize = xPortGetFreeHeapSize();
					printf("��ǰ�ڴ�ʣ���СΪ %d �ֽ� \n",g_memsize);
					
					//��Test_Ptr��д�����ݣ���ǰϵͳʱ��
					sprintf((char*)Test_Ptr,"��ǰϵͳTickCount = %d \n",xTaskGetTickCount());
					printf("д��������� %s \n",(char*)Test_Ptr);
				}
			}
			else
			{
				printf("���Ȱ���KEY2�ͷ��ڴ�������\n");
			}
			
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			/* KEY2������ */
			if(NULL != Test_Ptr)
			{
				printf("�ͷ��ڴ�\n");
				vPortFree(Test_Ptr);  //�ͷ��ڴ�
				Test_Ptr = NULL;
				/* ��ȡ��ǰ�ڴ�ʣ���С */
				g_memsize = xPortGetFreeHeapSize();
				printf("ϵͳ��ǰ�ڴ��СΪ %d �ֽڣ��ڴ��ͷ����\n",g_memsize);
			}
			else
			{
				printf("���Ȱ���KEY1�����ڴ����ͷ�\n");
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

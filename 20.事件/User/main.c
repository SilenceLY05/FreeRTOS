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
#include "event_groups.h"
	
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
/* LED_Task������ */
static TaskHandle_t LED_Task_Handle = NULL;
/* KEY_Task������ */
static TaskHandle_t KEY_Task_Handle = NULL;


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
static EventGroupHandle_t Event_Handle = NULL;

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
 
 
 /******************************* �궨�� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */
 #define KEY1_EVENT  (0x01 << 0)  //�����¼������λ0
 #define KEY2_EVENT  (0x01 << 1)  //�����¼������λ1


/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void BSP_Init(void);
static void LED_Task(void* parameter);
static void KEY_Task(void* parameter);
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
	printf("����һ��FreeRTOS-�¼���־��ʵ�飡\r\n");
	
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
	Event_Handle = xEventGroupCreate();
	
	if(NULL != Event_Handle)
		printf("����Event_Handle�¼��������ɹ���\r\n");

	
	/* ����LowPriority���� */
	xReturn = xTaskCreate((TaskFunction_t) LED_Task,
												(const char*)"LED_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)2,
												(TaskHandle_t*)&LED_Task_Handle); // ������ƿ�ָ��
																			
	if(pdPASS == xReturn) /* ��������ɹ� */
		printf("����LED_Task�ɹ�\r\n");
	
	/* ����LowPriority���� */
	xReturn = xTaskCreate((TaskFunction_t) KEY_Task,
												(const char*)"KEY_Task",
												(uint16_t)512,
												(void*)NULL,
												(UBaseType_t)3,
												(TaskHandle_t*)&KEY_Task_Handle); // ������ƿ�ָ��
																			
	if(pdPASS == xReturn) /* ��������ɹ� */
		printf("����KEY_Task�ɹ�\r\n");
	
	
	
	vTaskDelete(AppTaskCreate_Handle);   //ɾ��AppTaskCreate����
	
	taskEXIT_CRITICAL();  //�˳��ٽ���
}

/**********************************************************************
  * @ ������  �� LED_Task
  * @ ����˵���� LED_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void LED_Task(void* parameter)
{
	EventBits_t r_event;  //����һ���¼����ձ���
	
	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		/*******************************************************************
     * �ȴ������¼���־ 
     * 
     * ���xClearOnExit����ΪpdTRUE����ô��xEventGroupWaitBits()����֮ǰ��
     * �������ȴ�����������������ص�ԭ���ǳ�ʱ������ô���¼���������
     * ��uxBitsToWaitFor�е��κ�λ����������� 
     * ���xClearOnExit����ΪpdFALSE��
     * ���ڵ���xEventGroupWaitBits()ʱ����������¼��������õ�λ��
     *
     * xWaitForAllBits���xWaitForAllBits����ΪpdTRUE����uxBitsToWaitFor��
     * ������λ�����û�ָ���Ŀ�ʱ�䵽��ʱ��xEventGroupWaitBits()�ŷ��ء� 
     * ���xWaitForAllBits����ΪpdFALSE��������uxBitsToWaitFor�����õ��κ�
     * һ��λ��1 ��ָ���Ŀ�ʱ�䵽��ʱ��xEventGroupWaitBits()���᷵�ء� 
     * ����ʱ����xTicksToWait����ָ����          
      *********************************************************/
		r_event = xEventGroupWaitBits(Event_Handle,                     //�¼�������
																  KEY1_EVENT | KEY2_EVENT,          //�����̸߳���Ȥ���¼�
																	pdTRUE,                           //�˳�ʱ����¼�λ
																	pdTRUE,                           //�������Ȥ�������¼�
																	portMAX_DELAY);                   //ָ����ʱʱ�䣬һֱ�ȴ�
		
		if((r_event & (KEY1_EVENT | KEY2_EVENT)) == (KEY1_EVENT | KEY2_EVENT))
		{
			//���������ɲ�����ȷ
			printf("KEY1��KEY2������\n");
			LED1_TOGGLE;
		}
		else
			printf("�¼�����\n");
	}
}


/**********************************************************************
  * @ ������  �� KEY_Task
  * @ ����˵���� KEY_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void KEY_Task(void* parameter)
{
	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			printf("KEY1������\n");
			/* ����һ���¼�1 */
			xEventGroupSetBits(Event_Handle,KEY1_EVENT);
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			printf("KEY2������\n");
			/* ����һ���¼�1 */
			xEventGroupSetBits(Event_Handle,KEY2_EVENT);
		}
		
		vTaskDelay(20);   //ÿ20msɨ��һ��
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

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


/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */

 
 
 /******************************* �궨�� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */
#define KEY1_EVENT  (0x01 << 0)
#define KEY2_EVENT  (0x01 << 1)

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
	
	xReturn = xTaskCreate((TaskFunction_t)LED_Task,/* ������ں��� */
												(const char*)"LED_Task",/* �������� */
												(uint16_t)512,/* ����ջ��С */
												(void*)NULL,/* ������ں������� */
												(UBaseType_t)2,/* ��������ȼ� */
												(TaskHandle_t*)&LED_Task_Handle);/* ������ƿ�ָ�� */
												
	if(xReturn == pdPASS)
		printf("����LED_Task ����ɹ�!\r\n");
	
	xReturn = xTaskCreate((TaskFunction_t)KEY_Task,/* ������ں��� */
												(const char*)"KEY_Task",/* �������� */
												(uint16_t)512,/* ����ջ��С */
												(void*)NULL,/* ������ں������� */
												(UBaseType_t)3,/* ��������ȼ� */
												(TaskHandle_t*)&KEY_Task_Handle);/* ������ƿ�ָ�� */
												
	if(xReturn == pdPASS)
		printf("����KEY_Task ����ɹ�!\r\n");
	
	
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
	uint32_t r_event = 0;
	uint32_t last_event = 0;
	BaseType_t xReturn = pdTRUE;
	
	while(1)
	{
		/* BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry, 
                                  uint32_t ulBitsToClearOnExit, 
                                  uint32_t *pulNotificationValue, 
                                  TickType_t xTicksToWait ); 
     * ulBitsToClearOnEntry����û�н��յ�����֪ͨ��ʱ������ֵ֪ͨ��˲�����ȡ
       ��ֵ���а�λ�����㣬���˲���ΪOxfffff����ULONG_MAX��ʱ��ͻὫ����ֵ֪ͨ���㡣
     * ulBits ToClearOnExit��������յ�������֪ͨ����������Ӧ�Ĵ����˳�����֮ǰ��
       ����ֵ֪ͨ��˲�����ȡ��ֵ���а�λ�����㣬���˲���Ϊ0xfffff����ULONG MAX��ʱ��
       �ͻὫ����ֵ֪ͨ���㡣
     * pulNotification Value���˲���������������ֵ֪ͨ��
     * xTick ToWait������ʱ�䡣
     *
     * ����ֵ��pdTRUE����ȡ��������֪ͨ��pdFALSE������֪ͨ��ȡʧ�ܡ�
     */
    //��ȡ����֪ͨ ,û��ȡ����һֱ�ȴ�
		xReturn = xTaskNotifyWait(0x0,ULONG_MAX,&r_event,portMAX_DELAY);
		
		if(pdTRUE == xReturn)
		{
			last_event |= r_event;
			
			/* ���������ɲ�����ȷ */
			if(last_event == (KEY1_EVENT | KEY2_EVENT))
			{
				last_event = 0;
				printf("KEY1��KEY2������\r\n");
				LED1_TOGGLE;
			}
			else
				last_event = r_event;
			
		}
		
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

	while(1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			printf("KEY1������\n");
			
			xTaskNotify((TaskHandle_t)LED_Task_Handle,(uint32_t)KEY1_EVENT,(eNotifyAction)eSetBits);
			
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			printf("KEY2������\n");
			
			xTaskNotify((TaskHandle_t)LED_Task_Handle,(uint32_t)KEY2_EVENT,(eNotifyAction)eSetBits);
			
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

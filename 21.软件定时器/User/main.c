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
static TimerHandle_t Swtmr1_Handle = NULL;
static TimerHandle_t Swtmr2_Handle = NULL;

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
static uint32_t TmrCb_Count1 = 0;   /* ��¼�����ʱ��1�ص�����ִ�д��� */
static uint32_t TmrCb_Count2 = 0;   /* ��¼�����ʱ��2�ص�����ִ�д��� */
 
 
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
static void Swtmr1_Callback(void* parameter);
static void Swtmr2_Callback(void* parameter);
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
	printf("����һ��FreeRTOS-�����ʱ��ʵ�飡\r\n");
	
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
	
	taskENTER_CRITICAL();    //�����ٽ���
	
	/************************************************************************************
   * ����������ڶ�ʱ��
   * ����ԭ��
   * TimerHandle_t xTimerCreate(	const char * const pcTimerName,
								const TickType_t xTimerPeriodInTicks,
								const UBaseType_t uxAutoReload,
								void * const pvTimerID,
                TimerCallbackFunction_t pxCallbackFunction )
    * @uxAutoReload : pdTRUEΪ����ģʽ��pdFALSΪ����ģʽ
   * ���ζ�ʱ��������(1000��ʱ�ӽ���)������ģʽ
   *************************************************************************************/

	
	/* ����LowPriority���� */
	Swtmr1_Handle = xTimerCreate((const char*) "AutoReloadTimer",
												(TickType_t)1000,/* ��ʱ������ 1000(tick) */
												(UBaseType_t)pdTRUE,/* ����ģʽ */
												(void*)1,/* Ϊÿ����ʱ������һ��������ΨһID */
												(TimerCallbackFunction_t)Swtmr1_Callback); // ������ƿ�ָ��
																			
	if(Swtmr1_Handle != NULL)
	{
		/***********************************************************************************
     * xTicksToWait:����ڵ���xTimerStart()ʱ��������������tickΪ��λָ����������Ӧ����
     * ��Blocked(����)״̬�Եȴ�start����ɹ����͵�timer������е�ʱ�䡣 
     * ������������ȳ���֮ǰ����xTimerStart()�������xTicksToWait�����������õȴ�ʱ��Ϊ0.
     **********************************************************************************/
		xTimerStart(Swtmr1_Handle,0);   //�������ڶ�ʱ��
	}
	
	/************************************************************************************
   * ����������ڶ�ʱ��
   * ����ԭ��
   * TimerHandle_t xTimerCreate(	const char * const pcTimerName,
								const TickType_t xTimerPeriodInTicks,
								const UBaseType_t uxAutoReload,
								void * const pvTimerID,
                TimerCallbackFunction_t pxCallbackFunction )
    * @uxAutoReload : pdTRUEΪ����ģʽ��pdFALSΪ����ģʽ
   * ���ζ�ʱ��������(1000��ʱ�ӽ���)������ģʽ
   *************************************************************************************/

	
	/* ����LowPriority���� */
	Swtmr2_Handle = xTimerCreate((const char*) "OneShotTimer",
												(TickType_t)5000,/* ��ʱ������ 5000(tick) */
												(UBaseType_t)pdFALSE,/* ����ģʽ */
												(void*)2,/* Ϊÿ����ʱ������һ��������ΨһID */
												(TimerCallbackFunction_t)Swtmr2_Callback); // ������ƿ�ָ��
																			
	if(Swtmr2_Handle != NULL)
	{
		/***********************************************************************************
     * xTicksToWait:����ڵ���xTimerStart()ʱ��������������tickΪ��λָ����������Ӧ����
     * ��Blocked(����)״̬�Եȴ�start����ɹ����͵�timer������е�ʱ�䡣 
     * ������������ȳ���֮ǰ����xTimerStart()�������xTicksToWait�����������õȴ�ʱ��Ϊ0.
     **********************************************************************************/
		xTimerStart(Swtmr2_Handle,0);   //�������ڶ�ʱ��
	}
	
	
	vTaskDelete(AppTaskCreate_Handle);   //ɾ��AppTaskCreate����
	
	taskEXIT_CRITICAL();  //�˳��ٽ���
}

/***********************************************************************
  * @ ������  �� Swtmr1_Callback
  * @ ����˵���� �����ʱ��1 �ص���������ӡ�ص�������Ϣ&��ǰϵͳʱ��
  *              �����ʱ���벻Ҫ��������������Ҳ��Ҫ������ѭ����Ӧ������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void Swtmr1_Callback(void* parameter)
{
	TickType_t tick_num1;
	
	TmrCb_Count1++;     /* ÿ�ص�һ�μ�һ */
	
	tick_num1 = xTaskGetTickCount();   //��ȡ�δ�ʱ���ļ���ֵ
	
	printf("Swtmr1_Callback����ִ�� %d �� \n",TmrCb_Count1);
	printf("�δ�ʱ����ֵ = %d\n",tick_num1);
}


/***********************************************************************
  * @ ������  �� Swtmr2_Callback
  * @ ����˵���� �����ʱ��2 �ص���������ӡ�ص�������Ϣ&��ǰϵͳʱ��
  *              �����ʱ���벻Ҫ��������������Ҳ��Ҫ������ѭ����Ӧ������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void Swtmr2_Callback(void* parameter)
{
	TickType_t tick_num2;
	
	TmrCb_Count2++;     /* ÿ�ص�һ�μ�һ */
	
	tick_num2 = xTaskGetTickCount();   //��ȡ�δ�ʱ���ļ���ֵ
	
	printf("Swtmr2_Callback����ִ�� %d �� \n",TmrCb_Count2);
	printf("�δ�ʱ����ֵ = %d\n",tick_num2);
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

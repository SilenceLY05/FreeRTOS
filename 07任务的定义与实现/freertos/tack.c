#include "FreeRTOS.h"
#include "task.h"


/*
*************************************************************************
*                               ������ƿ�
*************************************************************************
*/

/* ��ǰ�������е������������ƿ�ָ�룬Ĭ�ϳ�ʼ��ΪNULL */
TCB_t * volatile pxCurrentTCB = NULL;

/* ��������б� */
List_t pxReadyTasksLists[configMAX_PRIORITIES];

static volatile UBaseType_t uxCurrentNumberOfTasks = (UBaseType_t) 0U;


static void  prvInitialiseNewTask(TaskFunction_t pxTaskCode,          /* ������� */
																  const char* const pcName,           /* �������ƣ��ַ�����ʽ */
															  	const uint32_t ulStackDepth,        /* ����ջ��С����λΪ�� */
																  void * const pvParameters,          /* �����β� */
															 	  TaskHandle_t* const puxStackBuffer,  /* ����ջ��ʼ��ַ */
																  TCB_t* const pxTaskBuffer );         /* ������ƿ�ָ�� */



/* ��̬���񴴽����� */
#if (configSUPPORT_STATIC_ALLOCATION == 1)


TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,          /* ������� */
																 const char* const pcName,           /* �������ƣ��ַ�����ʽ */
																 const uint32_t ulStackDepth,        /* ����ջ��С����λΪ�� */
																 void * const pvParameters,          /* �����β� */
																 StackType_t* const puxStackBuffer,  /* ����ջ��ʼ��ַ */
																 TCB_t* const pxTaskBuffer )         /* ������ƿ�ָ�� */
{
	TCB_t *pxNewTCB;
	TaskHandle_t xReturn;           /* ����һ������������ָ�������TCB */
	
	if((pxTaskBuffer != NULL) && (puxStackBuffer != NULL))
	{
		pxNewTCB = (TCB_t*) pxTaskBuffer;
		pxNewTCB->pxStack = (StackType_t*)puxStackBuffer;
		
		/* �����µ����� */
		prvInitialiseNewTask(pxTaskCode, pcName,ulStackDepth,pvParameters,&xReturn,pxNewTCB);
	}
	else
	{
		xReturn = NULL;
	}
	
	/* ������������������񴴽��ɹ�����ʱxReturnӦ��ָ��������ƿ� */
	return xReturn;
}

#endif /* configSUPPORT_STATIC_ALLOCATION */



static void  prvInitialiseNewTask(TaskFunction_t pxTaskCode,          /* ������� */
																  const char* const pcName,           /* �������ƣ��ַ�����ʽ */
															  	const uint32_t ulStackDepth,        /* ����ջ��С����λΪ�� */
																  void * const pvParameters,          /* �����β� */
															 	  TaskHandle_t* const pxCreatedTask,  /* ������ */
																  TCB_t*  pxNewTCB )         /* ������ƿ�ָ�� */
{
	StackType_t* pxTopOfStack;
	UBaseType_t x;
	
	/* ��ȡջ����ַ */
	pxTopOfStack = pxNewTCB->pxStack + (ulStackDepth - (uint32_t)1);
	
	/* ������8�ֽڶ��� */
	pxTopOfStack = (StackType_t*) (((uint32_t)pxTopOfStack)&(~((uint32_t)0x007)));
	
	/* ����������ִ洢��TCB�� */
	for(x=(UBaseType_t)0;x<(UBaseType_t)configMAX_TASK_NAME_LEN;x++)
	{
		pxNewTCB->pcTackName[x] = pcName[x];
		
		if(pcName[x] == 0x00)
		{
			break;
		}
	}
	
	/* �������ֵĳ��Ȳ��ܳ���configMAX_TASK_NAME_LEN */
	pxNewTCB->pcTackName[configMAX_TASK_NAME_LEN - 1] = '\0';
	
	/* ��ʼ��TCB�е�xStateListItem�ڵ� */
	vListInitialiseItem(&(pxNewTCB->xStateListItem));
	
	/* ����xStateListItem�ڵ��ӵ���� */
	listSET_LIST_ITEM_OWNER( &( pxNewTCB->xStateListItem ), pxNewTCB );
	
	/* ��ʼ������ջ */
	pxNewTCB->pxTopOfStack = pxPortInitialiseStack(pxTopOfStack,pxTaskCode,pvParameters);
	
	/* ��������ָ��������ƿ� */
	if((void*)pxCreatedTask != NULL)
	{
		*pxCreatedTask = (TaskHandle_t) pxNewTCB;
	}

}


/* ��ʼ��������ص��б� */
void prvInitialiseTaskLists( void )
{
  UBaseType_t uxPriority;
    
    
  for( uxPriority = ( UBaseType_t ) 0U; uxPriority < ( UBaseType_t ) configMAX_PRIORITIES; uxPriority++ )
	{
		vListInitialist( &( pxReadyTasksLists[ uxPriority ] ) );
	}
}

extern TCB_t Task1TCB;
extern TCB_t Task2TCB;
void vTaskStartScheduler( void )
{
    /* �ֶ�ָ����һ�����е����� */
    pxCurrentTCB = &Task1TCB;
    
    /* ���������� */
    if( xPortStartScheduler() != pdFALSE )
    {
        /* �����������ɹ����򲻻᷵�أ��������������� */
    }
}

void vTaskSwitchContext( void )
{    
    /* �������������л� */
    if( pxCurrentTCB == &Task1TCB )
    {
        pxCurrentTCB = &Task2TCB;
    }
    else
    {
        pxCurrentTCB = &Task1TCB;
    }
}

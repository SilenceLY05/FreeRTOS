#ifndef __LIST_H__
#define __LIST_H__


#include "FreeRTOS.h"

/*  ����ڵ�ṹ��  */
struct xLIST_ITEM
{
	TickType_t xItemValue;               /* ����ֵ�����ڰ����ڵ���˳������ */
	struct xLIST_ITEM* pxNext;           /* ָ���������һ���ڵ� */
	struct xLIST_ITEM* pxPrevious;       /* ָ���������һ���ڵ� */
	void* pvOwner;                       /* ָ��ӵ�иýڵ���ں˶���ͨ����TCB */
	void* pvContainer;                   /* ָ��ýڵ����ڵ����� */
};

typedef struct xLIST_ITEM ListItem_t;  /* �ڵ����������ض��� */


/* ������ڵ� */
struct xMINI_LIST_ITEM
{
	TickType_t xItemValue;               /* ����ֵ�����ڰ����ڵ���˳������ */
	struct xLIST_ITEM* pxNext;           /* ָ���������һ���ڵ� */
	struct xLIST_ITEM* pxPrevious;       /* ָ���������һ���ڵ� */
};

typedef struct xMINI_LIST_ITEM MiniListItem_t;    /* ��С�ڵ����������ض��� */


/* ����ṹ�� */
typedef struct xLIST
{
	UBaseType_t uxNumberOfItems;   /* ����ڵ������ */
	ListItem_t* pxIndex;           /* ����ڵ�����ָ�� */
	MiniListItem_t xListEnd;       /* �������һ���ڵ� */
}List_t;



/* �ڵ��ʼ�� */
void vListInitialiseItem(ListItem_t * const pxItem);
//������ڵ�ĳ�ʼ��
void vListInitialist(List_t* const pxList);
/* ����β������ڵ�  pxList��ʾ����  pxNewListItem��ʾ�¼��������ڵ� */
void vListInsertEnd(List_t* const pxList,ListItem_t* const pxNewListItem);
/* ���ڵ㰴������������뵽���� */
void vListInsert(List_t * const pxList,ListItem_t* const pxNewListItem);
/* ���ڵ��������ɾ�� */
UBaseType_t uxListRemove(ListItem_t* const pxItemToRemove);



#endif /* __LIST_H__ */



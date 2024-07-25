#include "list.h"


/* �ڵ��ʼ�� */
void vListInitialiseItem(ListItem_t * const pxItem)
{
	pxItem->pvContainer = NULL;
	
}



//������ڵ�ĳ�ʼ��
void vListInitialist(List_t* const pxList)
{
	/* ����������ָ��ָ�����һ���ڵ� */
	pxList->pxIndex  = (ListItem_t*)&(pxList->xListEnd);
	
	/* ���������һ���ڵ�ĸ��������ֵ����Ϊ���ȷ���ýڵ������������ڵ� */
	pxList->xListEnd.xItemValue = portMAX_DELAY;
	
	/* �����һ���ڵ��pxNext��pxPrevious��ָ��������ʾ����Ϊ�� */
	pxList->xListEnd.pxNext = (ListItem_t*)&(pxList->xListEnd);
	pxList->xListEnd.pxPrevious = (ListItem_t*)&(pxList->xListEnd);
	
	/* ��ʼ������ڵ��������ֵΪ0 */
	pxList->uxNumberOfItems = (UBaseType_t) 0U;
}


/* ����β������ڵ�  pxList��ʾ����  pxNewListItem��ʾ�¼��������ڵ� */
void vListInsertEnd(List_t* const pxList,ListItem_t* const pxNewListItem)
{
	ListItem_t* const ppxIndex = pxList->pxIndex;
	
	pxNewListItem->pxNext = ppxIndex;
	pxNewListItem->pxPrevious = ppxIndex->pxPrevious;
	ppxIndex->pxPrevious->pxNext = pxNewListItem;
	ppxIndex->pxPrevious = pxNewListItem;
	
	/* ��ס�ýڵ����ڵ����� */
	pxNewListItem->pvContainer = (void*) pxList;
	
	/* ����ڵ������++ */
	(pxList->uxNumberOfItems)++;
}


/* ���ڵ㰴������������뵽���� */
void vListInsert(List_t * const pxList,ListItem_t* const pxNewListItem)
{
	ListItem_t* pxIterator;
	
	/* ��ȡ�ڵ��������ֵ */
	const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;
	
	/* Ѱ�ҽڵ�Ҫ�����λ�� */
	if(xValueOfInsertion == portMAX_DELAY)
	{
		pxIterator = pxList->xListEnd.pxPrevious;
	}
	else
	{
		for(pxIterator = (ListItem_t*)&(pxList->xListEnd); pxIterator->pxNext->xItemValue <= xValueOfInsertion;pxIterator = pxIterator->pxNext)
		{
			/* û��������� Ѱ�ҽڵ�Ҫ�����λ�� */
		}
	}
	
	pxNewListItem->pxNext = pxIterator->pxNext;
	pxNewListItem->pxNext->pxPrevious = pxNewListItem;
	pxNewListItem->pxPrevious = pxIterator;
	pxIterator->pxNext = pxNewListItem;
	
	/* ��ס�ýڵ����ڵ����� */
	pxNewListItem->pvContainer = (void*)pxList;
	
	(pxList->uxNumberOfItems)++;
}



/* ���ڵ��������ɾ�� */
UBaseType_t uxListRemove(ListItem_t* const pxItemToRemove)
{
	/* ��ȡ�ڵ��������� */
	List_t* const pxlist = (List_t*)pxItemToRemove->pvContainer;
	
	pxItemToRemove->pxNext ->pxPrevious = pxItemToRemove->pxPrevious;
	pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;
	
	/* ȷ���������������Ч */
	if(pxlist->pxIndex == pxItemToRemove)
	{
		pxlist->pxIndex = pxItemToRemove->pxPrevious;
	}
	
	/* ��ʼ���ýڵ����ڵ�����Ϊ�գ���ʾ�ڵ㻹û�в����κ����� */
	pxItemToRemove->pvContainer = NULL;
	
	/* ����ڵ������-- */
	(pxlist->uxNumberOfItems)--;
	
	/* ���������нڵ���� */
	return pxlist->uxNumberOfItems;
}



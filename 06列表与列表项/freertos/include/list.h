#ifndef __LIST_H__
#define __LIST_H__


#include "FreeRTOS.h"

/*  定义节点结构体  */
struct xLIST_ITEM
{
	TickType_t xItemValue;               /* 辅助值，用于帮助节点做顺序排列 */
	struct xLIST_ITEM* pxNext;           /* 指向链表的下一个节点 */
	struct xLIST_ITEM* pxPrevious;       /* 指向链表的上一个节点 */
	void* pvOwner;                       /* 指向拥有该节点的内核对象，通常是TCB */
	void* pvContainer;                   /* 指向该节点所在的链表 */
};

typedef struct xLIST_ITEM ListItem_t;  /* 节点数据类型重定义 */


/* 定义根节点 */
struct xMINI_LIST_ITEM
{
	TickType_t xItemValue;               /* 辅助值，用于帮助节点做顺序排列 */
	struct xLIST_ITEM* pxNext;           /* 指向链表的下一个节点 */
	struct xLIST_ITEM* pxPrevious;       /* 指向链表的上一个节点 */
};

typedef struct xMINI_LIST_ITEM MiniListItem_t;    /* 最小节点数据类型重定义 */


/* 链表结构体 */
typedef struct xLIST
{
	UBaseType_t uxNumberOfItems;   /* 链表节点计数器 */
	ListItem_t* pxIndex;           /* 链表节点索引指针 */
	MiniListItem_t xListEnd;       /* 链表最后一个节点 */
}List_t;



/* 节点初始化 */
void vListInitialiseItem(ListItem_t * const pxItem);
//链表根节点的初始化
void vListInitialist(List_t* const pxList);
/* 链表尾部插入节点  pxList表示链表  pxNewListItem表示新加入的链表节点 */
void vListInsertEnd(List_t* const pxList,ListItem_t* const pxNewListItem);
/* 将节点按照升序排序插入到链表 */
void vListInsert(List_t * const pxList,ListItem_t* const pxNewListItem);
/* 将节点从链表中删除 */
UBaseType_t uxListRemove(ListItem_t* const pxItemToRemove);



#endif /* __LIST_H__ */



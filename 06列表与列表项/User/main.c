#include "list.h"

//定义根节点
struct xLIST List_Test;

//定义节点
struct xLIST_ITEM List_Item1;
struct xLIST_ITEM List_Item2;
struct xLIST_ITEM List_Item3;


int main(void)
{
	//根节点初始化
	vListInitialist(&List_Test);
	
	//节点初始化
	vListInitialiseItem(&List_Item1);
	vListInitialiseItem(&List_Item2);
	vListInitialiseItem(&List_Item3);
	
	vListInsert(&List_Test,&List_Item1);
	vListInsert(&List_Test,&List_Item2);
	vListInsert(&List_Test,&List_Item3);
	
	return 0;
}



#include "list.h"

//������ڵ�
struct xLIST List_Test;

//����ڵ�
struct xLIST_ITEM List_Item1;
struct xLIST_ITEM List_Item2;
struct xLIST_ITEM List_Item3;


int main(void)
{
	//���ڵ��ʼ��
	vListInitialist(&List_Test);
	
	//�ڵ��ʼ��
	vListInitialiseItem(&List_Item1);
	vListInitialiseItem(&List_Item2);
	vListInitialiseItem(&List_Item3);
	
	vListInsert(&List_Test,&List_Item1);
	vListInsert(&List_Test,&List_Item2);
	vListInsert(&List_Test,&List_Item3);
	
	return 0;
}



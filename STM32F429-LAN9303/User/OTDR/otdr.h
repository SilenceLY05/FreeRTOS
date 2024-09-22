#ifndef __OTDR_H__
#define __OTDR_H__

#include "stm32f4xx.h"
//#include <unistd.h> 
#include <stdio.h> 
#include <string.h> 
//#include <strings.h> 
//#include <sys/types.h> 
//#include <sys/socket.h> 
//#include <sys/select.h> 
//#include <sys/time.h> 
//#include <netinet/in.h> 
//#include <arpa/inet.h> 
#include <errno.h>
#include <stdint.h>



/********************************* CMD Macro **************************************/ 
#define CMD_HOST_START_MEASURE 0x10000000 // ���ò��Բ������������� 
#define CMD_HOST_STOP_MEASURE 0x10000001 // ȡ������ֹ���� 
#define CMD_HOST_SET_IP 0x10000002 // ���� IP 
#define CMD_HOST_NETWORK_IDLE 0x10000004 // ������� 
#define CMD_OTDR_UPDATE 0x20000000 // OTDR �������� 
#define CMD_DSP_UPLOAD_ALL_DATA 0x90000000 // �ϴ�ȫ���������� 
#define CMD_DSP_UPLOAD_REF_DATA 0x90000001 // �ϴ�ˢ�²������� 
#define CMD_RESPONSE_STATE 0xA0000000 // ����״̬��

/******************************* ֡��ʽ���� **************************************/ 
// �汾�� 
#define REV_ID 0 
#define RSVD_VALUE 0xffffeeee 
// ֡���� 
#define FRAMETYPE_HOST2TARGET 0 // ������������ 
#define FRAMETYPE_TARGET2HOST 1 // ��Ӧ�������� 
#define FRAME_SYNC_STRING "GLinkOtdr-3800M" 
// ֡ͷ��־ 
typedef struct 
{ 
 char FrameSync[16]; // GLinkOtdr-3800M 
 uint32_t TotalLength; // ��֡�� 
 uint32_t Rev; // �汾�� 
 uint32_t FrameType; // ֡���� 
 uint32_t Src; // Դ��ַ 
 uint32_t Dst; // Ŀ�ĵ�ַ 
 uint32_t PacketID; // ��ˮ�� 
 uint32_t RSVD; // ���� 
}frame_header_t; 


#endif /* __OTDR_H__ */

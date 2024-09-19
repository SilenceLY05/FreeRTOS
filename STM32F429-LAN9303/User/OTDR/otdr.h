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


/********************************* CMD Macro **************************************/ 
#define CMD_HOST_START_MEASURE 0x10000000 // 配置测试参数并启动测试 
#define CMD_HOST_STOP_MEASURE 0x10000001 // 取消或终止测试 
#define CMD_HOST_SET_IP 0x10000002 // 设置 IP 
#define CMD_HOST_NETWORK_IDLE 0x10000004 // 网络空闲 
#define CMD_OTDR_UPDATE 0x20000000 // OTDR 在线升级 
#define CMD_DSP_UPLOAD_ALL_DATA 0x90000000 // 上传全部测试数据 
#define CMD_DSP_UPLOAD_REF_DATA 0x90000001 // 上传刷新测试数据 
#define CMD_RESPONSE_STATE 0xA0000000 // 返回状态码

/******************************* 帧格式定义 **************************************/ 
// 版本号 
#define REV_ID 0 
#define RSVD_VALUE 0xffffeeee 
// 帧类型 
#define FRAMETYPE_HOST2TARGET 0 // 主控命令类型 
#define FRAMETYPE_TARGET2HOST 1 // 响应命令类型 
#define FRAME_SYNC_STRING "GLinkOtdr-3800M" 
// 帧头标志 
typedef struct 
{ 
 char FrameSync[16]; // GLinkOtdr-3800M 
 uint32_t TotalLength; // 总帧长 
 uint32_t Rev; // 版本号 
 uint32_t FrameType; // 帧类型 
 uint32_t Src; // 源地址 
 uint32_t Dst; // 目的地址 
 uint32_t PacketID; // 流水号 
 uint32_t RSVD; // 保留 
}frame_header_t; 


#endif /* __OTDR_H__ */


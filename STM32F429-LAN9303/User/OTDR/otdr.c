#include "otdr.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 


#define BUF_SIZE (128*1024)

//ʵ��htonl����
uint32_t htonl(uint32_t hostlong) {
    return ((hostlong & 0xFF000000) >> 24) |
           ((hostlong & 0x00FF0000) >> 8)  |
           ((hostlong & 0x0000FF00) << 8)  |
           ((hostlong & 0x000000FF) << 24);
}

// ����������ת��Ϊ���縡���� 
float htonf(float t) 
{ 
	uint32_t ux; 
  float x = t; 
  ux = *(uint32_t*)&x; 
  ux = htonl(ux); 
  return *(float*)&ux; 
} 

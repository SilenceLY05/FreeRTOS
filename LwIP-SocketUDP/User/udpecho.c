#include "udpecho.h"
#include "lwip/opt.h"

#include <lwip/sockets.h>
#include "lwip/sys.h"
#include "lwip/api.h"

#define PORT             5001
#define RECV_DATA        (1024)

static void udpecho_thread(void *arg)
{
	int sock = -1;
	char *recv_data;
	struct sockaddr_in udp_addr,seraddr;
	int recv_data_len;
	socklen_t addrlen;
	
	while(1)
	{
		recv_data = (char*)pvPortMalloc(RECV_DATA);
		if(recv_data == NULL)
		{
			printf("No Memory\n");
			goto __exit;
		}
		
		sock = socket(AF_INET,SOCK_DGRAM,0);
		if(sock < 0)
		{
			printf("Socket error \n");
			goto __exit;
		}
		
		udp_addr.sin_family = AF_INET;
		udp_addr.sin_addr.s_addr = INADDR_ANY;
		udp_addr.sin_port = htons(PORT);
		memset(&(udp_addr.sin_zero),0,sizeof(udp_addr.sin_zero));
		
		if(bind(sock,(struct sockaddr *)&udp_addr,sizeof(struct sockaddr)) == -1)
		{
			printf("Unable to bind \n");
			goto __exit;
		}
		while(1)
		{
			recv_data_len = recvfrom(sock,recv_data,RECV_DATA,0,(struct sockaddr*)&seraddr,&addrlen);
			
			/* ��ʾ���Ͷ˵�IP��ַ */
			printf("receive from %s \n",inet_ntoa(seraddr.sin_addr));
			
			/* ��ʾ���Ͷ˷������ַ��� */
			printf("recevce : %s",recv_data);
			
			/* ���ַ������ظ����Ͷ� */
			sendto(sock,recv_data,recv_data_len,0,(struct sockaddr*)&seraddr,addrlen);
		}
	}
	
__exit:
	if(sock >= 0)  closesocket(sock);
	if(recv_data)   free(recv_data);
}

void udpecho_init(void)
{
	sys_thread_new("udpcho_thread",udpecho_thread,NULL,2048,4);
}




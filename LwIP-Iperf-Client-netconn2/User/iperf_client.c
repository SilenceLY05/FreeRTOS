#include "iperf_client.h"

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include <lwip/sockets.h>


#define PORT          5001
#define IP_ADDR             "10.194.2.14"
#define IPERF_BUFSZ         (4*1024)

static void iperf_server(void *thread_param)
{
	int sock = -1,i;
	struct sockaddr_in client_addr;
	
	uint8_t *send_buf;
	uint64_t sentlen;
	
	u32_t tick1,tick2;
	
	send_buf = (uint8_t *)pvPortMalloc(IPERF_BUFSZ);
	
	if(!send_buf) return;
	
	for(i = 0;i<IPERF_BUFSZ;i++)
		send_buf[i] = i&0xff;
	
	while(1)
	{
		sock = socket(AF_INET,SOCK_STREAM,0);
		
		if(sock < 0)
		{
			printf("socket error!\n");
			vTaskDelay(10);
			continue;
		}
		
		client_addr.sin_addr.s_addr = inet_addr(IP_ADDR);
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(PORT);
		memset(&(client_addr.sin_zero),0,sizeof(client_addr.sin_zero));
	
		if(connect(sock,(struct sockaddr*)&client_addr,sizeof(struct sockaddr)) == -1)
		{
			printf("connect failed!\n");
			closesocket(sock);
			vTaskDelay(10);
			continue;
		}
		
		printf("connect to iperf server succeddful!\n");
		
		tick1 = sys_now();
		
		while(1)
		{
			tick2 = sys_now();
			
			if(tick2 - tick1 >= configTICK_RATE_HZ *5)
			{
				float f;
				f = (float)(sentlen*configTICK_RATE_HZ / 125/(tick2 - tick1));
				
				f /= 1000.0f;
				printf("send speed = %.4f Mbps!\n",f);
				
				tick1 = tick2;
				sentlen = 0;
			}
			
			if(write(sock,send_buf,IPERF_BUFSZ) < 0)
				break;
			else
				sentlen += IPERF_BUFSZ;
		}
		closesocket(sock);
	}
}


void iperf_server_init(void)
{
	sys_thread_new("iperf_server",iperf_server,NULL,2048,4);
}


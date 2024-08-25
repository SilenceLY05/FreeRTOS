#include "client.h"

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"

#include <lwip/sockets.h>

#define PORT       5001
#define IP_ADDR    "10.194.127.254"



static void client(void *thread_param)
{
	int sock = -1;
	struct sockaddr_in   client_addr;
	
	
	uint8_t send_buf[] = "This is a TCP Client test ...\n";
	
	while(1)
	{
		sock = socket(AF_INET,SOCK_STREAM,0);
		
		if(sock < 0)
		{
			printf("sock error ! \n");
			vTaskDelay(10);
			continue;
		}
		
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(PORT);
		client_addr.sin_addr.s_addr = inet_addr(IP_ADDR);
		memset(&(client_addr.sin_zero),0,sizeof(client_addr.sin_zero));
		
		if(connect(sock,(struct sockaddr *)&client_addr,sizeof(struct sockaddr)) == -1)
		{
			printf("Connect failed!\n");
			closesocket(sock);
			vTaskDelay(10);
			continue;
		}
		
		printf("Connnect to iperf server successfull!\n");
		while(1)
		{
			if(write(sock,send_buf,sizeof(send_buf)) < 0)
			{
				break;
			}
			vTaskDelay(1000);
		}
		closesocket(sock);
	}
}


void client_init(void)
{
	sys_thread_new("client",client,NULL,512,4);
}

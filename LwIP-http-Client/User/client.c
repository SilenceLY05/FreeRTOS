#include "client.h"
#include "bsp_sdram.h"
#include <lwip/sockets.h>

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"


#define PORT       80
#define IP_ADDR    "114.215.151.106"

#ifdef    LWIP_DNS  
#define   HOST_NAME        "embedfire.com"
#else
#define   HOST_NAME        "114.215.151.106"
#endif


uint32_t *pSDRAM=(uint32_t *)SDRAM_BANK_ADDR;

uint8_t get_buf[] = "GET / HTTP/1.1\r\n \
										 Host: embedfire.com \r\n\r\n\r\n\r\n";
										 
										 
static void client(void *thread_param)
{
	int sock =-1,rece;
	struct sockaddr_in client_addr;
	
	uint8_t *pbufdata = pvPortMalloc(5000);
	
	char* host_ip;
	
#ifdef LWIP_DNS
	ip4_addr_t dns_ip;
	netconn_gethostbyname(HOST_NAME,&dns_ip);
	host_ip = ip_ntoa(&dns_ip);
	PRINT_DEBUG("host name : %s ,host_ip : %s \n",HOST_NAME,host_ip);
#else
	host_ip = HOST_NAME;
#endif
	
	printf("目标端口号是： %d\n\n",DEST_PORT);
	
	while(1)
	{
		sock = socket(AF_INET,SOCK_STREAM,0);
		if(sock < 0)
		{
			printf("Socket error \n");
			vTaskDelay(10);
			continue;
		}
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(DEST_PORT);
		client_addr.sin_addr.s_addr = inet_addr(host_ip);
		memset(&(client_addr.sin_zero),0,sizeof(client_addr.sin_zero));
		
		if(connect(sock,(struct sockaddr*)&client_addr,sizeof(struct sockaddr)) == -1)
		{
			printf("Connect failed!\n");
			closesocket(sock);
			vTaskDelay(10);
			continue;
		}
		
		printf("Connect to server successful !\n");
		memset(pSDRAM,0,400000);
		printf("\n************************************************************\n");
		write(sock,get_buf,sizeof(get_buf));
		while(1)
		{
			rece = recv(sock,(uint8_t *)pSDRAM,400000,0);
			
			if(rece <= 0)
				break;
			
			printf("%s\n",(uint8_t *)pSDRAM);
		}
		
		printf("\n**********************************************************\n");
    
    memset(pSDRAM,0,400000);
    closesocket(sock);
    vTaskDelay(10000);
	}
}

void client_init(void)
{
  sys_thread_new("client", client, NULL, 2048, 4);
}


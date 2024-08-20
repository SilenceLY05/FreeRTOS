#include "tcpecho.h"
#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"

static void tcpecho_thread(void *arg)
{
	struct netconn* conn,*newconn;
	err_t err;
	LWIP_UNUSED_ARG(arg);
	
#if LWIP_IPV6
	conn = netconn_new(NETCONN_TCP_IPV6);
	netconn_bind(conn,IP6_ADDR_ANY,5001);
#else
	conn = netconn_new(NETCONN_TCP);
	netconn_bind(conn,IP_ADDR_ANY,5001);
#endif
	
	LWIP_ERROR("tcpecho: invalid conn",(conn != NULL),return;);
	
	netconn_listen(conn);
	
	while(1)
	{
		err = netconn_accept(conn,&newconn);
		
		if(err == ERR_OK)
		{
			struct netbuf *buf;
			void *data;
			u16_t len;
			
			while((err = netconn_recv(newconn,&buf)) == ERR_OK)
			{
				do
				{
					netbuf_data(buf,&data,&len);
					err = netconn_write(newconn,data,len,NETCONN_COPY);
					
					if(err != ERR_OK)
					{
						printf("tcpecho: netconn_write:error \"%s\"\n",lwip_strerr(err));
					}
				}while(netbuf_next(buf) >= 0);
				netbuf_delete(buf);
			}
			
			netconn_close(newconn);
			netconn_delete(newconn);
		}
	}
}

void tcpecho_init(void)
{
	sys_thread_new("tcpecho_thread",tcpecho_thread,NULL,512,4);
}

#endif /* LWIP_NETCONN */


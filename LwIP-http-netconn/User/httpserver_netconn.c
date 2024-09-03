#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"

#include "httpserver_netconn.h"


#if LWIP_NETCONN
#ifndef HTTPD_DEBUG
#define HTTPD_DEBUG       LWIP_DBG_OFF
#endif


static const char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\b\r\n";
static const char http_index_html[] = "<html><head><title>Congrats!</title></head>\
                                       <body><h1 align=\"center\">Hello World!</h1>\
                                       <h2 align=\"center\">Welcome to Fire LwIP HTTP Server!</h1>\
                                       <p align=\"center\">This is a small test page, served by httpserver-netconn.</p>\
                                       <p align=\"center\"><a href=\"http://www.firebbs.cn/forum.php/\"> <font size=\"6\"> Ұ�������̳ </font> </a></p>\
                                       <a href=\"http://www.firebbs.cn/forum.php/\">\
                                       <p align=\"center\"><img src=\"http://www.firebbs.cn/data/attachment/portal/201806/05/163015rhz7mbgbt0zfujzh.jpg\" /></a>\
                                       </body></html>";

static void http_server_netconn_serve(struct netconn *conn)
{
	struct netbuf *inbuf;
	char *buf;
	u16_t buflen;
	err_t err;
	
	/* ��ȡ���� */
	err = netconn_recv(conn,&inbuf);
	
	if(err == ERR_OK)
	{
		netbuf_data(inbuf,(void **)&buf,&buflen);
		
		/* �ж��ǲ���HTTP��GET���� */
		if(buflen >= 5 && buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T' && buf[3] == ' ' && buf[4] == '/' )
		{
			/* �������� */
			netconn_write(conn,http_html_hdr,sizeof(http_html_hdr)-1,NETCONN_NOCOPY);
			
			/* ������ҳ���� */
			netconn_write(conn,http_index_html,sizeof(http_index_html)-1,NETCONN_NOCOPY);
		}
	}
	netconn_close(conn);  //�ر�����
	
	/* �ͷ�inbuf */
	netbuf_delete(inbuf);
}

static void http_server_netconn_thread(void *arg)
{
	struct netconn *conn,*newconn;
	err_t err;
	LWIP_UNUSED_ARG(arg);
	
	/* ����netconn���ӽṹ */
	/* �󶨶˿ںź�IP��ַ���˿ں�Ĭ��Ϊ80 */
	conn = netconn_new(NETCONN_TCP);
	netconn_bind(conn,IP_ADDR_ANY,80);
	
	LWIP_ERROR("http_server:invalid conn",(conn != NULL),return ;);
	
	/* ���� */
	netconn_listen(conn);
	
	do
	{
		//������������
		err = netconn_accept(conn,&newconn);
		
		if(err == ERR_OK)
		{
			//������ҳ����
			http_server_netconn_serve(newconn);
			
			//ɾ�����ӽṹ
			netconn_delete(newconn);
		}
	}while(err == ERR_OK);
	
	//�ر�
	netconn_close(conn);
	netconn_delete(conn);
}

void http_server_netconn_init(void)
{
	sys_thread_new("http_server_netconn",http_server_netconn_thread,NULL,2048,4);
}


#endif

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"

#include "httpserver_netconn.h"


#if LWIP_NETCONN
#ifndef HTTPD_DEBUG
#define HTTPD_DEBUG       LWIP_DBG_OFF
#endif


static const char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\b\r\n";
const unsigned char Led1On_Data[] ="<HTML> \
                                    <head><title>HTTP LED Control</title></head> \
                                    <center> \
                                    <p> \
                                    <font size=\"6\">LED<font style = \"color:red\">已打开！</font> \
                                    <form method=post action=\"off\" name=\"ledform\"> \
                                    <input type=\"submit\" value=\"关闭\" style=\"width:80px;height:30px;\"></form> \
                                    </center> \
                                    </HTML> ";


//当LED灭时，向浏览器返回如下html信息，显示结果如下图15-7所示
const unsigned char Led1Off_Data[] =" <HTML> \
																			<head><title>HTTP LED Control</title></head> \
																			<center> \
																			<p> \
																			<font size=\"6\">LED<font style = \"color:red\">已关闭！</font> \
																			<form method=post action=\"on\" name=\"ledform\"> \
																			<input type=\"submit\" value=\"打开\" style=\"width:80px;height:30px;\"></form> \
																			</center> \
																			</HTML> ";

static const char http_index_html[] = "<html><head><title>Congrats!</title></head>\
																			 <body><h2 align=\"center\">Welcome to Fire LwIP HTTP Server!</h2>\
																			 <p align=\"center\">This is a small test page : http control led.</p>\
																			 <p align=\"center\"><a href=\"http://www.firebbs.cn/forum.php/\"> <font size=\"6\"> 野火电子论坛 </font> </a></p>\
																			 <a href=\"http://www.firebbs.cn/forum.php/\">\
																			 <img src=\"http://www.firebbs.cn/data/attachment/portal/201806/05/163015rhz7mbgbt0zfujzh.jpg\"/></a>\
																			 </body></html>";

static bool led_on = FALSE;

void httpserver_send_html(struct netconn *conn,bool led_status)
{
	netconn_write(conn,http_html_hdr,sizeof(http_html_hdr)-1,NETCONN_NOCOPY);
	
	if(led_status == TRUE)
		netconn_write(conn,Led1On_Data,sizeof(Led1On_Data)-1,NETCONN_NOCOPY);
	else
		netconn_write(conn,Led1Off_Data,sizeof(Led1Off_Data)-1,NETCONN_NOCOPY);
	
	netconn_write(conn,http_index_html,sizeof(http_index_html)-1,NETCONN_NOCOPY);
}
	

static void http_server_netconn_serve(struct netconn *conn)
{
	struct netbuf *inbuf;
	char *buf;
	u16_t buflen;
	err_t err;
	
	/* 读取数据 */
	err = netconn_recv(conn,&inbuf);
	
	if(err == ERR_OK)
	{
		netbuf_data(inbuf,(void **)&buf,&buflen);
		
		/* 判断是不是HTTP的GET命令 */
		if(buflen >= 5 && buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T' && buf[3] == ' ' && buf[4] == '/' )
		{
			httpserver_send_html(conn,led_on);
		}
		else if(buflen >= 8 && buf[0] == 'P' && buf[1] == 'O' && buf[2] == 'S' && buf[3] == 'T' )
		{
			if(buf[6] == 'o' && buf[7] == 'n')
			{
				led_on = TRUE;
				LED1_ON;
				PRINT_DEBUG("LED ON !\n");
			}
			else if(buf[6] == 'o' && buf[7] == 'f' && buf[8] == 'f')
			{
				led_on = FALSE;
				LED1_OFF;
				PRINT_DEBUG("LED OFF!\n");
			}
			httpserver_send_html(conn,led_on);
		}
		netbuf_delete(inbuf);
	}
	netconn_close(conn);  //关闭连接

}

static void http_server_netconn_thread(void *arg)
{
	struct netconn *conn,*newconn;
	err_t err;
	LWIP_UNUSED_ARG(arg);
	
	/* 创建netconn连接结构 */
	/* 绑定端口号和IP地址，端口号默认为80 */
	conn = netconn_new(NETCONN_TCP);
	
	LWIP_ERROR("http_server:invalid conn",(conn != NULL),return ;);
	
	led_on = TRUE;
	LED1_ON;
	
	netconn_bind(conn,NULL,LOCAL_PORT);
	
	/* 监听 */
	netconn_listen(conn);
	
	do
	{
		//处理连接请求
		err = netconn_accept(conn,&newconn);
		
		if(err == ERR_OK)
		{
			//发送网页数据
			http_server_netconn_serve(newconn);
			
			//删除连接结构
			netconn_delete(newconn);
		}
	}while(err == ERR_OK);
	
	LWIP_DEBUGF(HTTPD_DEBUG,("http_server_netconn_thread: netconn_accept received error %d, shutting down",err));
	
	//关闭
	netconn_close(conn);
	netconn_delete(conn);
}

void http_server_netconn_init(void)
{
	sys_thread_new("http_server_netconn",http_server_netconn_thread,NULL,2048,4);
}


#endif


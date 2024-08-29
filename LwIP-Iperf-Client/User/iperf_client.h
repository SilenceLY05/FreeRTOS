#ifndef _IPERF_CLIENT_H_
#define _IPERF_CLIENT_H_

#define TCP_SERVER_THREAD_NAME          "iperf_server"

#define TCP_SERVER_THREAD_STACKSIZE     1024
#define TCP_SERVER_THREAD_PRIO          4

void iperf_server(void*thread_param);
void iperf_server_init(void);

#endif /* _IPERF_CLIENT_H_ */


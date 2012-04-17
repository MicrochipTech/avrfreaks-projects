#ifndef webclient
#define webclient
//void webclient_appcall(void);
void webclient_app(void);
void webclient_init(void);
void webclient_connect(void);
extern uint8_t systemID[8];
#if defined PORT_APP_MAPPER
	#define WEBCLIENT_APP_CALL_MAP {webclient_app, 0, 80},
#else
	#define WEBCLIENT_APP_CALL_MAP
	#define UIP_TCP_APPCALL webclient_app
	typedef int uip_tcp_appstate_t;
#endif

#endif

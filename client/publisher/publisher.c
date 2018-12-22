#include "contiki.h"
#include "sys/etimer.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "contiki-net.h"
#include "er-coap-engine.h"
#include <stdio.h>
#include <string.h>

#define LOCAL_PORT      UIP_HTONS(COAP_DEFAULT_PORT + 1)
#define REMOTE_PORT     UIP_HTONS(COAP_DEFAULT_PORT)

#define DEBUG 
#ifdef DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

/* leading and ending slashes only for demo purposes, get cropped automatically when setting the Uri-Path */
char *service_urls[] =
{ ".well-known/core", "ps", "ps/sensors", "ps/sensors/accelorometer"};

/*---------------------------------------------------------------------------*/
PROCESS(publisher, "publisher example process");
AUTOSTART_PROCESSES(&publisher);
/*---------------------------------------------------------------------------*/

static void
set_global_address(void)
{
	uip_ipaddr_t ipaddr;
	int i;
	uint8_t state;

  	uip_ip6addr(&ipaddr, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0, 0, 0, 0);
	uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
	uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

	printf("IPv6 addresses: ");
	for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
		state = uip_ds6_if.addr_list[i].state;
		if(uip_ds6_if.addr_list[i].isused && (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
			uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
			printf("\n");
		  	/* hack to make address "final" */
			if (state == ADDR_TENTATIVE) {
				uip_ds6_if.addr_list[i].state = ADDR_PREFERRED;
		  	}
		}
	}
}

static 
void print_addresses(void)
{
	int i;
	for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
		uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
		printf("\n");
	}
}
/* This function is will be passed to COAP_BLOCKING_REQUEST() to handle responses. */
void
client_chunk_handler(void *response)
{
  const uint8_t *chunk;

  int len = coap_get_payload(response, &chunk);

  printf("|%.*s", len, (char *)chunk);
}

static 
void create_topic(const uip_ipaddr_t *broker_addr, 
				  const char *service_url, 
				  const char *topic_name, 
				  coap_packet_t *request)
{  
	// implementare il secure coding
	char msg[256];
	memset(msg,0,256);
	/* prepare request, TID is set by COAP_BLOCKING_REQUEST() */
	coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
	coap_set_header_uri_path(request, service_url);

	//create!!!!!
	//const char msg[] = "<topic1>;ct=0;";
	strcat(msg, "<");
	strcat(msg, topic_name);
	strcat(msg, ">;ct=0;"); 
	coap_set_payload(request, (uint8_t *)msg, strlen(msg));
}

static 
void update_topic(const uip_ipaddr_t *broker_addr, 
				  const char *service_url, 
				  const char *new_value, 
				  uint8_t len,
				  coap_packet_t *request)
{
	coap_init_message(request, COAP_TYPE_CON, COAP_PUT, 0);
	coap_set_header_uri_path(request, service_url);

	coap_set_payload(request, (uint8_t *)new_value, len);
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(publisher, ev, data)
{
	static uint16_t i = -1;
#define DIM 8
	static char buf[DIM];
	static struct etimer periodic_timer;
	static uip_ipaddr_t broker_addr;
	static coap_packet_t request[1];
	PROCESS_BEGIN();

	set_global_address();
	
	uip_ip6addr(&broker_addr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0x0001); 
	
	// NETSTACK_MAC.off(1); //duty cycle del 100%
  	/* receives all CoAP messages */
  	coap_init_engine();
	etimer_set(&periodic_timer, CLOCK_SECOND);

	PROCESS_WAIT_EVENT_UNTIL(PROCESS_EVENT_TIMER);
	create_topic(&broker_addr, "ps", "sensors", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);
	PRINTF("topic ps/sensors created\n");
	etimer_reset(&periodic_timer);
	
	PROCESS_WAIT_EVENT_UNTIL(PROCESS_EVENT_TIMER);
	create_topic(&broker_addr, "ps/sensors", "accelorometer", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);
	PRINTF("topic ps/sensors/accelorometer created\n");
	etimer_reset(&periodic_timer);

	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(PROCESS_EVENT_TIMER);
		PRINTF("I update_topic i=%d \n",i);
		sprintf(buf,"%d",i);
		update_topic(&broker_addr, "ps/sensors/accelorometer", buf, DIM, request);
		COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);
		etimer_reset(&periodic_timer);
		i++;
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/

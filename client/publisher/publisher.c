#include "contiki.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "simple-udp.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include "er-coap-engine.h"
#include <stdio.h>
#include <string.h>

#define PERIOD 1
#define LOCAL_PORT      UIP_HTONS(COAP_DEFAULT_PORT + 1)
#define REMOTE_PORT     UIP_HTONS(COAP_DEFAULT_PORT)

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

/* FIXME: This server address is hard-coded for Cooja and link-local for unconnected border router. */
//#define SERVER_NODE(ipaddr)   uip_ip6addr(ipaddr, 0xfe80, 0, 0, 0, 0x0212, 0x7402, 0x0002, 0x0202)      /* cooja2 */
#define SERVER_NODE(ipaddr)   uip_ip6addr(ipaddr, 0, 0, 0, 0, 0, 0, 0, 1) //localhost

/* Example URIs that can be queried. */
#define NUMBER_OF_URLS 3
/* leading and ending slashes only for demo purposes, get cropped automatically when setting the Uri-Path */
char *service_urls[NUMBER_OF_URLS] =
{ ".well-known/core", "/sensors/temperature", "/sensors/accelormeter"};

static struct simple_udp_connection connection;
uip_ipaddr_t broker_ipaddr;

/*---------------------------------------------------------------------------*/
PROCESS(publisher, "publisher example process");
AUTOSTART_PROCESSES(&publisher);
/*---------------------------------------------------------------------------*/

static void
receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
	printf("Data received from ");
	uip_debug_ipaddr_print(sender_addr);

	printf(" on port %d from port %d with length %d\n",
         receiver_port, sender_port, datalen);
   printf("data sending reply\n");
   simple_udp_sendto(c, "Reply", strlen("Reply") + 1, sender_addr);
}
/*---------------------------------------------------------------------------*/
static void
set_global_address(void)
{
	uip_ipaddr_t ipaddr;
	int i;
	uint8_t state;

	uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
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
/* This function is will be passed to COAP_BLOCKING_REQUEST() to handle responses. */
void
client_chunk_handler(void *response)
{
  const uint8_t *chunk;

  int len = coap_get_payload(response, &chunk);

  printf("|%.*s", len, (char *)chunk);
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(publisher, ev, data)
{
	static struct etimer periodic_timer;

	PROCESS_BEGIN();

	set_global_address();
	/*simple_udp_register(&connection, UDP_PORT,
	                  NULL, UDP_PORT,
	                  receiver);
	uip_ip6addr(&addr, 0xfe80, 0, 0, 0, 0xc30c, 0, 0, 0x0001); //broker address??
	*/
	static coap_packet_t request[1];      /* This way the packet can be treated as pointer as usual. */

	SERVER_NODE(&broker_ipaddr);

  	/* receives all CoAP messages */
  	coap_init_engine();

	etimer_set(&periodic_timer, PERIOD);
	
	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
		if(etimer_expired(&periodic_timer)) {
			printf("--Toggle timer--\n");

			/* prepare request, TID is set by COAP_BLOCKING_REQUEST() */
			coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
			coap_set_header_uri_path(request, service_urls[1]);

			const char msg[] = "Toggle!";

			coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

			PRINT6ADDR(&broker_ipaddr);
			PRINTF(" : %u\n", UIP_HTONS(REMOTE_PORT));

			COAP_BLOCKING_REQUEST(&broker_ipaddr, REMOTE_PORT, request,
			                    client_chunk_handler);

			printf("\n--Done--\n");
			etimer_reset(&periodic_timer);
		}
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
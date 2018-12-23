#include "../commons.h"

/*---------------------------------------------------------------------------*/
PROCESS(publisher, "publisher example process");
AUTOSTART_PROCESSES(&publisher);
/*---------------------------------------------------------------------------*/

/* This function is will be passed to COAP_BLOCKING_REQUEST() to handle responses. */
void
client_chunk_handler(void *response)
{
  const uint8_t *chunk;

  int len = coap_get_payload(response, &chunk);

  printf("|%.*s\n", len, (char *)chunk);
}

#define DIM_BUF_CREATE 64
static char buf_create[DIM_BUF_CREATE]; 
void create_topic(const uip_ipaddr_t *broker_addr, 
				  const char *service_url, 
				  const char *topic_name,
				  const char *ct, 
				  coap_packet_t *request)
{  
	memset(buf_create,0,DIM_BUF_CREATE);
	/* prepare request, TID is set by COAP_BLOCKING_REQUEST() */
	coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
	coap_set_header_uri_path(request, service_url);
	snprintf(buf_create, DIM_BUF_CREATE, "<%s>;ct=%s;",topic_name,ct);

	PRINTF("msg=%s l=%d\n",buf_create,strlen(buf_create));
	coap_set_payload(request, (uint8_t *)buf_create, strlen(buf_create));
}


void 
json_temp_msg(int16_t temp, char *buf_out, uint8_t buf_size)
{
	snprintf(buf_out, buf_size, "{\"temp\" : %d, \"udm\" : \"°C\" }",temp);
}

#define PERIOD 1
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(publisher, ev, data)
{
	PROCESS_BEGIN();	
	static uint16_t i = -1;
#define DIM 48
	static char buf[DIM];
	static struct etimer periodic_timer;
	static uip_ipaddr_t broker_addr;
	coap_packet_t request[1];
	//uip_ip6addr(&broker_addr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0x0001); 
	uip_ip6addr(&broker_addr, 0x2402, 0x9400, 0x1000, 0x0007, 0, 0, 0, 0xFFFF);
	
	/* receives all CoAP messages */
  	coap_init_engine();
	printf("coap_init done\n");		  

	create_topic(&broker_addr, urls[1], "sensors", "0", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);
		
	create_topic(&broker_addr, urls[2], "accelorometer", "0", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);

	etimer_set(&periodic_timer, PERIOD*CLOCK_SECOND);	
	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(PROCESS_EVENT_TIMER);
		json_temp_msg(i,buf,DIM);
		update_topic(&broker_addr, urls[3], buf, DIM);
		//COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);
		i++; etimer_reset(&periodic_timer);
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/

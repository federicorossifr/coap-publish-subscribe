#include "../commons.h"
#include "dev/i2cmaster.h"
#include "dev/tmp102.h"

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

void 
json_temp_msg(int16_t temp, char *buf_out, uint8_t buf_size)
{
	static uint16_t sample_n = 0;
	snprintf(buf_out, buf_size, "{\"temp\" : %d, \"udm\" : \"°C. \"sa_n\" : %u \" }",temp, sample_n);
	sample_n++;
}

#define PERIOD 1
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(publisher, ev, data)
{
	PROCESS_BEGIN();	
#define DIM 48
	static char buf[DIM];
	int16_t temp;
	static struct etimer periodic_timer;
	static uip_ipaddr_t broker_addr;
	coap_packet_t request[1];
	//uip_ip6addr(&broker_addr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0x0001); 
	uip_ip6addr(&broker_addr, 0x2402, 0x9400, 0x1000, 0x0007, 0, 0, 0, 0xFFFF);
	
	/* I activate sensor for temperature */
	SENSORS_ACTIVATE(tmp102);

	/* receives all CoAP messages */
  	coap_init_engine();
	printf("coap_init done\n");		  

	create_topic(&broker_addr, urls[1], "sensors", "0", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);
		
	create_topic(&broker_addr, urls[2], "temperature", "0", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);

	create_topic(&broker_addr, urls[2], "accelerometer", "0", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);

	etimer_set(&periodic_timer, PERIOD*CLOCK_SECOND);	
	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(PROCESS_EVENT_TIMER);
		//temp = tmp102.value(TMP102_READ);
		temp = tmp102_read_temp_x100();
		json_temp_msg(temp,buf,DIM);
		update_topic(&broker_addr, urls[3], buf, DIM);
		//COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);
		etimer_reset(&periodic_timer);
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/

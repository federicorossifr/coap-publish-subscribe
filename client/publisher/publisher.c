#include "../commons.h"
#include "dev/i2cmaster.h"
#include "dev/tmp102.h"
#include "dev/adxl345.h"

/*---------------------------------------------------------------------------*/
PROCESS(publisher, "publisher example process");
AUTOSTART_PROCESSES(&publisher);
/*---------------------------------------------------------------------------*/
#define FORCE_THRESHOLD 150 //150 È SOLO UN ESEMPIO NON SO QUALE SIA LA FORZA
/* This function is will be passed to COAP_BLOCKING_REQUEST() to handle responses. */
void
client_chunk_handler(void *response)
{
	const uint8_t *chunk;
	int len = coap_get_payload(response, &chunk);
}

uint8_t
json_temp_msg(int16_t temp, char *buf_out, uint8_t buf_size)
{
	static uint16_t sample_n = 0;
	memset(buf_out,0,buf_size);
	snprintf(buf_out, buf_size, "{\"temp\":%d,\"udm\":\"°C\",sa_n\":%u\"}",temp, sample_n);
	sample_n++;
	return strlen(buf_out)+1;
}


uint8_t 
json_accm_msg(int16_t x, int16_t y, int16_t z, char *buf_out, uint8_t buf_size)
{
	memset(buf_out,0,buf_size);
	snprintf(buf_out, buf_size, "{\"x\":%d,\"y\":%d,\"z\":%d}",x ,y, z);
	return strlen(buf_out)+1;
}

uint8_t 
alarm_msg(int16_t x, int16_t y, int16_t z, char *buf_out, uint8_t buf_size)
{
	memset(buf_out,0,buf_size);
	if(y > FORCE_THRESHOLD) // Y È SOLO UN ESEMPIO, NON SO QUALE SIA L'ASSE
		snprintf(buf_out, buf_size, "____ALARM");
	else
		snprintf(buf_out, buf_size, "____ALARM");		
	return strlen(buf_out)+1;
}


#define ACCM_READ_INTERVAL   10
#define TEMP_READ_INTERVAL	 20
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(publisher, ev, data)
{
	PROCESS_BEGIN();	
#define DIM 48
	static char buf[DIM];
	uint8_t size_msg = 0;
	int16_t temp;
	int16_t x, y, z;
	static struct etimer temp_timer;
	static struct etimer acc_timer;
	static uip_ipaddr_t broker_addr;
	coap_packet_t request[1];
	SERVER_NODE(&broker_addr);
	/* I activate sensor for temperature */
	SENSORS_ACTIVATE(tmp102);
	/* Start and setup the accelerometer with default values, eg no interrupts enabled. */
  	SENSORS_ACTIVATE(adxl345);

	/* receives all CoAP messages */
  	coap_init_engine();
	printf("coap_init done\n");		  

	create_topic(&broker_addr, urls[1], "sensors", "50", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);
		
	create_topic(&broker_addr, urls[2], "temperature", "50", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);

	create_topic(&broker_addr, urls[2], "accelerometer", "50", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);

	create_topic(&broker_addr, urls[2], "intrusion", "0", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);

	etimer_set(&temp_timer, TEMP_READ_INTERVAL*CLOCK_SECOND);
	etimer_set(&acc_timer, ACCM_READ_INTERVAL*CLOCK_SECOND);

	while(1) {
		PROCESS_WAIT_EVENT();
		if( etimer_expired(&temp_timer) ) {
			temp = tmp102_read_temp_x100();
			size_msg = json_temp_msg(temp, buf, DIM);
			update_topic(&broker_addr, urls[3], buf, size_msg);
			etimer_reset(&temp_timer);
		} else if ( etimer_expired(&acc_timer) ) {
			x = adxl345.value(X_AXIS);
			y = adxl345.value(Y_AXIS);
			z = adxl345.value(Z_AXIS);
			size_msg = json_accm_msg(x, y, z, buf, DIM);
			update_topic(&broker_addr, urls[4], buf, size_msg);
			size_msg = alarm_msg(x,y,z,buf,DIM);
			update_topic(&broker_addr,urls[5],buf,size_msg);
			etimer_reset(&acc_timer);
		}
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/

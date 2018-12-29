#include "../commons.h"
#include "dev/i2cmaster.h"
#include "dev/tmp102.h"
#include "dev/adxl345.h"
#include "dev/button-sensor.h"

/*Temperature and accelerometer topics disabled because of limited rom size*/
#define TEMP_TOPIC 0
#define ACC_TOPIC 0
#define ALARM_TOPIC 1

/* Sensing interval */
#define ACCM_READ_INTERVAL   2
#define TEMP_READ_INTERVAL	 20

/* Used to take over intrusion. */
#define FORCE_THRESHOLD 20 

/*---------------------------------------------------------------------------*/
PROCESS(publisher, "publisher");
AUTOSTART_PROCESSES(&publisher);
/*---------------------------------------------------------------------------*/
/* This function is will be passed to COAP_BLOCKING_REQUEST() to handle responses. */
void
client_chunk_handler(void *response)
{
	const uint8_t *chunk;
	int len = coap_get_payload(response, &chunk);
	PRINTF("response, len%d\n",len);
}

/* Serialize temperature message in json format. */
uint8_t
json_temp_msg(int16_t temp, char *buf_out, uint8_t buf_size)
{
	static uint16_t sample_n = 0;
	memset(buf_out,0,buf_size);
	snprintf(buf_out, buf_size, "{temp:%d,udm:C,sa_n:%u}",temp, sample_n);
	sample_n++;
	return strlen(buf_out)+1;
}

/* Serialize accelerometer message in json format. */
uint8_t 
json_accm_msg(int16_t x, int16_t y, int16_t z, char *buf_out, uint8_t buf_size)
{
	memset(buf_out,0,buf_size);
	snprintf(buf_out, buf_size, "{x:%d,y:%d,z:%d}",x ,y, z);
	return strlen(buf_out)+1;
}

/* Prepare ALARM msg if accelerometer senses unexpected movement. */
uint8_t 
alarm_msg(int16_t x, int16_t y, int16_t z, char *buf_out, uint8_t buf_size)
{
	memset(buf_out,0,buf_size);
	if(y>FORCE_THRESHOLD || y<-FORCE_THRESHOLD) 
		snprintf(buf_out, buf_size, "ALARM");
	else
		snprintf(buf_out, buf_size, "QUIET");		
	return strlen(buf_out)+1;
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(publisher, ev, data)
{
	PROCESS_BEGIN();	
	uint8_t size_msg = 0;
	int16_t temp;
	int16_t x, y, z;
	static struct etimer temp_timer;
	static struct etimer acc_timer;
	static uip_ipaddr_t broker_addr;
	static uint8_t alarm_on = 0;
	coap_packet_t request[1];
	SERVER_NODE(&broker_addr);

	/* I activate sensor for temperature */
	SENSORS_ACTIVATE(tmp102);
	/* Start and setup the accelerometer with default values, eg no interrupts enabled. */
  	SENSORS_ACTIVATE(adxl345);
  	/* I activate button sensor. */
  	SENSORS_ACTIVATE(button_sensor);

	/* receives all CoAP messages */
  	coap_init_engine();
	PRINTF("coap_init done\n");		  

	create_topic(&broker_addr, urls[0], "sensors", "50", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);

	#if TEMP_TOPIC
	create_topic(&broker_addr, urls[1], "temperature", "50", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);
	#endif

	#if ACC_TOPIC
	create_topic(&broker_addr, urls[1], "accelerometer", "50", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);
	#endif

	#if ALARM_TOPIC
	create_topic(&broker_addr, urls[1], "intrusion", "0", request);
	COAP_BLOCKING_REQUEST(&broker_addr, REMOTE_PORT, request, client_chunk_handler);
	#endif

	etimer_set(&temp_timer, TEMP_READ_INTERVAL*CLOCK_SECOND);
	etimer_set(&acc_timer, ACCM_READ_INTERVAL*CLOCK_SECOND);

	while(1) {
		PROCESS_WAIT_EVENT();
		/* button is used to enable or disable alarm */
		if( ev == sensors_event && data == &button_sensor ) {
			if(alarm_on==1) {
			    PRINTF("A0\n");
			    /* disable alarm */
				size_msg = alarm_msg(0,FORCE_THRESHOLD,0,buf,DIM_BUF);
				update_topic(&broker_addr,urls[4],buf,size_msg);			   	
			    alarm_on = 0;
		    } else {
			   	PRINTF("A1\n");          
			    alarm_on = 1;
		    }
		}
		if( etimer_expired(&temp_timer) ) {
			temp = tmp102_read_temp_x100();
			#if TEMP_TOPIC
			size_msg = json_temp_msg(temp, buf, DIM_BUF);
			update_topic(&broker_addr, urls[3], buf, size_msg);
			#endif
			etimer_reset(&temp_timer);
		} else if ( etimer_expired(&acc_timer) ) {
			x = adxl345.value(X_AXIS);
			y = adxl345.value(Y_AXIS);
			z = adxl345.value(Z_AXIS);
			#if ACC_TOPIC
			size_msg = json_accm_msg(x, y, z, buf, DIM_BUF);
			update_topic(&broker_addr, urls[3], buf, size_msg);
			#endif
			#if ALARM_TOPIC
			if( alarm_on ){
				size_msg = alarm_msg(x,y,z,buf,DIM_BUF);
				update_topic(&broker_addr,urls[4],buf,size_msg);
			}
			#endif
			etimer_reset(&acc_timer);
		}
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/

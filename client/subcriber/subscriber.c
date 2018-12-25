#define SUBSCRIBER
#include "../commons.h"
#include "leds.h"
#include "dev/button-sensor.h"



#define WARMUP 20
#define OBSERVE_TIME 500
#define DIM 8
/* leading and ending slashes only for demo purposes, get cropped automatically when setting the Uri-Path */
static coap_observee_t *obsA = NULL;

static uip_ipaddr_t server_ipaddr[1]; /* holds the server ip address */


/*---------------------------------------------------------------------------*/
PROCESS(subscriber, "subscriber example process");
AUTOSTART_PROCESSES(&subscriber);
/*---------------------------------------------------------------------------
/////////////DISCOVERY/////////////////////7/7777
static char buf_create[64]; 
void get_topic(const uip_ipaddr_t *broker_addr,const char *service_url,coap_packet_t *request){  
  memset(buf_create,0,64);
  coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
  coap_set_header_uri_path(request, service_url);

  //GET /ps/sensors/accelerometer?ct=0
  mystrcat(buf_create, "?ct=0");
  printf("printing message: %s\n",buf_create ); 
  printf("get topic msg=%s l=%d\n",buf_create,strlen(buf_create));
  coap_set_payload(request, (uint8_t *)buf_create, strlen(buf_create));
}
void
client_chunk_handler(void *response)
{
  const uint8_t *chunk;

  int len = coap_get_payload(response, &chunk);

  printf("|%.*s\n", len, (char *)chunk);
}*/
//////////////SUBSCRIBE////////////////////////////////////////
//Handles the response to the observe request and the following notifications

static void handleAlarmData(const uint8_t* data,int len) {
  char buf[64];
  int i = 0;
  for(; i < len-4; ++i)buf[i] = (char)(data[i+4]);
  if(strcmp(buf,"ALARM")==0)
    leds_on(LEDS_ALL);
  else
    leds_off(LEDS_ALL);
}

static void notification_callback(coap_observee_t *obs, void *notification,coap_notification_flag_t flag){
  int len = 0;
  uint8_t *payload = NULL;
  if(notification) {
    len = coap_get_payload(notification, &payload);
    if(len > 0) {
        handleAlarmData(payload,len);
    }
  }
}
//observe the resource specified by the string uri
coap_observee_t * observe(char * uri){
  return coap_obs_request_registration(server_ipaddr, REMOTE_PORT,uri, notification_callback, NULL);
}
//unsubscribe from the specific observe resource
void remove_observe(coap_observee_t *o){
  coap_obs_remove_observee(o);
  return;
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(subscriber, ev, data){
  PROCESS_BEGIN();  
  static uint16_t i = -1;
  static char buf[DIM];
  static struct etimer periodic_timer;
  static int observing = 0;
  SENSORS_ACTIVATE(button_sensor);
  coap_init_engine();
  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == sensors_event && data == &button_sensor) {
      if(observing) {
        printf("Alarm disabled\n");
        leds_off(LEDS_ALL);
        coap_obs_remove_observee(obsA);
        obsA = NULL;
        observing = 0;
      } else {
        printf("Alarm enabled\n");
        leds_off(LEDS_ALL);
        obsA = observe((char *)urls[5]);               
        observing = 1;
      }
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

#define SUBSCRIBER
#include "../commons.h"
#include "leds.h"



#define WARMUP 20
#define OBSERVE_TIME 500
#define DIM 8
/* leading and ending slashes only for demo purposes, get cropped automatically when setting the Uri-Path */
static coap_observee_t *obsA = NULL;

static uip_ipaddr_t server_ipaddr[1]; /* holds the server ip address */


/*---------------------------------------------------------------------------*/
PROCESS(subscriber, "subscriber example process");
AUTOSTART_PROCESSES(&subscriber);
/*---------------------------------------------------------------------------*/
//////////////SUBSCRIBE////////////////////////////////////////
//Handles the response to the observe request and the following notifications

static void handleAlarmData(const uint8_t* data,int len) {
  printf("alarm: %s\n",(char*)data);
  if(strcmp((char*)data,"ALARM")==0)
    leds_on(LEDS_ALL);
  else
    leds_off(LEDS_ALL);
}

static void notification_callback(coap_observee_t *obs, void *notification,coap_notification_flag_t flag){
  int len = 0;
  int i = 0;
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
  static struct etimer periodic_timer;
  coap_init_engine();
  SERVER_NODE(server_ipaddr);
  etimer_set(&periodic_timer, WARMUP*CLOCK_SECOND);
  while(1) {
    PROCESS_WAIT_EVENT();
    if( etimer_expired(&periodic_timer)) {
      printf("observing\n");
      obsA = observe((char*)urls[4]);
    }

  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

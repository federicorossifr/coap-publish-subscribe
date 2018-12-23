#include "../commons.h"

#define PERIOD 1
#define DIM 8
/* leading and ending slashes only for demo purposes, get cropped automatically when setting the Uri-Path */
static coap_observee_t *obs = NULL;

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
static void notification_callback(coap_observee_t *obs, void *notification,coap_notification_flag_t flag){
  int len = 0;
  const uint8_t *payload = NULL;

  printf("Notification handler\n");
  printf("Observe URI: %s\n", obs->url);
  if(notification) {
    len = coap_get_payload(notification, &payload);
    printf("payload: %d\n", (int)payload);
  }
  printf("%d\n",flag);
  switch(flag) {
  case NOTIFICATION_OK:
    printf("NOTIFICATION OK: %*s\n", len, (char *)payload);
    break;
  case OBSERVE_OK: /* server accepeted observation request */
    printf("OBSERVE_OK: %*s\n", len, (char *)payload);
    break;
  case OBSERVE_NOT_SUPPORTED:
    printf("OBSERVE_NOT_SUPPORTED: %*s\n", len, (char *)payload);
    obs = NULL;
    break;
  case ERROR_RESPONSE_CODE:
    printf("ERROR_RESPONSE_CODE: %*s\n", len, (char *)payload);
    obs = NULL;
    break;
  case NO_REPLY_FROM_SERVER:
    printf("NO_REPLY_FROM_SERVER: "
           "removing observe registration with token %x%x\n",
           obs->token[0], obs->token[1]);
    obs = NULL;
    break;
  }
}
void toggle_observation(void)
{
  if(obs) {
    printf("Stopping observation\n");
    coap_obs_remove_observee(obs);
    obs = NULL;
  } else {
    printf("Starting observation\n");
    obs = coap_obs_request_registration(broker_addr, REMOTE_PORT,urls[3], notification_callback, NULL);
  }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(subscriber, ev, data){
  PROCESS_BEGIN();  
  static uint16_t i = -1;
  static char buf[DIM];
  static struct etimer periodic_timer;
  static uip_ipaddr_t broker_addr;
  //coap_packet_t request[1];
  //SERVER_NODE(broker_addr);
  uip_ip6addr(&broker_addr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0x0001); 
  //uip_ip6addr(&broker_addr, 0x2402, 0x9400, 0x1000, 0x0007, 0, 0, 0, 0xFFFF);
  
  /* receives all CoAP messages */
  coap_init_engine();
  printf("coap_init done\n");     


  etimer_set(&periodic_timer, PERIOD*CLOCK_SECOND); 
  
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(PROCESS_EVENT_TIMER);
    sprintf(buf,"%d",i);
    i++;
    toggle_observation();
    etimer_reset(&periodic_timer);
  }
    
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

#include "../commons.h"
#include "./observe.h"

#define SERVER_NODE(ipaddr)    uip_ip6addr(ipaddr, 0x2402, 0x9400, 0x1000, 0x0007, 0, 0, 0, 0xFFFF); 



#define WARMUP 60
#define OBSERVE_TIME 200
#define DIM 8
/* leading and ending slashes only for demo purposes, get cropped automatically when setting the Uri-Path */
static coap_observee_t *obsT = NULL;
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
static void notification_callback(coap_observee_t *obs, void *notification,coap_notification_flag_t flag){
  printf("Notification handler\n");
  int len = 0;
  const uint8_t *payload = NULL;
  printf("Observe URI: %s\n", obs->url);
  if(notification) {
    len = coap_get_payload(notification, &payload);
    printf("len %d\n",len);
    printf("payload: %.*s\n",len, (char*)payload);
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
//observe the resource specified by the string uri
coap_observee_t * observe(char * uri){
  printf("Starting observation\n");
  return coap_obs_request_registration(server_ipaddr, REMOTE_PORT,uri, notification_callback, NULL);
}
//unsubscribe from the specific observe resource
void remove_observe(coap_observee_t *o){
  printf("Stopping observation\n");
  coap_obs_remove_observee(o);
  return;
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(subscriber, ev, data){
  PROCESS_BEGIN();  
  static uint16_t i = -1;
  static char buf[DIM];
  static struct etimer periodic_timer;
  SERVER_NODE(server_ipaddr);

  
  /* receives all CoAP messages */
  coap_init_engine();
  //wait for the subscriber to load
  etimer_set(&periodic_timer, WARMUP*CLOCK_SECOND); 
  PROCESS_WAIT_EVENT();
  obsA = observe((char *)urls[4]); 
  obsT = observe((char *)urls[3]); 
  //after a certain period unsubscribe from observing
  etimer_set(&periodic_timer, OBSERVE_TIME*CLOCK_SECOND); 
  PROCESS_WAIT_EVENT();
  remove_observe(obsA);
  remove_observe(obsT);
  obsA = obsT = NULL;
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

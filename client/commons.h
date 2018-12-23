#ifndef COAP_PUBSUB_H
#define COAP_PUBSUB_h
#include "contiki.h"
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
const char *urls[] = {".well-known/core", "ps", "ps/sensors", "ps/sensors/accelorometer"};

void print_addresses(void){
	int i;
	for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
		uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
		printf("\n");
	}
}

void update_topic(const uip_ipaddr_t *broker_addr, 
				  char *service_url, 
				  char *new_value, 
				  uint8_t len)
{
	static coap_packet_t pkt[1];
	static uint8_t pkt_serialized[256];
	static size_t size_pkt;
	coap_init_message(pkt, COAP_TYPE_CON, COAP_PUT, 0);
	coap_set_header_uri_path(pkt, service_url);
	coap_set_payload(pkt, (uint8_t *)new_value, len);
	size_pkt = coap_serialize_message(pkt, pkt_serialized);
	coap_send_message(broker_addr, REMOTE_PORT, pkt_serialized, size_pkt);
}


#endif
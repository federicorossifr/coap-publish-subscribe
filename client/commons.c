#include "commons.h"

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

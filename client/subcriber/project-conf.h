
#ifndef PROJECT_ERBIUM_CONF_H_
#define PROJECT_ERBIUM_CONF_H_

#undef UIP_CONF_IPV6_RPL
#define UIP_CONF_IPV6_RPL 1

#undef CC2420_CONF_CHANNEL 
#define CC2420_CONF_CHANNEL 26
#undef DEBUG
#define DEBUG 0
#define PUBLISHER 0
#define SUBSCRIBER 1
#undef REST_MAX_CHUNK_SIZE
#define REST_MAX_CHUNK_SIZE    128
#undef COAP_MAX_OPEN_TRANSACTIONS
#define COAP_MAX_OPEN_TRANSACTIONS   4
#undef COAP_OBSERVE_CLIENT
#define COAP_OBSERVE_CLIENT 1	
/* Save some memory for the sky platform. */
#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS     2
#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   2
#undef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE    280
#endif /* PROJECT_ERBIUM_CONF_H_ */
#undef COAP_OBSERVE_CLIENT
#define COAP_OBSERVE_CLIENT 1
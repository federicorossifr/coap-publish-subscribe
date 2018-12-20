all: publisher
# use this target explicitly if requried: er-plugtest-server

#non so perchè ma non funziona così
#CONTIKI=~/contiki
CONTIKI=/home/user/contiki

# linker optimizations
SMALL=1

TARGET=z1

# for some platforms
UIP_CONF_IPV6=1
WITH_UIP6=1
# IPv6 make config disappeared completely
CFLAGS += -DUIP_CONF_IPV6=1 -DUIP_CONF_IPV6_RPL -DRPL_CONF_STATS=1

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

#riduce la dimensione del firmware
CFLAGS += -DUIP_CONF_TCP=0 

APPS += er-coap
APPS += rest-engine

include $(CONTIKI)/Makefile.include

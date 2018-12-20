all: publisher
# use this target explicitly if requried: er-plugtest-server

CONTIKI=/home/user/contiki

# linker optimizations
SMALL=1

TARGET=z1

APPS += er-coap
APPS += rest-engine

# for some platforms
UIP_CONF_IPV6=1
# IPv6 make config disappeared completely
CFLAGS += -DUIP_CONF_IPV6=1

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

#riduce la dimensione del firmware
CFLAGS += -DUIP_CONF_TCP=0

include $(CONTIKI)/Makefile.include

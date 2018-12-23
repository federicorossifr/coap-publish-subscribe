all: publisher
# use this target explicitly if requried: er-plugtest-server

publisher: commons.a

commons.o: ../commons.c ../commons.h
	msp430-gcc -mmcu=msp430x261 -c ../commons.c -o ../commons.o

commons.a: commons.o
	msp430-ar -cvq ../commons.a ../commons.o
	cp ../commons.a ./commons.a

CONTIKI=/home/$(USER)/contiki

# linker optimizations
SMALL=1

#TARGET=z1

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

#TARGET_LIBFILES += -L. -lcommons

include $(CONTIKI)/Makefile.include

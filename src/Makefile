CC = gcc
CFLAGS = -Wall -g -DHAVE_PCAP  -Iinclude 
LDFLAGS = -lpthread
LIBS = 

TARGET = tun.out

SOURCES = main.c tuntap.c phys.c thread.c bridge.c prot.c gum.c packet.c
OBJ = ${SOURCES:.c=.o}

all: ${TARGET}

${TARGET}: ${OBJ}
	${CC} ${LDFLAGS} -o ${TARGET} ${OBJ} ${LIBS}

clean:
	rm -rf *.o *.out

.c.o:
	${CC} ${CFLAGS} -c $< -o $@
CC=gcc
C_FLAGS=-Wall -g -Werror -Wpedantic -ggdb -pthread
L_FLAGS=-lm -lrt -lpthread

#~ server and client code
SERVER=server
SERVER_OBJ= server.o
SERVER_CODE= ./server-lib/server.c

CLIENT=client
CLIENT_OBJ= client.o
CLIENT_CODE= ./client-lib/client.c

SPECTATOR=spectator
SPECTATOR_OBJ= spectator.o
SPECTATOR_CODE= ./client-lib/spectator.c

#~ messsaging, server and client features (threads, sockets, a lot of other stuff)
COMMON_HEADER=common/common.h
COMMON_CODE=common/common.c
COMMON_OBJ=common.o

LIB_OBJ=jb_library.o
LIB_CODE=./common/jb_library.c
LIB_HEADER=./common/jb_library.h

SERVER_F_OBJ=server-handlers.o
SERVER_F_CODE=./server-lib/server-handlers.c
SERVER_F_HEADER=./server-lib/server-handlers.h

SERVER_DATA_OBJ=server-data.o
SERVER_DATA_CODE=./server-lib/server-data.c
SERVER_DATA_HEADER=./server-lib/server-data.h

CLIENT_F_OBJ=client-handlers.o
CLIENT_F_CODE=./client-lib/client-handlers.c
CLIENT_F_HEADER=./client-lib/client-handlers.h

CLIENT_DISPLAY_OBJ=client-display.o
CLIENT_DISPLAY_CODE=./client-lib/client-display.c
CLIENT_DISPLAY_HEADER=./client-lib/client-display.h

all: ${CLIENT} ${SERVER} ${SPECTATOR}

#~ linking
${SERVER}: ${SERVER_OBJ} ${SERVER_F_OBJ} ${SERVER_DATA_OBJ} ${LIB_OBJ} ${COMMON_OBJ}
	${CC} -o ${SERVER} ${SERVER_OBJ} ${SERVER_F_OBJ} ${SERVER_DATA_OBJ} ${SERVER_THREAD_OBJ} ${LIB_OBJ} ${COMMON_OBJ} ${L_FLAGS}
	
${CLIENT}: ${CLIENT_OBJ} ${CLIENT_F_OBJ} ${CLIENT_DISPLAY_OBJ} ${LIB_OBJ} ${COMMON_OBJ}
	${CC} -o ${CLIENT} ${CLIENT_OBJ} ${CLIENT_F_OBJ} ${CLIENT_DISPLAY_OBJ} ${LIB_OBJ} ${COMMON_OBJ} ${L_FLAGS}
	
${SPECTATOR}: ${SPECTATOR_OBJ} ${CLIENT_F_OBJ} ${CLIENT_DISPLAY_OBJ} ${LIB_OBJ} ${COMMON_OBJ}
	${CC} -o ${SPECTATOR} ${SPECTATOR_OBJ} ${CLIENT_F_OBJ} ${CLIENT_DISPLAY_OBJ} ${LIB_OBJ} ${COMMON_OBJ} ${L_FLAGS}

#~ compilation
${SERVER_F_OBJ}: ${SERVER_F_CODE} ${SERVER_F_HEADER} ${SERVER_DATA_HEADER} ${COMMON_HEADER}
	${CC} -o ${SERVER_F_OBJ} -c ${SERVER_F_CODE} ${C_FLAGS}
	
${SERVER_DATA_OBJ}: ${SERVER_DATA_CODE} ${SERVER_DATA_HEADER} ${COMMON_HEADER}
	${CC} -o ${SERVER_DATA_OBJ} -c ${SERVER_DATA_CODE} ${C_FLAGS}
	
${CLIENT_F_OBJ}: ${CLIENT_F_CODE} ${CLIENT_F_HEADER} ${CLIENT_DISPLAY_HEADER}
	${CC} -o ${CLIENT_F_OBJ} -c ${CLIENT_F_CODE} ${C_FLAGS}
	
${CLIENT_DISPLAY_OBJ}: ${CLIENT_DISPLAY_CODE} ${CLIENT_DISPLAY_HEADER} ${COMMON_HEADER}
	${CC} -o ${CLIENT_DISPLAY_OBJ} -c ${CLIENT_DISPLAY_CODE} ${C_FLAGS}

${COMMON_OBJ}: ${COMMON_CODE} ${COMMON_HEADER}
	${CC} -o ${COMMON_OBJ} -c ${COMMON_CODE} ${C_FLAGS}
	
${LIB_OBJ}: ${LIB_CODE} ${LIB_HEADER}
	${CC} -o ${LIB_OBJ} -c ${LIB_CODE} ${C_FLAGS}
	
${SERVER_OBJ}: ${SERVER_CODE} ${SERVER_F_HEADER}
	${CC} -o ${SERVER_OBJ} -c ${SERVER_CODE} ${C_FLAGS}
	
${CLIENT_OBJ}: ${CLIENT_CODE} ${CLIENT_F_HEADER}
	${CC} -o ${CLIENT_OBJ} -c ${CLIENT_CODE} ${C_FLAGS}
	
${SPECTATOR_OBJ}: ${SPECTATOR_CODE} ${CLIENT_F_HEADER}
	${CC} -o ${SPECTATOR_OBJ} -c ${SPECTATOR_CODE} ${C_FLAGS}

clean:
	rm -f ${SERVER} ${CLIENT} ${SPECTATOR} ${SERVER_OBJ} ${CLIENT_OBJ} ${SERVER_F_OBJ} ${SERVER_DATA_OBJ} ${CLIENT_F_OBJ} ${CLIENT_DISPLAY_OBJ} ${LIB_OBJ} ${SPECTATOR_OBJ} ${COMMON_OBJ}


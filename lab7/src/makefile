CC=gcc
CFLAGS=-I. -lpthread
BIN_DIR=bin/



all: tcp_server tcp_client udp_server udp_client

tcp_server: tcpserver.c process_options.c
	mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)$@ $^ $(CFLAGS)

tcp_client: tcpclient.c process_options.c
	mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)$@ $^ $(CFLAGS)

udp_server: udpserver.c process_options.c
	mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)$@ $^ $(CFLAGS)

udp_client: udpclient.c process_options.c
	mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)$@ $^ $(CFLAGS)

clean:
	rm -r $(BIN_DIR)

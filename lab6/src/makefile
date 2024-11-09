CC=gcc
CFLAGS=-I. -lpthread
BIN_DIR=bin/
BUILD_DIR=build/
LIB_DIR=lib/

all: server client

server: server.c libmodulo.so
	mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)$@ server.c -L$(LIB_DIR) -lmodulo $(CFLAGS)
client: client.c libmodulo.so
	mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)$@ client.c -L$(LIB_DIR) -lmodulo $(CFLAGS)

libmodulo.so: $(BUILD_DIR)modulo.o
	mkdir -p $(LIB_DIR)
	$(CC) -shared -o $(LIB_DIR)$@ $^ $(CFLAGS)

$(BUILD_DIR)modulo.o: modulo.c
	mkdir -p $(BUILD_DIR)
	$(CC) -o $@ -c $^ -fPIC $(CFLAGS)

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(LIB_DIR)
	rm -rf $(BUILD_DIR)

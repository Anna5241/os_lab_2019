#ifndef UTILS_H
#define UTILS_h 

#define UTILS_BUFFSIZE_ADDR 64


struct ServerInfo {
    int server_port;
    int buff_size;
};

struct ClientInfo {
    char server_address[UTILS_BUFFSIZE_ADDR];
    int server_port;
    int buff_size;
};

int process_options_server(int argc, char *argv[], struct ServerInfo *info);
int process_options_client(int argc, char *argv[], struct ClientInfo *info);


#endif

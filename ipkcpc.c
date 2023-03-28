////////////////////////////////////////////////////////////////////////////
//
// Modul: ipkcpc.c
//
// Autor: Michal Zapletal
// Email: xzaple41@stud.fit.vutbr.cz
//
// This modul creates terminal app for remote calculator
// Copyright (C) 2023, Zapletal Michal
//
// Source codes inspired from:
// - https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs
// - https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
// - https://www.educative.io/answers/how-to-implement-udp-sockets-in-c
// - And from pdf used in presentations in class IPK
//
///////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include "ipkcpc.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winsock.h>
#include <conio.h>
#endif // _WIN32

#ifdef __unix__
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif // unix

//#define DEBUG

// Structure and socket has to be global, because of exit
// if SIGINT is called
ConnectInfo_t * info;

#ifdef _WIN32
SOCKET client_socket;
#endif // _WIN32
#ifdef __unix__
int client_socket;
#endif // unix


/**
 * @brief Function that decodes parametrs from command line, puts them into structure
 *
 * @param argv Arguments from command line
 * @return ConnectInfo_t* Connection structure
 */
void decode_params(int argc, char *argv[]){
    info = malloc(sizeof(ConnectInfo_t));
    if(info == NULL){
        #ifdef DEBUG
        printf("[ERROR] Malloc failure\n");
        #endif
        exit(INTERNAL_ERROR);
    }
    info->IPv4 = NULL;
    info->port = NULL;
    info->connectmode = NONE_MODE;

    if(argc > 1){
        if(strcmp(argv[1],"--help") == 0){
            printf(
            "ipkcpc [options] [details] ...\n\n"
            "OPTIONS\n"
            "\t-h [IP]\n"
            "\t\tSpecify IP address of server to communicate to\n"
            "\t\t[%s by default]\n"
            "\t-m [mode]\n"
            "\t\tSpecify mode of connection. TCP or UDP\n"
            "\t\t[TCP by default]\n"
            "\t-p [port]\n"
            "\t\tSpecify port for connection.\n"
            "\t\t[%s by default]\n\n"
            "DETAILS\n"
            "\tIP\n"
            "\t\tIPv4 address in form \"x.x.x.x\"\n"
            "\t\twhere x is in range <0,255>\n"
            "\tmode\n"
            "\t\tMode of connection. TCP or UDP\n"
            "\tport\n"
            "\t\tPort where it connects to. Number in range <0,65535>\n\n"
            "Author:  Michal Zapletal\n"
            "Contact: xzaple41@stud.fit.vutbr.cz\n",DEFAULT_IPv4,DEFAULT_PORT
            );
            exit(ALL_OK);
        }

        for(int i = 1; i < argc ; ){
            if(strcmp(argv[i],"-h") == 0){
                decode_ip(argv[i+1]);
            }else if(strcmp(argv[i],"-m") == 0){
                decode_mode(argv[i+1]);
            }else if(strcmp(argv[i],"-p") == 0){
                decode_port(argv[i+1]);
            }else{
                #ifdef DEBUG
                printf("[ERROR] Unknown settings\"%s\"\n",argv[i]);
                #endif
                exit(PARAM_ERROR);
            }
            i = i+2;
        }
    }

    //Setting default values if not declared while executing
    if(info->connectmode == NONE_MODE){
        info->connectmode = TCP;
    }
    if(info->IPv4 == NULL){
        info->IPv4 = calloc(sizeof(char),strlen(DEFAULT_IPv4)+1);
        memcpy(info->IPv4,DEFAULT_IPv4,strlen(DEFAULT_IPv4)+1);
    }
    if(info->port == NULL){
        info->port = calloc(sizeof(char),strlen("2023")+1);
        memcpy(info->port,"2023",strlen("2023")+1);
    }
}

/**
 * @brief Function that decodes port, saves port into structure
 *
 * @param port String with port
 */
void decode_port(char * port){
    if(info->port != NULL){
        #ifdef DEBUG
        printf("[ERROR] Double declaration of port\n");
        #endif
        exit(PARAM_ERROR);
    }
    unsigned length = strlen(port);
    if(length > MAX_PORT_CHAR_LENGTH){
        #ifdef DEBUG
        printf("[ERROR] Port contains more than 5 numbers\n");
        #endif
        exit(PARAM_ERROR);
    }
    for(unsigned i = 0; i < length; i++){
        if(!isdigit(port[i])){
            #ifdef DEBUG
            printf("[ERROR] Port must contain only numbers\n");
            #endif
            exit(PARAM_ERROR);
        }
    }
    int portnumber = atoi(port);
    if(portnumber > MAX_PORT_NUM){
        #ifdef DEBUG
        printf("[ERROR] Port number is out of range\n");
        #endif
        exit(PARAM_ERROR);
    }
    info->port = calloc(length+1,sizeof(char));
    if(info->port == NULL){
        #ifdef DEBUG
        printf("[ERROR] Malloc failure\n");
        #endif
        exit(INTERNAL_ERROR);
    }
    memcpy(info->port,port,length);
}

/**
 * @brief Function that decodes mode, saves mode into structure
 *
 * @param mode String with mode
 */
void decode_mode(char * mode){
    if(info->connectmode != NONE_MODE){
        #ifdef DEBUG
        printf("[ERROR] Double declaration of mode\n");
        #endif
        exit(PARAM_ERROR);
    }
    unsigned length = strlen(mode);
    if(length == MODE_CHAR_LENGTH){
        for(unsigned i = 0; i < length; i++){
            if((mode[i] >= 'a' && mode[i] <= 'z') || (mode[i] >= 'A' && mode[i] <= 'Z')){
                mode[i] = toupper(mode[i]);
                continue;
            }else{
                #ifdef DEBUG
                printf("[ERROR] Mode do not contain only characters\n");
                #endif
                exit(PARAM_ERROR);
            }
        }
        if(strcmp(mode,"UDP") == 0){
            info->connectmode = UDP;
        }else if(strcmp(mode,"TCP") == 0){
            info->connectmode = TCP;
        }else{
            #ifdef DEBUG
            printf("[ERROR] Mode has to be UDP or TCP\n");
            #endif
            exit(PARAM_ERROR);
        }
    }else{
        #ifdef DEBUG
        printf("[ERROR] Mode has to be UDP or TCP\n");
        #endif
        exit(PARAM_ERROR);
    }
}

/** Function that decodes ip address.
 *
 * Function check if ip is in right form and puts it into struct
 *
 * @param ip String with ip inside
 */
void decode_ip(char *ip){
    if(info->IPv4 != NULL){
        #ifdef DEBUG
        printf("[ERROR] Double declaration of IPv4\n");
        #endif
        exit(PARAM_ERROR);
    }
    unsigned length = strlen(ip);
    unsigned i = 0;
    unsigned max_range = MAX_IP_COMPONENT_LENGTH; // Boundaries
    unsigned dots_counter = 0;
    for(unsigned j = 0; j < MAX_IP_NUM_OF_COMPONENT; j++){
        if(j != dots_counter){
            #ifdef DEBUG
            printf("[ERROR] IPv4 dont have all elements\n");
            #endif
            exit(PARAM_ERROR);
        }
        for( ; i < length; i++){
            if(!isdigit(ip[i])){
                if(ip[i] == '.'){
                    i++;
                    max_range = i + MAX_IP_COMPONENT_LENGTH;
                    dots_counter++;
                    break;
                }else{
                    #ifdef DEBUG
                    printf("[ERROR] IP address is not created from digits and dots\n");
                    #endif
                    exit(PARAM_ERROR);
                }
            }


            if(i == max_range){
                #ifdef DEBUG
                printf("[ERROR] Wrong IP address\n");
                #endif
                exit(PARAM_ERROR);
            }
        }
    }
    info->IPv4 = calloc(length+1,sizeof(char));
    if(info->IPv4 == NULL){
        #ifdef DEBUG
        printf("[ERROR] Malloc failure\n");
        #endif
        exit(INTERNAL_ERROR);
    }
    memcpy(info->IPv4,ip,length);
    check_ip(info->IPv4);
}

/**
 * @brief Function that check boundaries of IP, if all numbers
 * are in range <0,255>
 *
 * @param ip String with ip address
 */
void check_ip(char * ip){
    int length = 0;
    char * to_int = malloc((length+1)*sizeof(char));
    if(to_int == NULL){
        #ifdef DEBUG
        printf("[ERROR] Malloc failure\n");
        #endif
        exit(INTERNAL_ERROR);
    }
    for(unsigned i = 0; i <= strlen(ip); i++){
        if(isdigit(ip[i])){
            length++;
            char * new_to_int = realloc(to_int,(length+1)*sizeof(char));
            if(new_to_int == NULL){
                #ifdef DEBUG
                printf("[ERROR] Realloc failure\n");
                #endif
                exit(INTERNAL_ERROR);
            }
            to_int = new_to_int;
            to_int[length-1] = ip[i];
            to_int[length] = '\0';
        }else{
            int var = atoi(to_int);
            if(var > MAX_IP_COMPONENT_VALUE){
                #ifdef DEBUG
                printf("[ERROR] Value of IP out of range <0,255>\n");
                #endif
                exit(PARAM_ERROR);
            }
            length = 0;
        }
    }
    free(to_int);
}

/**
 * @brief Connection struct dectructor
 */
void struct_dtor(){
    if(info != NULL){
        if(info->IPv4 != NULL){
            free(info->IPv4);
        }
        if(info->port != NULL){
            free(info->port);
        }
        free(info);
    }
}

/**
 * @brief Create a socket for TCP connection
 *
 * @return Server socket
 */
void create_socket_tcp(){
    int family = AF_INET;
    int type = SOCK_STREAM;
    #ifdef _WIN32
    WSADATA wsa;
    char c = 0;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        #ifdef DEBUG
        printf("[ERROR] wsa\n");
        #endif
        exit(WSA_FAILURE);
    }
    client_socket = socket(family, type, IPPROTO_TCP);
    (void)c;
    #endif
    #ifdef __unix__
    client_socket = socket(family, type, 0);
    #endif
    if(client_socket <= 0){
        #ifdef DEBUG
        printf("[ERROR] Socket failure\n");
        #endif
        exit(SOCKET_FAILURE);
    }

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(info->port));
    server_addr.sin_addr.s_addr = inet_addr(info->IPv4);

    if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        #ifdef DEBUG
        printf("[ERROR] Unable to connect\n");
        #endif
        exit(CONNECTION_FAILURE);
    }
}

/**
 * @brief Create connection with declared server, on declared port, TCP.
 * In infinite loop sending messages to server until connection is closed
 */
void send_message_tcp(){
    char client_message[MAX_MESSAGE_LENGTH];
    char server_message[MAX_MESSAGE_LENGTH];
    char message_to_print[MAX_MESSAGE_LENGTH];

    create_socket_tcp();

    while(true){
        memset(client_message,'\0',MAX_MESSAGE_LENGTH);
        memset(server_message,'\0',MAX_MESSAGE_LENGTH);
        memset(message_to_print,'\0',MAX_MESSAGE_LENGTH);
        for(int i = 0; ; i++){
            int c = fgetc(stdin);
            client_message[i] = c;
            if(i == MAX_MESSAGE_LENGTH-1){
                #ifdef DEBUG
                printf("[ERROR] Input message is too long!\n");
                exit(TOO_LONG_MESSAGE);
                #endif
            }
            if(c == '\n'){
                break;
            }
        }
        int bytes_tx = send(client_socket,client_message,strlen(client_message), 0);
        if(bytes_tx < 0){
            #ifdef DEBUG
            printf("[ERROR] sendto failure\n");
            #endif
            exit(SENDTO_FAILURE);
        }
        // TCP will recieve until LF is read
        while(true){
            int bytes_rx = recv(client_socket,server_message,MAX_MESSAGE_LENGTH,0);
            if(bytes_rx < 0){
                #ifdef DEBUG
                printf("[ERROR] Recieve Failure\n");
                #endif
                exit(RECIEVE_FAILURE);
            }
            strcat(message_to_print,server_message);
            memset(server_message,'\0',MAX_MESSAGE_LENGTH);
            if(message_to_print[strlen(message_to_print)-1] == '\n'){
                break;
            }
        }
        if(strcmp(message_to_print,SERVER_ENDING_MESSAGE) == 0){
            printf("%s",message_to_print);
            break;
        }else{
            printf("%s",message_to_print);
        }
    }

    close(client_socket);
    #ifdef _WIN32
    WSACleanup();
    #endif
}
/**
 * @brief Create a socket for udp communication
 * 
 */
void create_socket_udp(){
    #ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        #ifdef DEBUG
        printf("[ERROR] wsa\n");
        #endif
        exit(WSA_FAILURE);
    }
    #endif

    client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(client_socket <= 0){
        #ifdef DEBUG
        printf("[ERROR] Socket failure\n");
        #endif
        exit(SOCKET_FAILURE);
    }

    struct timeval timeout;
    timeout.tv_sec = UDP_TIMEOUT;
    timeout.tv_usec = 0;

    #ifdef __unix__
    int sock_opt = setsockopt(client_socket,SOL_SOCKET, SO_RCVTIMEO,&timeout,sizeof(timeout));
    #endif
    #ifdef _WIN32
    int sock_opt = setsockopt(client_socket,SOL_SOCKET, SO_RCVTIMEO,(const char *)&timeout,sizeof(timeout));
    #endif
    if(sock_opt < 0){
        #ifdef DEBUG
        printf("[ERROR] Set socket option failure\n");
        #endif
        exit(SOCKET_FAILURE);
    }
}
/**
 * @brief Function that send socket with UDP mode
 * 
 */
void send_message_udp(){
    create_socket_udp();

    struct hostent * server = gethostbyname(info->IPv4);
    if(server == NULL){
        #ifdef DEBUG
        printf("[ERROR] Server IP error\n");
        #endif
        exit(SENDTO_FAILURE);
    }

    struct sockaddr_in server_address;
    memset(&server_address,0,sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(info->port));
    server_address.sin_addr.s_addr = inet_addr(info->IPv4);
    socklen_t addr_size = sizeof(server_address);

    char client_message[MAX_MESSAGE_LENGTH];
    char server_message[MAX_MESSAGE_LENGTH];

    while(true){
        memset(client_message,'\0',MAX_MESSAGE_LENGTH);
        memset(server_message,'\0',MAX_MESSAGE_LENGTH);
        for(int i = UDP_SKIP_BYTES_SEND; ; i++){
            int c = fgetc(stdin);
            if(i == MAX_MESSAGE_LENGTH-1){
                #ifdef DEBUG
                printf("[ERROR] Input message is too long!\n");
                exit(TOO_LONG_MESSAGE);
                #endif
            }
            if(c == '\n'){
                break;
            }
            client_message[i] = c;
        }
        client_message[UDP_OPCODE_POS] = UDP_OPCODE_REQUEST;
        client_message[1] = strlen(&(client_message[UDP_SKIP_BYTES_SEND]));
        int bytes_tx = sendto(client_socket,client_message,
                            strlen(&(client_message[UDP_SKIP_BYTES_SEND]))+UDP_SKIP_BYTES_SEND,
                            0,(struct sockaddr *)&server_address,addr_size);
        if(bytes_tx < 0){
            #ifdef DEBUG
            printf("[ERROR] sendto failure\n");
            #endif
            exit(SENDTO_FAILURE);
        }

        int bytes_rx = recvfrom(client_socket,server_message,MAX_MESSAGE_LENGTH,0,
                            (struct sockaddr *)&server_address,&addr_size);
        if(bytes_rx < 0){
            #ifdef DEBUG
            printf("[ERROR] Recieve Failure\n");
            #endif
            exit(RECIEVE_FAILURE);
        }
        if(server_message[UDP_OPCODE_POS] == UDP_OPCODE_REQUEST){
            printf("ERR: Expected message from server with opcode \"RESPONSE\"\n");
        }else if(server_message[UDP_OPCODE_POS] == UDP_OPCODE_RESPONSE){
            if(server_message[UDP_STATUS_CODE_POS] == UDP_STATUS_CODE_OK){
                printf("OK:%s\n",&(server_message[UDP_SKIP_BYTES_RECV]));
            }else if(server_message[UDP_STATUS_CODE_POS] == UDP_STATUS_CODE_ERR){
                printf("ERR:%s\n",&(server_message[UDP_SKIP_BYTES_RECV]));
            }else{
                #ifdef DEBUG
                printf("[ERROR] Wrong OPCODE\n");
                #endif
                exit(WRONG_STATUS_CODE);
            }
        }else{
            #ifdef DEBUG
            printf("[ERROR] Wrong OPCODE\n");
            #endif
            exit(WRONG_OPCODE);
        }
    }
}

/**
 * @brief Function for handling interruption
 * 
 * @param _ EMPTY
 */
static void sigint_handler(int _){
    signal(SIGINT, sigint_handler);
    if(info->connectmode == TCP){
        char client_message[MAX_MESSAGE_LENGTH] = SERVER_ENDING_MESSAGE;
        char server_message[MAX_MESSAGE_LENGTH];
        char message_to_print[MAX_MESSAGE_LENGTH];
        memset(server_message,'\0',MAX_MESSAGE_LENGTH);
        memset(message_to_print,'\0',MAX_MESSAGE_LENGTH);
        int bytes_tx = send(client_socket,client_message,strlen(client_message), 0);
        if(bytes_tx < 0){
            #ifdef DEBUG
            printf("[ERROR] sendto failure\n");
            #endif
            exit(SENDTO_FAILURE);
        }
        while(true){
            int bytes_rx = recv(client_socket,server_message,MAX_MESSAGE_LENGTH,0);
            if(bytes_rx < 0){
                #ifdef DEBUG
                printf("[ERROR] Recieve Failure\n");
                #endif
                exit(RECIEVE_FAILURE);
            }
            strcat(message_to_print,server_message);
            memset(server_message,'\0',MAX_MESSAGE_LENGTH);
            if(message_to_print[strlen(message_to_print)-1] == '\n'){
                printf("\n%s",message_to_print);
                break;
            }
        }

        close(client_socket);
    }
    #ifdef _WIN32
    WSACleanup();
    #endif
    struct_dtor();
    (void)_;
    exit(ALL_OK);
}

int main(int argc, char *argv[]){
    signal(SIGINT, sigint_handler);

    decode_params(argc,argv);

    #ifdef DEBUG
    printf("IP:   %s\n",info->IPv4);
    printf("PORT: %s\n",info->port);
    if(info->connectmode == UDP){
        printf("MODE: UDP\n");
    }else if(info->connectmode == TCP){
        printf("MODE: TCP\n");
    }else{
        printf("MODE: none\n");
    }
    #endif

    if(info->connectmode == TCP){
        send_message_tcp(info);
    }else{
        send_message_udp(info);
    }

    struct_dtor();
    exit(ALL_OK);
}

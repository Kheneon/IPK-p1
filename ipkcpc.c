//////////////////////////////////////
//
// Modul: ipkcpc.c
//
// Autor: Michal Zapletal
// Email: xzaple41@stud.fit.vutbr.cz
//
//////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "ipkcpc.h"

#define DEBUG
/**
 * @brief Function that decodes parametrs from command line, puts them into structure
 * 
 * @param argv Arguments from command line
 * @return ConnectInfo_t* Connection structure
 */
ConnectInfo_t * decode_params(int argc, char *argv[]){
    ConnectInfo_t * info;
    info = malloc(sizeof(ConnectInfo_t)); 
    if(info == NULL){
        #ifdef DEBUG
        printf("[ERROR] Malloc failure\n");
        #endif
        exit(INTERNAL_ERROR);
    }
    info->IPv4 = NULL;
    info->port = NULL;

    if(argc < 2){
        return info;
    }

    if(strcmp(argv[1],"--help") == 0){
        printf(
        "ipkcpc [options] [details] ...\n\n"
        "OPTIONS\n"
        "\t-h [IP]\n"
        "\t\tSpecify IP address of server to communicate to\n"
        "\t\t[0.0.0.0 by default]\n"
        "\t-m [mode]\n"
        "\t\tSpecify mode of connection. TCP or UDP\n"
        "\t\t[TCP by default]\n"
        "\t-p [port]\n"
        "\t\tSpecify port for connection.\n"
        "\t\t[2023 by default]\n\n"
        "DETAILS\n"
        "\tIP\n"
        "\t\tIPv4 address in form \"x.x.x.x\"\n"
        "\t\twhere x is in range <0,255>\n"
        "\tmode\n"
        "\t\tMode of connection. TCP or UDP\n"
        "\tport\n"
        "\t\tPort where it connects to. Number in range <0,65535>\n\n"
        "Author:  Michal Zapletal\n"
        "Contact: xzaple41@stud.fit.vutbr.cz\n"
        );
        exit(ALL_OK);
    }

    for(int i = 1; i < argc ;){
        if(strcmp(argv[i],"-h") == 0){
            decode_ip(info, argv[i+1]);
        }else if(strcmp(argv[i],"-m") == 0){
            decode_mode(info,argv[i+1]);
        }else if(strcmp(argv[i],"-h") == 0){
            decode_port(info,argv[i+1]);
        }else{
            #ifdef DEBUG
            printf("[ERROR] Unknown settings\"%s\"\n",argv[i]);
            #endif
            exit(PARAM_ERROR);
        }
        i = i+2;
    }
    return info;
}

/**
 * @brief Function that decodes port, saves port into structure
 * 
 * @param info Connection structure
 * @param port String with port
 */
void decode_port(ConnectInfo_t * info, char * port){
    unsigned maxportlength = 5; //Maximal port number is 65535, so it contains 5 numbers
    unsigned length = strlen(port);
    for(unsigned i = 0; i < length; i++){
        if(!isdigit(port[i])){
            #ifdef DEBUG
            printf("[ERROR] Port must contain only numbers\n");
            #endif
            exit(PARAM_ERROR);
        }
        if(i == maxportlength){
            #ifdef DEBUG
            printf("[ERROR] Port contains more than 5 numbers\n");
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
    strncpy(info->port,port,length);
}

/**
 * @brief Function that decodes mode, saves mode into structure
 * 
 * @param info Connection structure
 * @param mode String with mode
 */
void decode_mode(ConnectInfo_t * info, char * mode){
    unsigned length = strlen(mode);
    if(length == 3){
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
 * @param info Struct with informations about connection
 * @param ip String with ip inside
 */
void decode_ip(ConnectInfo_t * info, char *ip){
    unsigned length = strlen(ip);
    unsigned i = 0;
    unsigned range = 3; // Maximal number of digits in ip address element
    unsigned maxrange = range; // Boundaries
    unsigned elements = 4; // Elements of IPv4 address, like XX.XX.XX.XX
    unsigned dots_counter = 0;
    for(unsigned j = 0; j < elements; j++){
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
                    maxrange = i + range;
                    dots_counter++;
                    break;
                }else{
                    #ifdef DEBUG
                    printf("[ERROR] IP address is not created from digits and dots\n");
                    #endif
                    exit(PARAM_ERROR);
                }
            }


            if(i == maxrange){
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
    strncpy(info->IPv4,ip,length);
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
    char * toint = malloc((length+1)*sizeof(char));
    if(toint == NULL){
        #ifdef DEBUG
        printf("[ERROR] Malloc failure\n");
        #endif
        exit(INTERNAL_ERROR);
    }
    for(unsigned i = 0; i <= strlen(ip); i++){
        if(isdigit(ip[i])){
            length++;
            char * newtoint = realloc(toint,(length+1)*sizeof(char));
            if(newtoint == NULL){
                #ifdef DEBUG
                printf("[ERROR] Realloc failure\n");
                #endif
                exit(INTERNAL_ERROR);
            }
            toint = newtoint;
            toint[length-1] = ip[i];
            toint[length] = '\0';
        }else{
            int var = atoi(toint);
            if(var > 255){
                #ifdef DEBUG
                printf("[ERROR] Value of IP out of range <0,255>\n");
                #endif
                exit(PARAM_ERROR);
            }
            length = 0;
        }
    }
    free(toint);
}

/**
 * @brief Connection struct dectructor
 * 
 * @param info Connection struct
 */
void struct_dtor(ConnectInfo_t * info){
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

int create_socket(){
    int family = AF_INET;
    int type = SOCK_STREAM;
    int client_socket = socket(family, type, 0);
    if(client_socket <= 0){
        #ifdef DEBUG
        printf("[ERROR] Socket failure\n");
        #endif
        exit(SOCKET_FAILURE);
    }
    return client_socket;
}

/**
 * @brief 
 * 
 * @param info Connection structure
 * @param message Message to be sent
 */
void send_message_tcp(ConnectInfo_t * info, char * message){
    int client_socket = create_socket();

    struct sockaddr_in server_addr;
    socklen_t address_size;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = &(info->port);
    server_addr.sin_addr.s_addr = inet_addr(info->IPv4);

    if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Unable to connect\n");
        return -1;
    }
    printf("Connected with server successfully\n");

    /*struct hostent *server = gethostbyname(info->IPv4);
    if(server == NULL){
        #ifdef DEBUG
        printf("[ERROR] Get host name failure");
        #endif
        exit(GET_HOST_NAME_FAILURE);
    }

    struct sockaddr_in server_adress;

    struct sockaddr * address = (struct sockaddr *) &(server_adress);
    int address_size = sizeof(server_adress);
    int flags = 0;
    int bytes_tx = sendto(client_socket,message,strlen(message),flags,address,address_size);
    if(bytes_tx < 0){
        #ifdef DEBUG
        printf("[ERROR] sendto failure\n");
        #endif
        exit(SENDTO_FAILURE);
    }*/
}

int main(int argc, char *argv[]){
    ConnectInfo_t * info;
    info = decode_params(argc,argv);

    #ifdef DEBUG
    printf("IP:   %s\n",info->IPv4);
    printf("PORT: %s\n",info->port);
    if(info->connectmode == UDP){
        printf("MODE: UDP\n");
    }else{
        printf("MODE: TCP\n");
    }
    #endif

    send_message_tcp(info,"HELLO");


    struct_dtor(info);
    exit(ALL_OK);
}
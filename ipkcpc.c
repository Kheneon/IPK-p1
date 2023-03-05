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
#include "ipkcpc.h"

#define DEBUG
/**
 * @brief Function that decodes parametrs from command line, puts them into structure
 * 
 * @param argv Arguments from command line
 * @return ConnectInfo_t* Connection structure
 */
ConnectInfo_t * decode_params(char *argv[]){
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
    if(strcmp(argv[1],"-h") == 0){
        decode_ip(info,argv[2]);
        if(strcmp(argv[3],"-m") == 0){
            decode_mode(info,argv[4]);
            if(strcmp(argv[5],"-p") == 0){
                decode_port(info,argv[6]);
            }else{
                #ifdef DEBUG
                printf("[ERROR] Unknown setting \"%s\"\n",argv[3]);
                #endif
                exit(PARAM_ERROR);
            }
        }else if(strcmp(argv[3],"-p") == 0){
            decode_port(info,argv[3]);
            if(strcmp(argv[5],"-m") == 0){
                decode_mode(info,argv[6]);
            }else{
                #ifdef DEBUG
                printf("[ERROR] Unknown setting \"%s\"\n",argv[3]);
                #endif
                exit(PARAM_ERROR);
            }
        }else{
            #ifdef DEBUG
            printf("[ERROR] Unknown setting \"%s\"\n",argv[3]);
            #endif
            exit(PARAM_ERROR);
        }
    }else if(strcmp(argv[1],"-m") == 0){
        decode_mode(info,argv[2]);
        if(strcmp(argv[3],"-h") == 0){
            decode_ip(info,argv[4]);
            if(strcmp(argv[5],"-p") == 0){
                decode_port(info,argv[6]);
            }else{
                #ifdef DEBUG
                printf("[ERROR] Unknown setting \"%s\"\n",argv[3]);
                #endif
                exit(PARAM_ERROR);
            }
        }else if(strcmp(argv[3],"-p") == 0){
            decode_port(info,argv[3]);
            if(strcmp(argv[5],"-h") == 0){
                decode_ip(info,argv[6]);
            }else{
                #ifdef DEBUG
                printf("[ERROR] Unknown setting \"%s\"\n",argv[3]);
                #endif
                exit(PARAM_ERROR);
            }
        }else{
            #ifdef DEBUG
            printf("[ERROR] Unknown setting \"%s\"\n",argv[3]);
            #endif
            exit(PARAM_ERROR);
        }
    }else if(strcmp(argv[1],"-p") == 0){
        decode_ip(info,argv[2]);
        if(strcmp(argv[3],"-m") == 0){
            decode_mode(info,argv[4]);
            if(strcmp(argv[5],"-h") == 0){
                decode_ip(info,argv[6]);
            }else{
                #ifdef DEBUG
                printf("[ERROR] Unknown setting \"%s\"\n",argv[3]);
                #endif
                exit(PARAM_ERROR);
            }
        }else if(strcmp(argv[3],"-h") == 0){
            decode_ip(info,argv[3]);
            if(strcmp(argv[5],"-m") == 0){
                decode_mode(info,argv[6]);
            }else{
                #ifdef DEBUG
                printf("[ERROR] Unknown setting \"%s\"\n",argv[3]);
                #endif
                exit(PARAM_ERROR);
            }
        }else{
            #ifdef DEBUG
            printf("[ERROR] Unknown setting \"%s\"\n",argv[3]);
            #endif
            exit(PARAM_ERROR);
        }
    }else{
        #ifdef DEBUG
        printf("[ERROR] Unknown setting \"%s\"\n",argv[1]);
        #endif
        exit(PARAM_ERROR);
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

void send_socket(ConnectInfo_t * info, char * message){
    int family = AF_INET;
    int type = SOCK_DGRAM;
    int client_socket = socket(family, type, 0);
    if(client_socket <= 0){
        #ifdef DEBUG
        printf("[ERROR] Socket failure\n");
        #endif
        exit(SOCKET_FAILURE);
    }

    struct sockaddr * address = (struct sockaddr *) &(info->IPv4);
    int address_size = sizeof(info->IPv4);
    int flags = 0;
    int bytes_tx = sendto(client_socket,message,strlen(message),flags,address,address_size);
    if(bytes_tx < 0){
        #ifdef DEBUG
        printf("[ERROR] sendto failure\n");
        #endif
        exit(SENDTO_FAILURE);
    }
}

int main(int argc, char *argv[]){
    if(argc < 7 || argc > 7){
        #ifdef DEBUG
        printf("[ERROR] Wrong number of parameters\n");
        #endif
        exit(PARAM_ERROR);
    }
    ConnectInfo_t * info;
    info = decode_params(argv);

    #ifdef DEBUG
    printf("IP:   %s\n",info->IPv4);
    printf("PORT: %s\n",info->port);
    if(info->connectmode == UDP){
        printf("MODE: UDP\n");
    }else{
        printf("MODE: TCP\n");
    }
    #endif

    send_socket(info,"HELLO");


    struct_dtor(info);
    exit(ALL_OK);
}
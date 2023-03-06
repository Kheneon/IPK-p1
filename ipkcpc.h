//////////////////////////////////////
//
// Modul: ipkcpc.h
//
// Autor: Michal Zapletal
// Email: xzaple41@stud.fit.vutbr.cz
//
//////////////////////////////////////

#ifndef _IPK_PROJ1_LIB_
#define _IPK_PROJ1_LIB_

#define MAX_PORT_NUM 65535
typedef enum{
    UDP,
    TCP
}MODE;

typedef enum{
    ALL_OK,
    PARAM_ERROR,
    SOCKET_FAILURE,
    SENDTO_FAILURE,
    GET_HOST_NAME_FAILURE,
    INTERNAL_ERROR = 99
} EXIT_STATUSES;

typedef struct ConnectionInfo ConnectInfo_t;
struct ConnectionInfo{
    char *IPv4;
    char *port;
    MODE connectmode;
};

void decode_ip(ConnectInfo_t * info, char *ip);
ConnectInfo_t * decode_params(int argc,char *argv[]);
void struct_dtor(ConnectInfo_t * info);
void decode_port(ConnectInfo_t * info, char * port);
void decode_mode(ConnectInfo_t * info, char * mode);
void check_ip(char * ip);

#endif
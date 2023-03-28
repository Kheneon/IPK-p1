/////////////////////////////////////////////////////////
//
// Modul: ipkcpc.h
//
// Autor: Michal Zapletal
// Email: xzaple41@stud.fit.vutbr.cz
//
// Library for ipkcpc.c file
// Copyright (C) 2023, Zapletal Michal
//
/////////////////////////////////////////////////////////

#ifndef _IPK_PROJ1_LIB_
#define _IPK_PROJ1_LIB_

// Port number (from his definition, 16-bit number) has to be in range <0,65535>
#define MAX_PORT_NUM 65535

// Randomly choosed maximal length of message that can be sent(in bytes)
#define MAX_MESSAGE_LENGTH 256

// Default port to connect to, same value as reference server has as default
#define DEFAULT_PORT "2023"

// Default IPv4 to connect to, same value as reference server has as default
#define DEFAULT_IPv4 "0.0.0.0"

// Definiton of maximal length(chars) of port
#define MAX_PORT_CHAR_LENGTH 5

// TCP or UDP are created by 3 characters
#define MODE_CHAR_LENGTH 3

/** Components of IP address are created of numbers, IP address is X.X.X.X,
 * where X is in range <0,255>, where 255 is length of 3
 */
#define MAX_IP_COMPONENT_LENGTH 3

// IPv4 address is in shape X.X.X.X, wher X is four times
#define MAX_IP_NUM_OF_COMPONENT 4

// Components of IP address are in range <0,255>
#define MAX_IP_COMPONENT_VALUE 255

// TCP connection is terminated by server by sending this message
#define SERVER_ENDING_MESSAGE "BYE\n"

// From its definition (described in documentation) we skip first 3 bytes from recieved message
#define UDP_SKIP_BYTES_RECV 3

// From its definition (described in documentation) we skip first 2 bytes from sending message
#define UDP_SKIP_BYTES_SEND 2

// Definition of opcodes
#define UDP_OPCODE_REQUEST 0
#define UDP_OPCODE_RESPONSE 1

// Definiton of status codes
#define UDP_STATUS_CODE_OK 0
#define UDP_STATUS_CODE_ERR 1

// Positions of STATUS CODES and OPCODES
#define UDP_OPCODE_POS 0
#define UDP_STATUS_CODE_POS 1

// Timeout for udp connection, in seconds
#define UDP_TIMEOUT 5

// Enum for connection types
typedef enum{
    UDP,
    TCP,
    NONE_MODE
}MODE;

// Error numbers
typedef enum{
    ALL_OK,
    PARAM_ERROR,
    SOCKET_FAILURE,
    SENDTO_FAILURE,
    CONNECTION_FAILURE,
    RECIEVE_FAILURE,
    WRONG_OPCODE,
    WRONG_STATUS_CODE,
    WSA_FAILURE,
    TOO_LONG_MESSAGE,
    INTERNAL_ERROR = 99
} EXIT_STATUSES;

/**
 * @brief Structure that saves communication details
 * 
 * @param IPv4 IPv4 address
 * @param port port number
 * @param connectmode TCP or UDP mode
 */
typedef struct ConnectionInfo ConnectInfo_t;
struct ConnectionInfo{
    char *IPv4;
    char *port;
    MODE connectmode;
};

void decode_params(int argc,char *argv[]);
void decode_port(char * port);
void decode_mode(char * mode);
void decode_ip(char *ip);
void check_ip(char * ip);
void struct_dtor();
void create_socket_tcp();
void send_message_tcp();
void create_socket_udp();
void send_message_udp();
static void sigint_handler(int _);

#endif
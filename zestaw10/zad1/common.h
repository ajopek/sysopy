#ifndef LAB10_COMMON_H
#define LAB10_COMMON_H

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netinet/in.h>

#define OK(_EXPR, _ERR_MSG) if((_EXPR) < 0) { fprintf(stderr, "%s: %d %s\n", _ERR_MSG, errno, strerror(errno)); exit(1); }
#define CHECK(_EXPR, _ERR_MSG) if((_EXPR) < 0) { fprintf(stderr, "%s: %d %s\n", _ERR_MSG, errno, strerror(errno)); }

#define UNIX_PATH_MAX 108
#define MAX_NAME_LEN 23
#define MAX_CLIENTS 20
#define MAX_LEN 64
#define MAX_TOKENS 5
#define PING_FREQ 10

typedef enum
{
    REGISTER = 0x11,
    REGISTER_ACK = 0x22,
    NAME_TAKEN = 0x33,
    ARITH = 0x44,
    RESULT = 0x55,
    UNREGISTER = 0x66,
    PING = 0x77,
} msg_type;

typedef enum arith_op
{
    ADD,
    MUL,
    SUB,
    DIV
} arith_op;

typedef struct
{
    uint16_t type;
    uint32_t len;
    char client_name[MAX_NAME_LEN + 1];
    uint32_t data[0];
} message;

typedef struct
{
    int32_t id;
    int32_t op;
    int32_t arg1;
    int32_t arg2;
} arith_request;

typedef struct
{
    int32_t id;
    int32_t result;
} arith_response;


#endif //LAB10_COMMON_H

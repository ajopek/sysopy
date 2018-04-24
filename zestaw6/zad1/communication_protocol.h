//
// Created by artur on 21.04.18.
//

#ifndef ZESTAW6_COMMUNICATION_PROTOCOL_H
#define ZESTAW6_COMMUNICATION_PROTOCOL_H


#include <fcntl.h>

#define MAX_DATA_LEN 50
#define PROJECT_ID 's'
#define MSG_SIZE sizeof(message)

typedef enum reqeuest_type {
    Mirror = 1,
    Calc = 2,
    Time = 3,
    End = 4,
    Key = 5,
    Response = 6
} request_type;

typedef struct message {
    long mtype;
    int client_id;
    char data[MAX_DATA_LEN];
} message;

#endif //ZESTAW6_COMMUNICATION_PROTOCOL_H

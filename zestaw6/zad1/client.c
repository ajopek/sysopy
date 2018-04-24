//
// Created by artur on 22.04.18.
//
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "communication_protocol.h"

// Queue handling
void remove_queue(void);
// Messages handling
void send(request_type type, void* data,size_t data_size);
message* receive(void);
// Requests handling
void key_request(void);
void mirror_request(char* data);
void calculate_request(char* data);
void time_request(void);
void end_request(void);
// Args handling


// GLOBAL VARS -----------------------------------------
int client_id = -1;
int server_queue_id;
int client_queue_id;

// MAIN ------------------------------------------------
int main(int argc, char* argv[]) {
    FILE *input;
    if(argc <= 1 || strcmp("-", argv[1]) == 0) {
        fprintf(stderr, "Reading input from stdin\n");
        input = stdin;
    } else {
        input = fopen(argv[1], "r");
        if(input == NULL) {
            fprintf(stderr, "Opening input file '%s' failed: %s\n", argv[1], strerror(errno));
        }
    }

    atexit(remove_queue);
    client_queue_id = msgget(ftok(getenv("HOME"), PROJECT_ID), IPC_CREAT | 0644u);
    server_queue_id = msgget(ftok(getenv("HOME"), PROJECT_ID), 0644u);
    printf("Created queue id: %i \n", client_queue_id);
    key_request();

    char *line = NULL;
    size_t length = 0;
    while(getline(&line, &length, input) > 0) {
        printf("Executing '%.*s':\n", (int) (strlen(line) - 1), line);

        char* arg = strtok(line, " \n");
        if(strcmp(arg, "Mirror") == 0) {
            if(!(arg = strtok(NULL, " "))) {
                fprintf(stderr, "You must provider string to be mirrored\n");
            } else {
                mirror_request(arg);
            }
        } else if(strcmp(arg, "Calc") == 0) {
            if(!(arg = strtok(NULL, " "))) {
                fprintf(stderr, "You must provide <operand><operator><operand>\n");
            } else {
                calculate_request(arg);
            }
        } else if(strcmp(arg, "Time") == 0) {
            time_request();
        } else if(strcmp(arg, "End") == 0) {
            end_request();
            exit(0);
        } else {
            fprintf(stderr, "Unknown command '%s'\n", arg);
        }
    }
    free(line);

    mirror_request("123456789124124124");
    calculate_request("12*5");
    time_request();
    end_request();

    exit(EXIT_SUCCESS);
}

// QUEUE HANDLING --------------------------------------

void remove_queue(void) {
    printf("Removing client queue. \n");
    msgctl(client_queue_id, IPC_RMID, NULL);
}

// MESSAGES HANDLING -----------------------------------

void send(request_type type, void* data,size_t data_size) {
    message req = {
            .mtype = type,
            .client_id = client_id,
            .data = {0}
    };
    memcpy(req.data, data, data_size);
    if(msgsnd(server_queue_id, &req, MSG_SIZE, 0) < 0) {
        printf("Sending message (type %ld) to server failed: %d %s\n",
                req.mtype,
                errno,
                strerror(errno));
    }


}

message* receive(void) {
    message* received = malloc(MSG_SIZE);
    memset(received, 0, MSG_SIZE);
    msgrcv(client_queue_id, received, MSG_SIZE, 0, 0);
    return received;
}

// REQUEST HANDLERS ------------------------------------

void key_request(void) {
    send(Key, &client_queue_id, sizeof(int));

    message* response = receive();
    client_id = *(int *) response->data;
    printf("Received client id: %i \n", client_id);
    free(response);
}

void mirror_request(char* data) {
    int size = (int) strlen(data);
    send(Mirror, data, sizeof(char) * size);

    message* response = receive();
    printf("Received mirror: %s \n", response->data);
    free(response);
}

void calculate_request(char* data) {
    int size = (int) strlen(data);
    send(Calc, data, sizeof(char) * size);

    message* response = receive();
    printf("Received calc: %i \n", *(int*)response->data);
    free(response);
}

void time_request(void) {
    send(Time, "", sizeof(char));

    message* response = receive();
    printf("Received time: %s \n", response->data);
    free(response);
}

void end_request(void) {
    send(End, "", sizeof(char));
    exit(EXIT_SUCCESS);
}

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <mqueue.h>
#include "communication_protocol.h"

#define MAX_CLIENTS_NUM 50

// Requests handlers
void handle_mirror(message *msg);

void handle_time(message *msg);

void handle_key(message *msg);

void handle_calculate(message *msg);

void handle_bad_request(message *msg);

void handle_end(message *msg);

// Queues handling
void remove_queue(void);

void remove_client(int client_id);

// Message handling
void send(int client_id, void* data, size_t data_size);

mqd_t clients[MAX_CLIENTS_NUM];
int last_client_id = 0;
mqd_t requests_queue_id;

int main() {
    atexit(remove_queue);
    struct mq_attr attr;  
	attr.mq_flags = 0;  
	attr.mq_maxmsg = 10;  
	attr.mq_msgsize = sizeof(message);  
	attr.mq_curmsgs = 0; 
    // Create requests queue
    if ((requests_queue_id = mq_open(SERVER_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR, NULL)) < 0) {
        printf("Server queue open error %s", strerror(errno));
	exit(1);
    }	

    message *received_message = malloc(MSG_SIZE);

    int end_flag = 0;
    ssize_t bytes_read;
    while (1) {
        if (mq_receive(requests_queue_id, (char*)received_message, 8192, NULL) < 0) {
            printf("Error: %s \n", strerror(errno));
	    break;
        }
        switch (received_message->mtype) {
            case Mirror:
                //send back reversed string
                handle_mirror(received_message);
                break;
            case Calc:
                //send back answer
                handle_calculate(received_message);
                break;
            case Time:
                //send back system time
                handle_time(received_message);
                break;
            case End:
                // after empty queue end
                end_flag = 1;
                handle_end(received_message);
                break;
            case Key:
                handle_key(received_message);
                break;
            case Stop:
                remove_client(received_message->client_id);
                break;
            default:
                handle_bad_request(received_message);
                // No such type, send error
                break;
        }
	if(end_flag) break;
    }
    free(received_message);
    exit(EXIT_SUCCESS);
}

    void handle_mirror(message *msg) {
        int size = (int) strlen(msg->data);
        printf("Handling mirror: %s \n", msg->data);
        char* reversed = malloc(size * sizeof(char));
	int i;
        for(i = 0; i < size; ++i) {
            reversed[i] = msg->data[size - i - 1];
        }
        send(msg->client_id, reversed, size * sizeof(char));
    }

    void handle_calculate(message *msg) {
        if (strlen(msg->data) < 3) return handle_bad_request(msg);
        char *arg_end;
        printf("Handling calc %s \n", msg->data);
        int i = 0;
        while (msg->data[i] != '+' && msg->data[i] != '-' && msg->data[i] != '/' && msg->data[i] != '*') i++;
        arg_end = msg->data + i - 1;
        long first_arg = strtol(msg->data, &arg_end, 10);
        arg_end += 1;
        long second_arg = strtol(arg_end, NULL, 10);
        long *result = malloc(sizeof(long));
        switch (msg->data[i]) {
            case '+':
                *result = first_arg + second_arg;
                break;
            case '-':
                *result = first_arg - second_arg;
                break;
            case '*':
                *result = first_arg * second_arg;
                break;
            case '/':
                *result = first_arg / second_arg;
                break;
            default:
                handle_bad_request(msg);
                return;
        }
        send(msg->client_id, result, sizeof(long));
    }

    void handle_time(message *msg) {
        char *result = malloc(sizeof(char) * MAX_DATA_LEN);
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        strftime(result, MAX_DATA_LEN, "%c", &tm);
        send(msg->client_id, result, strlen(result) * sizeof(char));
    }

    void handle_key(message *msg) {
        // Open queue and add to clients table
        char name[MAX_DATA_LEN];
        sprintf(name, "%s", msg->data);
        if ((clients[last_client_id] = mq_open(name, O_WRONLY)) < 0)
            printf("Client queue open error: %s", strerror(errno));
        printf("Registered client id: %i, queue: %i \n", last_client_id, clients[last_client_id]);
        // Send client id
        int *result = malloc(sizeof(int));
        *result = last_client_id;
        last_client_id++;
        send(*result, result, sizeof(int));
    }

    void handle_end(message *msg) {
        printf("Server ending. \n");
    }

    void handle_bad_request(message *msg) {
        printf("Bad request. \n");
    }

    void remove_queue(void) {
        printf("Removing server queue. \n");
        mq_unlink(SERVER_NAME);
    }

    void remove_client(int client_id) {
        if (mq_close(clients[client_id]) < 0)
            printf("Remove client error: %s", strerror(errno));
        clients[client_id] = -1;
    }

void send(int client_id, void* data, size_t data_size) {
    message res = {
            .mtype = Response,
            .client_id = client_id,
            .data = {0}
    };
    memcpy(res.data, data, data_size );
    printf("Sending: %i \n", *(int*)data);
    if(mq_send(clients[client_id], (char*) &res, 8192, 1) < 0) {
        printf("Sending response to client id: %i failed: %d %s\n",
               client_id,
               errno,
               strerror(errno));
    }


}

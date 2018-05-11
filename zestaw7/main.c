#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <time.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/signal.h>

#include "barber_problem.h"

int shm_id = 0;
int seats_num;
int sem_id = 0;

void handle_sigterm(int signum) {
    exit(EXIT_SUCCESS);
}

void setup_signals() {
    signal(SIGTERM, handle_sigterm);
    signal(SIGINT, handle_sigterm);
}

int init_barbershop() {
    key_t key = ftok(PROJECT_PATHNAME, PROJECT_ID);
    if(key == -1) {
        printf("Key ftok failure. \n");
        return -1;
    }

    // Get semaphores
    if((sem_id = semget(key, 2, IPC_CREAT | S_IRWXU)) == -1) {
        printf("semget failed. \n");
        return -1;
    }

    if(semctl(sem_id, 0, SETVAL, 1) == -1) {
        printf("semctl failed \n");
        return -1;
    }

    if(semctl(sem_id, 1, SETVAL, 1) == -1) {
        printf("semctl failed \n");
        return -1;
    }

    shm_id = shmget(key, sizeof(struct barber_shop_t) + sizeof(int) * seats_num, IPC_CREAT | 0660);
    if(shm_id == -1) {
        printf("shmget failure. \n");
        return -1;
    }

    if((barber_shop = (struct barber_shop_t*)shmat(shm_id, NULL, NULL)) == (void*) -1) {
        printf("shmat failed. \n");
        return -1;
    }

    barber_shop->barber_status = IDLE;
    barber_shop->last_client = -1;
    barber_shop->first_client = -1;
    barber_shop->queue_size = seats_num;
    barber_shop->current_client = -1;
    barber_shop->finished_cut = -1;

    return 0;
}

int is_queue_empty() {
    return (barber_shop->first_client == -1 && barber_shop->last_client == -1) ? 1 : 0;
}

long get_timestamp() {
    struct timespec buf;
    clock_gettime(CLOCK_MONOTONIC, &buf);
    return buf.tv_nsec / 1000;
}

void sem_up(int sem_num) {
    struct sembuf semops;
    semops.sem_num = sem_num;
    semops.sem_op = 1;
    semops.sem_flg = 0;

    if(semop(sem_id, &semops, 1) == -1) {
        printf("semop failure: \t %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void sem_down(int sem_num) {
    struct sembuf semops;
    semops.sem_num = sem_num;
    semops.sem_op = -1;
    semops.sem_flg = 0;

    if(semop(sem_id, &semops, 1) == -1) {
        printf("semop failure: \t %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void serve_client_start(int client_id) {
    barber_shop->current_client = client_id;
    printf("Started serving client id: %i, timestamp: %li \n", client_id, get_timestamp());
    printf("Ended serving client id: %i, timestamp: %li \n", client_id, get_timestamp());
    barber_shop->finished_cut = 1;
};

void invite_client() {
    printf("Invited client id: %i, timestamp: %li \n", barber_shop->first_client, get_timestamp());
    serve_client_start(get_client());

};

void go_to_sleep() {
    printf("Barber went to sleep, timestamp: %li \n", get_timestamp());
    barber_shop->barber_status = SLEEPING;
};

void woke_up() {
    printf("Barber woke up, timestamp: %li \n", get_timestamp());
    barber_shop->barber_status=IDLE;
};

void clean_up() {
    if(sem_id != 0) {
        semctl(sem_id, 0, IPC_RMID);
    }

    if(shm_id != 0) {
        shmctl(shm_id, IPC_RMID, NULL);
    }
}

int get_client() {
    int client_id = barber_shop->client_queue[barber_shop->first_client];
    if(barber_shop->first_client == barber_shop->last_client) {
        barber_shop->first_client = -1;
        barber_shop->last_client = -1;
    } else {
        barber_shop->first_client = (barber_shop->first_client+1)%seats_num;
    }

    return client_id;
}

int main() {
    setup_signals();
    atexit(&clean_up);
    if(init_barbershop()) {
        printf("Error: %s", strerror(errno));
        exit(EXIT_FAILURE);
    };
    sem_up(0);
    while(1) {
        sem_down(0);
        switch(barber_shop->barber_status){
            case IDLE:
                if(is_queue_empty()) {
                    go_to_sleep();
                } else {
                    invite_client();
                }
                break;
            case WOKEUP:
                woke_up();
                break;
            default:
                break;
        }
        sem_up(0);
    }

    return 0;
}

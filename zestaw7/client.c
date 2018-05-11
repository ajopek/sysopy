//
// Created by artur on 11.05.18.
//

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
#include <zconf.h>

#include "barber_problem.h"

int sem_id = 0;
int shm_id = 0;
int seats_num;

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


int init_client() {
    key_t key = ftok(PROJECT_PATHNAME, PROJECT_ID);
    // Get semaphores
    if((sem_id = semget(key, 0, S_IRWXU)) == -1) {
        printf("semget failed. \n");
        return -1;
    }
    sem_down(0);
    if(key == -1) {
        printf("Key ftok failure. \n");
        return -1;
    }

    if((shm_id = (shmget(key, NULL, 0)) < 0)) {
        printf("shmget failed. \n");
        return -1;
    }

    if((barber_shop = (struct barber_shop_t*)shmat(shm_id, NULL, 0)) == (void*) -1) {
        printf("shmat failed. \n");
        return -1;
    }

    sem_up(0);
    seats_num = barber_shop->queue_size;
    return 0;
}


long get_timestamp() {
    struct timespec buf;
    clock_gettime(CLOCK_MONOTONIC, &buf);
    return buf.tv_nsec / 1000;
}

void clean_up() {
    if(sem_id != 0) {
        semctl(sem_id, 0, IPC_RMID);
    }

    if(shm_id != 0) {
        shmctl(shm_id, IPC_RMID, NULL);
    }
}

int is_queue_empty() {
    return (barber_shop->first_client == -1 && barber_shop->last_client == -1) ? 1 : 0;
}

void take_seat() {
    if(is_queue_empty()) {
        barber_shop->first_client = 0;
        barber_shop->last_client = 0;
    } else {
        barber_shop->last_client++;
    }
    printf("Client took a seat, pid: %i, timestamp: %li \n", getpid(), get_timestamp());
    barber_shop->client_queue[barber_shop->last_client] = getpid();
}

int is_queue_full() {
    return ((barber_shop->last_client + 1) % seats_num == barber_shop->first_client) ? 1 : 0;
}

void leave_shop() {
    printf("Client left shop, pid: %i, timestamp: %li \n", getpid(), get_timestamp());
}

void client(int cut_num) {
    printf("Here2. \n");
    if(init_client() == -1) {
        printf("Error: %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Here3. \n");
    int i;
    for(i = 0; i < cut_num; ) {
        sem_down(0);
        switch (barber_shop->barber_status) {
            case SLEEPING:
                barber_shop->barber_status = WOKEUP;
                printf("Client woke barber, pid: %i, timestamp: %li \n", getpid(), get_timestamp());
                break;
            case WORKING:
                if(barber_shop->current_client == getpid() && barber_shop->finished_cut == 1){
                    leave_shop();
                    barber_shop->finished_cut = 0;
                    barber_shop->barber_status = IDLE;
                    cut_num--;
                } else if(!is_queue_full()) {
                    take_seat();
                } else {
                    leave_shop();
                }
                break;
            default:
                printf("%i \n", barber_shop->barber_status);
                break;
        }
        sem_up(0);
    }
}

void main(int argc, char* argv[]) {
    printf("Here. \n");
    client(4);
}

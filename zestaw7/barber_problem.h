//
// Created by artur on 10.05.18.
//

#ifndef ZESTAW7_BARBER_PROBLEM_H
#define ZESTAW7_BARBER_PROBLEM_H

#define PROJECT_PATHNAME getenv("HOME")
#define PROJECT_ID 'c'

#define IDLE 0
#define SLEEPING 1
#define WORKING 2
#define WOKEUP 3

struct barber_shop_t {
    int barber_status;
    int queue_size;
    int first_client;
    int last_client;
    int current_client;
    int finished_cut;
    int *client_queue;
} *barber_shop;

#endif //ZESTAW7_BARBER_PROBLEM_H

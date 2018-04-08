//
// Created by artur on 08.04.18.
//
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <zconf.h>
#include <time.h>
#include <string.h>

void sigusr1_action(int signum, siginfo_t *siginfo, void *context);

int main(int argc, char* argv[]) {
    // Handle permission signal
    // SIGUSR1
    struct sigaction sigusr1_act;
    memset(&sigusr1_act, '\0', sizeof(struct sigaction));
    sigusr1_act.sa_flags = SA_SIGINFO;
    sigusr1_act.sa_sigaction = &sigusr1_action;
    sigusr1_act.

    // Wait and send permission request
    srand(time(NULL));
    int waiting_time = rand() % 10;
    sleep(waiting_time);
    printf("%i \n", waiting_time);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);

    sigsuspend(&mask);
    return 0;
}

void sigusr1_action(int signum, siginfo_t *siginfo, void *context) {
    printf("Got signal");
    //kill(getppid(), SIGRTMIN + rand() % (SIGRTMAX - SIGRTMIN));
}
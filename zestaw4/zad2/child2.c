//
// Created by artur on 08.04.18.
//
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <zconf.h>
#include <time.h>
#include <string.h>

void sigusr2_action(int signum, siginfo_t *siginfo, void *context);

int main(int argc, char* argv[]) {
    // Handle permission signal
    // SIGUSR1
    int rndm;
    srand(time(NULL) + getpid());
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    act.sa_sigaction = sigusr2_action;
    sigaction(SIGUSR2, &act, NULL);
    sleep(rndm = rand()%10 + 1);
    //kill(getppid(), SIGUSR1);
    pause();
    union sigval a;
    a.sival_int = rndm;
    printf("ASD");
    //sigqueue(getppid(), rand() % (SIGRTMAX - SIGRTMIN) + SIGRTMIN, a);
    exit(rndm);
}

void sigusr2_action(int signum, siginfo_t *siginfo, void *context) {
    //printf("Got signal");
    //kill(getppid(), SIGRTMIN + rand() % (SIGRTMAX - SIGRTMIN));
}
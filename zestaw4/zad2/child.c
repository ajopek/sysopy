//
// Created by artur on 02.04.18.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <zconf.h>

// Signals
void sigusr1_action(int signum, siginfo_t *siginfo, void *context);


sig_atomic_t permission_flag = 0;
int main(int argc, char* argv[]) {
    // Handle signals
    // SIGUSR1
    /*struct sigaction sigusr1_act;
    memset(&sigusr1_act, '\0', sizeof(struct sigaction));
    sigusr1_act.sa_sigaction = &sigusr1_action;
    sigusr1_act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sigusr1_act, NULL);
    */
    //srand(time(NULL));
    //int waiting_time = rand()%10;
    //sleep(waiting_time);
    //kill(getppid(), SIGUSR1);
    printf("%i \n", getpid());

    // Wati for permission - sigusr1
    //sigset_t mask;
    //sigfillset(&mask);
    //sigdelset(&mask, SIGUSR1);

    //sigsuspend(&mask);
    while(1);

    printf("Send rt, pid: %i \n", getpid());
    //return waiting_time;
}

void sigusr1_action(int signum, siginfo_t *siginfo, void *context) {
    kill(getppid(), SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN + 1)));
};
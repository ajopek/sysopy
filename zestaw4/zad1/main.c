#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <zconf.h>
#include <time.h>
#include <stdlib.h>



// Signal handlers
void sigtstp_handler(int signum, siginfo_t *siginfo, void *context);
void sigint_handler(int _signum);

sig_atomic_t is_paused = 0; // Program pause flag

int main(int argc, char *argv[]) {
    // Define SIGTSTP disposition
    struct sigaction sigtstp_act;
    memset(&sigtstp_act, '\0', sizeof(struct sigaction));
    sigtstp_act.sa_sigaction = &sigtstp_handler;
    sigtstp_act.sa_flags = SA_SIGINFO;
    sigaction(SIGTSTP, &sigtstp_act, NULL);

    // Define SIGINT disposition
    signal(SIGINT, &sigint_handler);

    // main program
    while (1) {
        time_t rawtime;
        struct tm * timeinfo;

        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        printf ( "%s", asctime (timeinfo) );

        if(is_paused) pause();
    }
}


void sigtstp_handler(int signum, siginfo_t *siginfo, void *context) {
    if (is_paused) {
        is_paused = 0;
        return;
    }
    else {
        is_paused = 1;
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu \n");
    }
}

void sigint_handler(int _signum) {
    printf("Odebrano sygnał SIGINT \n");
    exit(0);
}

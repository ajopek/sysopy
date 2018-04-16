#include <stdio.h>
#include <signal.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>

void sigusr1_child_action(int signum, siginfo_t *siginfo, void *context);
void sigusr1_parent_action(int signum, siginfo_t *siginfo, void *context);
void sigusr2_child_action(int signum, siginfo_t *siginfo, void *context);
void sigrtmin_child_action(int signum, siginfo_t *siginfo, void *context);
void sigrtmin_parent_action(int signum, siginfo_t *siginfo, void *context);
void sigrtmax_child_action(int signum, siginfo_t *siginfo, void *context);

sig_atomic_t child_signals = 0;
sig_atomic_t parent_signals = 0;

int main(int argc, char* argv[]) {
    int i;
    int L, type;

    if(argc < 3) {
        exit(1);
    }

    L = atoi(argv[1]);
    type = atoi(argv[2]);

    // Handle signals
    sigset_t used_signals;
    sigemptyset(&used_signals);
    sigaddset(&used_signals, SIGUSR1);
    sigaddset(&used_signals, SIGUSR2);
    sigaddset(&used_signals, SIGRTMIN);
    sigaddset(&used_signals, SIGRTMAX);

    pid_t pid = fork();

    struct sigaction sig_act;
    memset(&sig_act, '\0', sizeof(struct sigaction));
    sig_act.sa_mask = used_signals;
    sig_act.sa_flags = SA_SIGINFO;

    if(pid == 0) {
        switch (type) {
            case 0:
                sig_act.sa_sigaction = &sigusr1_child_action;
                sigaction(SIGUSR1, &sig_act, NULL);
                sig_act.sa_sigaction = &sigusr2_child_action;
                sigaction(SIGUSR2, &sig_act, NULL);
                break;
            case 1:
                sig_act.sa_sigaction = &sigusr1_child_action;
                sigaction(SIGUSR1, &sig_act, NULL);
                sig_act.sa_sigaction = &sigusr2_child_action;
                sigaction(SIGUSR2, &sig_act, NULL);
                break;
            case 2:
                sig_act.sa_sigaction = &sigrtmin_child_action;
                sigaction(SIGRTMIN, &sig_act, NULL);
                sig_act.sa_sigaction = &sigrtmax_child_action;
                sigaction(SIGRTMAX, &sig_act, NULL);
                break;
            default:
                exit(1);
        }

        while(1);

    } else if(pid > 0) {
        sleep(1);
         switch (type) {
             case 0:
                 sig_act.sa_sigaction = &sigusr1_parent_action;
                 sigaction(SIGUSR1, &sig_act, NULL);
                 for(i = 0; i < L; i++) {
                     kill(pid, SIGUSR1);
                     printf("Send %i signals to child. \n", i);
                 }
                 kill(pid, SIGUSR2);
                 break;
             case 1:
                 sig_act.sa_sigaction = &sigusr1_parent_action;
                 sigaction(SIGUSR1, &sig_act, NULL);
                 for(i = 0; i < L; i++) {
                     kill(pid, SIGUSR1);
                     pause();
                     printf("Send %i signals to child. \n", i+1);
                 }
                 kill(pid, SIGUSR2);
                 break;
             case 2:
                 sig_act.sa_sigaction = &sigrtmin_parent_action;
                 sigaction(SIGRTMIN, &sig_act, NULL);
                 for(i = 0; i < L; i++) {
                     kill(pid, SIGRTMIN);
                     printf("Send %i signals to child. \n", i+1);
                 }
                 kill(pid, SIGRTMAX);
                 break;
             default:
                 exit(1);
         }
    } else {
        exit(1);
    }
    return 0;
}

void sigusr1_child_action(int signum, siginfo_t *siginfo, void *context) {
    kill(getppid(), SIGUSR1);
    printf("Child got %i signals. \n", ++child_signals);
}

void sigusr1_parent_action(int signum, siginfo_t *siginfo, void *context) {
    printf("Parent got %i signals. \n", ++parent_signals);
}

void sigusr2_child_action(int signum, siginfo_t *siginfo, void *context) {
    _Exit(1);
}

void sigrtmin_child_action(int signum, siginfo_t *siginfo, void *context) {
    kill(getppid(), SIGRTMIN);
    printf("Child got %i signals. \n", ++child_signals);
}

void sigrtmin_parent_action(int signum, siginfo_t *siginfo, void *context) {
    printf("Parent got %i signals. \n", ++parent_signals);
}

void sigrtmax_child_action(int signum, siginfo_t *siginfo, void *context) {
    _Exit(1);
}
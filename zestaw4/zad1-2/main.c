#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <zconf.h>
#include <time.h>
#include <stdlib.h>

#define SCRIPT_PATH "date_script.sh"

// child process handlers
void start_child();
void end_child();
// Signal handlers
void sigtstp_handler(int signum, siginfo_t *siginfo, void *context);
void sigint_handler(int _signum);

sig_atomic_t is_alive; // Program pause flag
pid_t child_pid;

int main(int argc, char *argv[]) {
    // Define SIGTSTP disposition
    struct sigaction sigtstp_act;
    memset(&sigtstp_act, '\0', sizeof(struct sigaction));
    sigtstp_act.sa_sigaction = &sigtstp_handler;
    sigtstp_act.sa_flags = SA_SIGINFO;
    sigaction(SIGTSTP, &sigtstp_act, NULL);

    // Define SIGINT disposition
    signal(SIGINT, &sigint_handler);

    // Initialize program
    start_child();
}

/**
 * Child process handlers
 */

void start_child() {
    pid_t pid = fork();
    if (pid < 0) {
        printf("Fork failed");
        exit(1);
    } else if (pid) {
        // parent process
        child_pid = pid;
    } else {
        // child process
        execvp(SCRIPT_PATH, NULL);
        // exec returned, smth failed
        printf("Exec failed.");
        exit(1);
    }
};
void end_child();

/**
 * Signal handlers
 */

void sigtstp_handler(int signum, siginfo_t *siginfo, void *context) {
    if (is_alive) {
        is_alive = 0;
        end_child();
        return;
    }
    else {
        is_alive = 1;
        start_child();
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu \n");
    }
}

void sigint_handler(int _signum) {
    printf("Odebrano sygnał SIGINT \n");
    if (is_alive) {
        end_child();
    }
    exit(0);
}
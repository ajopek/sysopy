#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <zconf.h>
#include <time.h>
#include <stdlib.h>
#include <wait.h>

#define CHILD_SCRIPT "./child"

// Signal handlers
void sigusr1_action(int signum, siginfo_t *siginfo, void *context);
void sigchld_action(int signum, siginfo_t *siginfo, void *context);
void sigint_action(int signum, siginfo_t *siginfo, void *context);
// Children handlers
pid_t spawn_child();
void terminate_child(pid_t);
// At exit functions
void terminate_children(void);
void free_pids_arrays(void);
// Helpers
void print_help();

// Children
pid_t * children_pid; // array of N children PID
pid_t * request_pid; // array of max. N children, from which a request to start have been received
sig_atomic_t requests_num = 0;
sig_atomic_t alive_children = 0;
// Parameters
int K; // Number of signals after which children are allowed to start work
int N; // Number of children to spawn

int main(int argc, char* argv[]) {
    int i;
    if (argc < 3) {
        //print help
        exit(1);
    }
    // Parameters
    N = atoi(argv[1]);
    K = atoi(argv[2]);
    // Children pids
    children_pid = malloc(sizeof(pid_t) * N);
    request_pid = malloc(sizeof(pid_t) * K);

    // At exit (reversed order of execution compared to registration)
    // Free pids arrays
    atexit(free_pids_arrays);
    // Terminate all spawned children
    atexit(terminate_children);

    // Handle signals
    // SIGUSR1
    struct sigaction sigusr1_act;
    memset(&sigusr1_act, '\0', sizeof(struct sigaction));
    sigusr1_act.sa_sigaction = &sigusr1_action;
    sigusr1_act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sigusr1_act, NULL);

    // SIGCHLD
    struct sigaction sigchld_act;
    memset(&sigchld_act, '\0', sizeof(struct sigaction));
    sigchld_act.sa_sigaction = &sigchld_action;
    sigchld_act.sa_flags = SA_SIGINFO;
    sigaction(SIGCHLD, &sigchld_act, NULL);

    // Spawn children
    for (i = 0; i < N; i++) {
        children_pid[i] = spawn_child();
        alive_children++;
    }

    // Process signals
    while (requests_num < K);
    // Send confirmation to all processes that already requested
    for (i = 0; i < K; i++){
        kill(SIGUSR1, request_pid[i]);
    }

    while(alive_children);

    return 0;
}

/**
 * Signal handlers
 */

void sigusr1_action(int signum, siginfo_t *siginfo, void *context) {
    requests_num++;
    pid_t child_pid = siginfo->si_pid;
    if (requests_num < K) {
        request_pid[requests_num-1] = child_pid;
    } else {
        kill(SIGUSR1, child_pid);
    }
    printf("Got signal SIGINT from process pid: %i", (int)child_pid);
}

void sigchld_action(int signum, siginfo_t *siginfo, void *context) {
    pid_t child_pid = siginfo->si_pid;
    alive_children--;
    int exit_status;
    wait(&exit_status);
    printf("Child pid: %i, exited with status: %i", child_pid, exit_status);
}

void sigint_action(int signum, siginfo_t *siginfo, void *context) {
    terminate_children();
    free_pids_arrays();
    _exit(0);
}


/**
 * Children handlers
 */

pid_t spawn_child() {
    pid_t pid = fork();
    if (pid < 0) {
        printf("Fork failed");
        exit(1);
    } else if (pid) {
        // parent process
        return pid;
    } else {
        // child process
        char *args[2];
        args[0] = CHILD_SCRIPT;
        args[1] = NULL;
        execvp(CHILD_SCRIPT, args);
        // exec returned, something failed
        printf("Exec failed.");
        exit(1);
    }
};
void terminate_child(pid_t child_pid) {
    kill(child_pid, SIGKILL);
};


/**
 * At exit functions
 */

void terminate_children(void) {
    int i = 0;
    for (; i < N; i++) {
        terminate_child(children_pid[i]);
    }
}

void free_pids_arrays(void) {
    free(children_pid);
    free(request_pid);
}

/**
 * Helpers
 */

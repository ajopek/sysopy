#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <zconf.h>
#include <stdlib.h>
#include <wait.h>
#include <time.h>

#define CHILD_SCRIPT "./child2"

// Signal handlers
void sigusr1_action(int signum, siginfo_t *siginfo, void *context);
void sigusr2_action(int signum, siginfo_t *siginfo, void *context);
void sigchld_action(int signum, siginfo_t *siginfo, void *context);
void sigint_action(int signum, siginfo_t *siginfo, void *context);
void sigrt_action(int signum, siginfo_t *siginfo, void *context);
// Children handlers
pid_t spawn_child();
void terminate_child(pid_t);
// At exit functions
void terminate_children(void);
void free_pids_arrays(void);
// Helpers
//void print_help();

// Children
pid_t * children_pid; // array of N children PID
pid_t * request_pid; // array of max. N children, from which a request to start have been received
sig_atomic_t requests_num = 0;
sig_atomic_t alive_children = 0;
sig_atomic_t exit_flag = 0;
// Parameters
long K; // Number of signals after which children are allowed to start work
long N; // Number of children to spawn

int main(int argc, char* argv[]) {
    int i;
    if (argc < 3) {
        //print help
        exit(1);
    }
    // Parameters
    N = strtol(argv[1], NULL, 10);
    K = strtol(argv[2], NULL, 10);
    printf("%li %li \n", N, K);
    // Children pids

    children_pid = malloc(sizeof(pid_t) * N);
    request_pid = malloc(sizeof(pid_t) * K);

    // Handle signals

    struct sigaction sig_act;
    sigemptyset(&sig_act.sa_mask);
    memset(&sig_act, '\0', sizeof(struct sigaction));
    sig_act.sa_flags = SA_SIGINFO | SA_RESTART;

    // SIGUSR1
    sig_act.sa_sigaction = sigusr1_action;
    sigaction(SIGUSR1, &sig_act, NULL);

    // SIGINT
    sig_act.sa_sigaction = sigint_action;
    sigaction(SIGINT, &sig_act, NULL);

    // SIGRT
    sig_act.sa_sigaction = sigrt_action;
    for (i = SIGRTMIN; i <= SIGRTMAX; ++i) {
        sigaction(i, &sig_act, NULL);
    }

    // Spawn children
    for (i = 0; i < N; i++) {
        children_pid[i] = spawn_child();
        alive_children++;
    }

    // Process signals
    while (requests_num < K);


    while(alive_children) {
        if(exit_flag) {
            terminate_children();
            free_pids_arrays();
            exit(0);
        }
    };

    free_pids_arrays();
    exit(0);
}

/**
 * Signal handlers
 */

void sigusr2_action(int signum, siginfo_t *siginfo, void *context){
    //ignored
    printf("ASD");
};

void sigusr1_action(int signum, siginfo_t *siginfo, void *context) {
    pid_t child_pid = siginfo->si_pid;
    int i;
    if (requests_num < K) {
        request_pid[requests_num] = child_pid;
    } else if(requests_num == K) {
        // Send confirmation to all processes that already requested
        for (i = 0; i < K; i++){
            kill(SIGUSR2, request_pid[i]);
            printf("Send to %i \n", request_pid[i]);
        }
    } else {
        kill(SIGUSR2, child_pid);
        printf("Send to %i \n", child_pid);
    }
    ++requests_num;
    printf("Got signal SIGUSR1 from process pid: %i \n", (int)child_pid);
}

void sigint_action(int signum, siginfo_t *siginfo, void *context) {
    exit_flag = 1;
}

void sigrt_action(int signum, siginfo_t *siginfo, void *context) {
    pid_t child_pid = siginfo->si_pid;
    printf("Got signal SIGMIN+%i, from child pid: %i \n", signum - SIGRTMIN, child_pid);
    alive_children--;
    int exit_status;
    waitpid(child_pid, &exit_status, 1);
    exit_status = WIFEXITED(&exit_status);
    printf("Child pid: %i, exited with status: %i \n", child_pid, siginfo->si_value.sival_int);
}


/**
 * Children handlers
 */

pid_t spawn_child() {
    pid_t pid = fork();
    if (pid < 0) {
        printf("Fork failed \n");
        exit(1);
    } else if (pid) {
        return pid;
    } else {
        // child process
        int rndm;
        srand(time(NULL) + getpid());
        struct sigaction act;
        memset(&act, 0, sizeof(act));
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_SIGINFO | SA_RESTART;
        act.sa_sigaction = sigusr2_action;
        sigaction(SIGUSR2, &act, NULL);
        sleep(rndm = rand()%10 + 1);
        kill(getppid(), SIGUSR1);
        pause();
        union sigval a;
        a.sival_int = rndm;
        sigqueue(getppid(), rand() % (SIGRTMAX - SIGRTMIN) + SIGRTMIN, a);
        exit(rndm);
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

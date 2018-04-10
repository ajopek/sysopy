#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>


pid_t *PIDS;
pid_t *CHILDREN;
sig_atomic_t CNT = 0;
int KBUFFER_SIZE=0;
sig_atomic_t exit_flag = 0;
sig_atomic_t ALIVE = 0;

void print_program_error(int error_code);
int parse(int argc, char *argv[], int *N, int *K);
void set_handlers();
int create_processes(int N, int K);
void handler_sigusr(int sig_num, siginfo_t *sig_info, void *sig_context);
void handler_sigrt(int sig_num, siginfo_t *sig_info, void *sig_context);
void handler_sigint();
void handler_sigusr2();
void handler_sigint();
void kill_remaining();

int main(int argc, char *argv[])
{
    int N, K, result;
    if((result = parse(argc, argv, &N, &K)) != 0){
        print_program_error(result);
        return result;
    }

    create_processes(N, K);
    free(PIDS);
    free(CHILDREN);

    return 0;
}

int parse(int argc, char *argv[], int *N, int *K){
    if(argc < 4 ) return 1;
    if((*N = atoi(argv[1])) == 0) return 2;
    if ((*K = atoi(argv[2])) == 0)  return 3;
    
    PIDS = malloc(*K * sizeof(pid_t));
    CHILDREN = malloc((*N + 1) * sizeof(pid_t) );
    CHILDREN[*N] = (pid_t)0;
    int i;
    for(i = 0; i < *N; ++i) {
        CHILDREN[i] = -1;
    }
    KBUFFER_SIZE = *K;
    return 0;

}

void print_program_error(int error_code)
{
    switch (error_code)
    {
        case 0:
            printf("succesfully done all operations\n");
            break;
        case 1:
            printf("./zad2 NumberOfProcesses(N) NumberOfWaitedProcesses(K) \n");
            break;
        case 2:
            printf("provided N is not valid \n");
            break;
        case 3:
            printf("provided K is not valid \n");
            break;
    }
}

int create_processes(int N, int K){
    set_handlers();
    for(int i=0; i < N ; i++){
        int curr_pid;
        curr_pid = fork();
        srand(time(NULL) + curr_pid);
        if (curr_pid == 0){
            int rndm;
            struct sigaction act;
            memset(&act, 0, sizeof(act));
            sigemptyset(&act.sa_mask);
            act.sa_flags = SA_SIGINFO | SA_RESTART;
            act.sa_sigaction = handler_sigusr2;
            sigaction(SIGUSR2, &act, NULL);
            sleep(rndm = rand()%10 + 1);
            kill(getppid(), SIGUSR1);
            pause();
            union sigval a;
            a.sival_int = rndm;
            sigqueue(getppid(), rand() % (SIGRTMAX - SIGRTMIN) + SIGRTMIN, a);
            exit(rndm);
        }else{
            sleep(1);
            ALIVE++;
        }
    }

    while(ALIVE > 0){
        if(exit_flag) {
            kill_remaining();
            break;
        }
    }
    return 0;
}

void set_handlers(){
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    sigfillset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    act.sa_sigaction = handler_sigusr;
    sigaction(SIGUSR1, &act, NULL);

    act.sa_sigaction = &handler_sigrt;
    for(int i=SIGRTMIN; i<=SIGRTMAX; i++){
        sigaction(i, &act, NULL);
    }

    act.sa_sigaction = &handler_sigint;
    sigaction(SIGINT, &act, NULL);
}


void handler_sigusr(int sig_num, siginfo_t *sig_info, void *sig_context){
    printf("<%d> received request from <%d>\n", getpid(), sig_info->si_pid);
    if(CNT == KBUFFER_SIZE){
        for(int i =0; i < KBUFFER_SIZE ; i++){
            kill(PIDS[i], SIGUSR2);
        }
        kill(sig_info->si_pid, SIGUSR2);
    }else if(CNT < KBUFFER_SIZE ){
        PIDS[CNT] = sig_info->si_pid;
    }else{
        kill(sig_info->si_pid, SIGUSR2);
    }
    CNT++;
}

void handler_sigusr2(){ // ignored
}

void handler_sigrt(int sig_num, siginfo_t *sig_info, void *sig_context){
    printf("Got signal SIGMIN+%i from %i \n", sig_num - SIGRTMIN, sig_info->si_pid);
    int status, i;
    waitpid(sig_info->si_pid, &status, 1);
    status = WEXITSTATUS(status);
    printf("Child %i, exited with status %i \n", sig_info->si_pid, status);
    ALIVE--;
    for(i = 0; CHILDREN[i] != (pid_t)0; ++i) {
        if(CHILDREN[i] == sig_info->si_pid) {
            CHILDREN[i] = -1;
        }
    }
}

void handler_sigint() {
    exit_flag = 1;
}

void kill_remaining() {
    int i;
    for(i = 0; CHILDREN[i] != (pid_t)0; ++i) {
        if(CHILDREN[i] != -1) {
            kill(SIGKILL, CHILDREN[i]);
        }
    }
}



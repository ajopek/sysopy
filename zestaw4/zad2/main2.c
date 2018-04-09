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
int PROCESS_N;
int CNT = 0;
int KBUFFER_SIZE=0;
int cntrl = 0;

void print_program_error(int error_code);
int parse(int argc, char *argv[], int *N, int *K);
void set_handlers();
int create_processes(int N, int K);
void handler_sigusr(int sig_num, siginfo_t *sig_info, void *sig_context);
void handler_sigrt(int sig_num, siginfo_t *sig_info, void *sig_context);
void handler_sigint();
void handler_sigusr2();

int main(int argc, char *argv[])
{
    int N, K, result;
    if((result = parse(argc, argv, &N, &K)) != 0){
        print_program_error(result);
        return result;
    }

    result = create_processes(N, K);

    return 0;
}

int parse(int argc, char *argv[], int *N, int *K){
    if(argc < 3 ) return 1;
    if((*N = atoi(argv[1])) == 0) return 2;
    if ((*K = atoi(argv[2])) == 0)  return 3;
    PIDS = malloc(*K * sizeof(pid_t));
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
            printf("./program NumberOfProcess(N) NumberOfWaitedProcess(K) \n");
            break;
        case 2:
            printf("provided N is not a number \n");
            break;
        case 3:
            printf("provided K is not a number \n");
            break;
    }
}

int create_processes(int N, int K){
    set_handlers();
    for(int i=0; i < N ; i++){
        srand(time(NULL));
        int curr_pid;
        curr_pid = fork();
        if (curr_pid == 0){
            int wait_time;
            struct sigaction act;
            memset(&act, 0, sizeof(act));
            sigemptyset(&act.sa_mask);
            act.sa_flags = SA_SIGINFO | SA_RESTART;
            act.sa_sigaction = handler_sigusr2;
            sigaction(SIGUSR2, &act, NULL);
            sleep(wait_time = rand()%10 + 1);
            kill(getppid(), SIGUSR1);
            pause();
            union sigval a;
            a.sival_int = wait_time;
            sigqueue(getppid(), rand() % (SIGRTMAX - SIGRTMIN) + SIGRTMIN, a);
            exit(wait_time);
        }else{
            sleep(1);
        }
    }
    while(CNT){}
    printf("%d\n", CNT);
    return 0;
}

void set_handlers(){
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler_sigusr;
    sigaction(SIGUSR1, &act, NULL);

    act.sa_sigaction = &handler_sigrt;
    for(int i=SIGRTMIN; i<=SIGRTMAX; i++){
        sigaction(i, &act, NULL);
    }
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
    int exit_status;
    waitpid(sig_info->si_pid, &exit_status, 1);
    exit_status = WEXITSTATUS(exit_status);
    printf("<%d> ended child <%d> ended with %d\n", getpid(), sig_info->si_pid,\
            exit_status);
    CNT--;
}

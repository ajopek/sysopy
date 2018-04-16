#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>


int child_count;
int parent_count;
int sent_count;
pid_t child;
pid_t parent;
int mode;

void sig_parent(int sig)
{
    parent_count++;
    if (sig == SIGUSR1 || sig == SIGRTMIN)
    {
        printf("Parent received %i signals \n", parent_count);
    }
    else if (sig == SIGINT)
    {
        printf("Parent process: received SIGINT \n");
        kill(child, (mode != 3) ? SIGUSR2 : SIGRTMAX);
        exit(EXIT_SUCCESS);
    }
}

void sig_child(int sig)
{
    child_count++;
    if (sig == SIGUSR1 || sig == SIGRTMIN)
    {
        printf("Child process: received %i signals\n", child_count);
        kill(parent, (sig == SIGUSR1) ? SIGUSR1 : SIGRTMIN);
    }
    else if (sig == SIGUSR2 || sig == SIGRTMAX)
    {
        printf("Child process: received %s \n", (sig == SIGUSR2) ? "SIGUSR2" : "SIGRTMAX");
        exit(EXIT_SUCCESS);
    }
}

void parent_function(int amount)
{
    struct sigaction act;
    act.sa_handler = sig_parent;
    sigfillset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGUSR1);
    act.sa_flags = 0;
    if (sigaction(SIGINT, &act, NULL) < -1 ||
        sigaction((mode != 3) ? SIGUSR1 : SIGRTMIN, &act, NULL) < -1)
    {
        exit(EXIT_FAILURE);
    }
    int i;
    for (i = 0; i < amount; i++)
    {
        sent_count++;
        printf("Parent process: send %i signals \n", sent_count);
        kill(child, (mode != 3) ? SIGUSR1 : SIGRTMIN);
        if (mode == 2) pause();
    }
    kill(child, (mode != 3) ? SIGUSR2 : SIGRTMAX);
    alarm(1);
    while(parent_count < amount) pause();
}

void child_function(void)
{
    struct sigaction act;
    act.sa_handler = sig_child;
    sigfillset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGUSR1);
    act.sa_flags = 0;
    if (sigaction(SIGINT, &act, NULL) < -1 ||
        sigaction((mode != 3) ? SIGUSR1 : SIGRTMIN, &act, NULL) < -1 ||
        sigaction((mode != 3) ? SIGUSR2 : SIGRTMAX, &act, NULL) < -1)
    {
        exit(EXIT_FAILURE);
    }
    while (1) {pause();}
}

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        exit(EXIT_FAILURE);
    }
    int amount = atoi(argv[1]);
    mode = atoi(argv[2]);

    sigset_t newmask;
    sigset_t oldmask;
    sigfillset(&newmask);
    sigdelset(&newmask, SIGINT);
    sigdelset(&newmask, SIGUSR1);
    sigdelset(&newmask, SIGUSR2);
    sigdelset(&newmask, SIGRTMIN);
    sigdelset(&newmask, SIGRTMAX);
    sigprocmask(SIG_SETMASK, &newmask, &oldmask);

    parent = getpid();
    child_count = sent_count = parent_count = 0;
    if ((child = fork()) < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (child == 0) child_function();
    else {
        sleep(1);
        parent_function(amount);
    }
    exit(EXIT_SUCCESS);
}
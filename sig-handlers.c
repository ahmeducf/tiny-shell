#include "sig-handlers.h"


void sigchld_handler(int sig) 
{
    int olderrno = errno;
    sigset_t mask_all, prev_all;
    pid_t pid, jid;

    sigfillset(&mask_all);
    int status; /* status of the child that caused waitpid to return */

    // Reap as many as possible, but should not wait for unexited child!
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {
        jid = pid2jid(pid);
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        /* Child terminated normally */
        if (WIFEXITED(status)) {
            deletejob(jobs, pid);
        }
        /* Child terminated due to signal that was not caught */
        else if (WIFSIGNALED(status)) {
            deletejob(jobs, pid);
            printf("Job [%d] (%d) terminated by signal %d\n",
                   jid, pid, WTERMSIG(status));
        }
        /* Child that caused the return is currently stopped */
        else if (WIFSTOPPED(status)) {
            job_t *job = getjobpid(jobs, pid);
            job->state = ST;
            printf("Job [%d] (%d) stopped by signal %d\n",
                   jid, pid, WSTOPSIG(status));
        }
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
    errno = olderrno;
    return;
}



void sigint_handler(int sig) 
{
    pid_t pid = fgpid(jobs);
    /* Send SIGINT signal to the process group that contains process pid */
    kill(-pid, SIGINT);
    return;
}



void sigtstp_handler(int sig) 
{
    pid_t pid = fgpid(jobs);
    /* Send SIGTSTP signal to the process group that contains process pid */
    kill(-pid, SIGTSTP);
    return;
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}
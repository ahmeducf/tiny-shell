#include "tiny-shell.h"

/* Global variables */
static char **environ;             /* defined in libc */
/* End global variables */


void eval(char *cmdline) 
{
    char *argv[MAXARGS];    /* Argument list execve() */
    char buf[MAXLINE];      /* Holds modified command line */
    int bg;                 /* Should the job run in bg or fg? */
    pid_t pid;              /* Process id */
    sigset_t mask, mask_all, prev_mask;

    sigfillset(&mask_all);
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);

    strcpy(buf, cmdline);
    bg = parseline(cmdline, argv);
    if (argv[0] == NULL)
        return;     /* Ignore empty line */
    
    if (!builtin_cmd(argv)) {
        sigprocmask(SIG_BLOCK, &mask, &prev_mask);
        if ((pid = fork()) == 0) {      /* Child runs user job */
            sigprocmask(SIG_SETMASK, &prev_mask, NULL);
            setpgid(0, 0);
            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found\n", argv[0]);
                exit(0);
            }
        }
        
        if (!bg) {
            sigprocmask(SIG_BLOCK, &mask_all, NULL);
            addjob(jobs, pid, FG, cmdline);
            sigprocmask(SIG_SETMASK, &prev_mask, NULL);

            waitfg(pid);    /* Parent waits for fg jobs to terminate */
        }
        else {
            sigprocmask(SIG_BLOCK, &mask_all, NULL);
            addjob(jobs, pid, BG, cmdline);
            sigprocmask(SIG_SETMASK, &prev_mask, NULL);

            job_t *job = getjobpid(jobs, pid);
            printf("[%d] (%d) %s", job->jid, pid, cmdline);
        }
    }
    return;
}


int parseline(const char *cmdline, char **argv) 
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
	    buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'') {
	    buf++;
	    delim = strchr(buf, '\'');
    }
    else {
	    delim = strchr(buf, ' ');
    }

    while (delim) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) /* ignore spaces */
            buf++;

        if (*buf == '\'') {
            buf++;
            delim = strchr(buf, '\'');
        }
        else {
            delim = strchr(buf, ' ');
        }
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* ignore blank line */
	    return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
	    argv[--argc] = NULL;
    }
    return bg;
}


int builtin_cmd(char **argv) 
{
    if (!strcmp(argv[0], "quit"))
        exit(0);
    if (!strcmp(argv[0], "jobs")) {
        listjobs(jobs);
        return 1;
    }
    if (!strcmp(argv[0], "bg") || !strcmp(argv[0], "fg")){
        do_bgfg(argv);
        return 1;
    }
    return 0;     /* not a builtin command */
}


void do_bgfg(char **argv) 
{
    if (argv[1] == NULL) {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }
    char *id = argv[1];

    if (!isvalid(id)) {
        printf("%s: argument must be a PID or %%jobid\n", argv[0]);
        return;
    }
    job_t *job;

    /* given jid */
    if (id[0] == '%') {
        char *tmp = id+1;
        if ((job = getjobjid(jobs, atoi(tmp))) == NULL) {
            printf("(%s): No such job\n", tmp);
            return;
        }
    }
    else {  /* given pid */
        if ((job = getjobpid(jobs, atoi(id))) == NULL) {
            printf("(%s): No such process\n", id);
            return;
        }
    }


    if (!strcmp(argv[0], "bg")) {   /* bg command */
        kill(-(job->pid), SIGCONT);
        job->state = BG;
        printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
    }
    else {  /* fg command */
        job->state = FG;
        kill(-(job->pid), SIGCONT);
        waitfg(job->pid);
    }
    return;
}


void waitfg(pid_t pid)
{
    sigset_t mask;
    sigemptyset(&mask);
    while (fgpid(jobs) == pid)
    {
        sigsuspend(&mask);
    }
    return;
}


int isvalid(char *id)
{
    int idx = (id[0] == '%') ? 1 : 0;
    int n = strlen(id);
    for (int i = idx; i < n; i++) {
        if (!isdigit(id[i]))
            return 0;
    }
    return 1;
}


void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}
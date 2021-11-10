#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

typedef void handler_t(int);



/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler);


/*
 * app_error - application-style error routine
 */
void app_error(char *msg);


/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg);

#endif
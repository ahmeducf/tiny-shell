# Makefile for the CS:APP Shell Lab


CC = gcc
CFLAGS = -g -Wall -Werror  -m64


tsh: tsh.c tiny-shell.c job.c wrappers.c sig-handlers.c tiny-shell.h job.h wrappers.h sig-handlers.h job.h wrappers.h sig-handlers.h
	$(CC) $(CFLAGS) -o tsh tsh.c tiny-shell.c job.c wrappers.c sig-handlers.c



#
# Clean the src dirctory
#
clean:
	rm -f tsh



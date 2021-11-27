# Tiny Shell
Tiny Shell is a simple Unix shell program that supports job control, making use of the interface provided by Unix operating systems.

## How does it work?
Tiny Shell performs a sequence of (read/evaluate) steps and then terminates.

It has the following detailed behaviour:

- The read step reads a command-line from the user. The evaluate step parses the command-line and runs programs on behalf of the user.

- **tiny-shell** file contains ***eval()*** function that contains the code which evaluates the command-line. Its first task is to call the ***parseline()*** function, which parse the space-separated command-line arguments and builds the argv vector that will eventually be passed to execve. The first argument is assumed to be either the name of a built-in shell command that is interpreted immediately, or an executable object file that will be loaded and run in the context of a new child process.

- If the last argument is an *&* character,then ***parseline()*** returns 1, indicating that the program should be executed in the *background* (the shell doesn't wait for it to complete). Otherwise, it returns 0, indicating that the program should be run in the *foreground* (the shell waits for it to complete).

- After parsing the command-line, the ***eval()*** function calls the ***builtin_command()*** function, which checks whether the first command-line argument is a built-in shell command. If so, it interprets the command immediately and returns 1. Otherwise, it returns 0.

- If ***builtint_command()*** returns 0, then the shell creates a child process and excutes the requested program inside the child and adds it to the ***jobs*** list. If the user has asked for the program to run in the background, then the shell returns to the top of the loop and waits for the next command-line. Otherwise the shell uses the ***waitfg()*** function to wait for the job to terminate. when the job terminates the shell reaps it, delete it from ***jobs*** list, and goes on to the next iteration.

- If any of the background jobs terminated, it sends **SIGCHLD** signal to the shell, and the shell then reaps it through the ***sigchld_handler()*** function.



## what commands does Tiny Shell support?
Tiny Shell supports the following built-in commands:

  - The `quit` command terminates the shell.
  - The `jobs` command lists all background jobs.
  - The `bg <job>` command restarts `<job>` by sending it a **SIGCONT** signal, and then runs it in the *background*. The `<job>` argument can be either a ***PID*** or a ***JID***.
  - The `fg <job>` command restarts `<job>` by sending it a **SIGCONT** signal, and then runs it in the *foreground*. The `<job>` argument can be either a ***PID*** or a ***JID***.

## Requirements
- Git
- GCC

## How to test it?
Run the following commands in a terminal:
  - `git clone https://github.com/ahmed-salah-ucf/tiny-shell.git`
  - `make`
  - `.\tsh`

## Thoughts
- The Project helped me become more familiar with the concepts of process control and signalling.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

pid_t child_pid = -1;
int child_running = 1;

void handle_sigtstp(int sig);
void handle_sigint(int sig);

int main()
{
  signal(SIGTSTP, handle_sigtstp); // Handle ctrl-z
  signal(SIGINT, handle_sigint);   // Handle ctrl-c

  child_pid = fork();
  if (child_pid == 0)
  {
    execlp("yes", "yes", NULL);
    perror("execlp failed");
    exit(EXIT_FAILURE);
  }
  else if (child_pid > 0)
  {
    printf("Child process started with PID: %d\n", child_pid);
    printf("Waiting for signals. Use ctrl-z to stop/resume 'yes' process and ctrl-c to exit.\n");

    while (1)
    {
      pause();
    }
  }
  else
  {
    perror("fork failed");
    exit(EXIT_FAILURE);
  }

  return 0;
}

// SIGTSTP == (ctrl-z)
void handle_sigtstp(int sig)
{
  if (child_pid > 0)
  {
    if (child_running)
    {
      printf("ctrl-z caught, stopping child process\n");
      kill(child_pid, SIGSTOP);
      child_running = 0;
    }
    else
    {
      printf("ctrl-z caught, resuming child process\n");
      kill(child_pid, SIGCONT);
      child_running = 1;
    }
  }
}

void handle_sigint(int sig)
{
  if (child_pid > 0)
  {
    printf("ctrl-c caught, killing child process and exiting\n");
    kill(child_pid, SIGKILL);
    waitpid(child_pid, NULL, 0);
    exit(0);
  }
}

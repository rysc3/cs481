#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

long handleFib(long n, long m);
long fib_seq(long n);
long finalFib;

int main(int argc, char *argv[])
{
  if (argc != 5)
    return 1;
  char *endptr1;
  char *endptr2;

  int opt;
  char *n_arg = NULL;
  char *m_arg = NULL;

  while ((opt = getopt(argc, argv, "F:S:")) != -1)
  {
    switch (opt)
    {
    case 'F':
      n_arg = optarg; // Argument for -F
      break;
    case 'S':
      m_arg = optarg; // Argument for -S
      break;
    default: /* Invalid argument */
      fprintf(stderr, "Usage: %s [-F n] [-S m]\n", argv[0]);
      return 1;
    }
  }

  long n = strtol(n_arg, &endptr1, 10);
  long m = strtol(m_arg, &endptr2, 10);
  if (*endptr1 != '\0' || *endptr2 != '\0')
  {
    printf("Non-integer argument given.\n");
    return 1;
  }

  if (((n - 1) > m) || ((n - 2) > m))
  {
    finalFib = handleFib(n, m);
  }else
  {
    finalFib = fib_seq(n);
  }

  printf("%ld\n", finalFib);
  return 0;
}

long handleFib(long n, long m)
{
  // Check cases
  long finalVal = 0;
  if (n == 0)
    return 0;
  if (n == 1)
    return 1;
  
  long n1 = n - 1;
  long n2 = n - 2;
  long val1 = 0;
  long val2 = 0;

  //Setup shared memory. As an array of two longs.
  key_t key = ftok("PA02fib.c", 50);
  int id = shmget(key, sizeof(val1) + sizeof(val2), 0666 | IPC_CREAT);
  long* vals = (long*) shmat(id, NULL, 0);

  pid_t child1 = fork();
  if (child1 == 0)
  {
    long sendData = handleFib(n1, m);
    vals[0] = sendData;
    shmdt(vals);
    exit(EXIT_SUCCESS);
  }
  //Wait for child one to avoid memory conflicts.
  waitpid(child1, NULL, 0);
  val1 = vals[0];
  
  pid_t child2 = fork();
  if (child2 == 0)
  {
    long sendData2 = handleFib(n2, m);
    vals[1] = sendData2;
    shmdt(vals);
    exit(EXIT_SUCCESS);
  }
  // Wait for child 2.
  waitpid(child2, NULL, 0);
  val2 = vals[1];

  shmdt(vals);
  /* Combine case 1 results to get fib(n) */
  finalVal = val1 + val2;


  return finalVal;
}

long fib_seq(long n)
{
  /* slow/recursive implementation of Fib */
  int i, rint = (rand() % 30);
  double dummy;
  for (i = 0; i < rint * 100; i++)
  {
    dummy = 2.345 * i * 8.765 / 1.234;
  }
  if (n == 0)
    return 0;
  else if (n == 1)
    return 1;
  else
    return fib_seq(n - 1) + fib_seq(n - 2);
}

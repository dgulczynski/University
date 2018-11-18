/* Imię nazwisko: Maksymilian Debeściak
 * Numer indeksu: 999999
 *
 * Oświadczam, że:
 *  - rozwiązanie zadania jest mojego autorstwa,
 *  - jego kodu źródłowego dnie będę udostępniać innym studentom,
 *  - a w szczególności nie będę go publikować w sieci Internet.
 *
 * Q: Jak proces wykrywa, że drugi koniec potoku został zamknięty?
 * A: Próba pisania do zamkniętego potoku powoduje przyjście sygnału SIGTERM.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int fd_read, fd_write;

static void func_1() {
  static int words = 0;
  char c;
  while (read(fd_read, &c, 1)) {
    write(fd_write, &c, 1);
  }

  close(fd_read);
  close(fd_write);
  fprintf(stderr, "words = %d\n", words);
}

static void func_2() {
  static int removed = 0;
  char c;

  while (read(fd_read, &c, 1)) {
    if (c != ' ')
      write(fd_write, &c, 1);
  }

  close(fd_read);
  close(fd_write);
  fprintf(stderr, "removed = %d\n", removed);
}

static void func_3() {
  static int chars = 0;
  char c;
  while (read(fd_read, &c, 1) > 0) {
    write(fd_write, &c, 1);
  }

  close(fd_read);
  close(fd_write);
  fprintf(stderr, "chars = %d\n", chars);
}

int main(void) {
  int pipes[4];
  pipe(pipes);
  pipe(pipes + 2);

  pid_t proc[3];

  if ((proc[0] = fork()) == 0) {
    fd_read = STDIN_FILENO;
    fd_write = pipes[1];
    close(pipes[0]);
    close(pipes[2]);
    close(pipes[3]);
    func_1();
  } else if ((proc[1] = fork()) == 0) {
    fd_read = pipes[0];
    fd_write = pipes[3];
    close(pipes[1]);
    close(pipes[2]);
    func_2();
  } else if ((proc[2] = fork()) == 0) {
    fd_read = pipes[2];
    fd_write = STDOUT_FILENO;
    close(pipes[0]);
    close(pipes[1]);
    close(pipes[3]);
    func_3();
  } else {
    close(pipes[0]);
    close(pipes[1]);
    close(pipes[2]);
    close(pipes[3]);
    for (unsigned short i = 0; i < 3; i++) {
      int status;
      if (waitpid(proc[i], &status, 0) == -1) {
        perror("waitpid failed");
        return EXIT_FAILURE;
      }

      if (WIFEXITED(status)) {
        const int es = WEXITSTATUS(status);
        printf("exit status of proc %d was %d\n", i, es);
      }
    }
  }
  return EXIT_SUCCESS;
}
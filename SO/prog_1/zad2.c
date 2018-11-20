/* Imię nazwisko: Maksymilian Debeściak
 * Numer indeksu: 999999
 *
 * Oświadczam, że:
 *  - rozwiązanie zadania jest mojego autorstwa,
 *  - jego kodu źródłowego dnie będę udostępniać innym studentom,
 *  - a w szczególności nie będę go publikować w sieci Internet.
 *
 * Q: Zdefiniuj proces "sierotę".
 * A: Proces w chwilowym stanie pomiędzy śmiercią rodzica a przygarnięciem
 *    procesu przez inny.
 *
 * Q: Co się stanie, jeśli główny proces nie podejmie się roli żniwiarza?
 * A: Żniwiarzem osieroconego procesu zostaje init(1).
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  // args for calling ps
  char *args[] = {"/bin/ps", "-o", "pid,ppid,cmd", NULL};

  // set this process as subreaper
  prctl(PR_SET_CHILD_SUBREAPER);

  pid_t pid;
  // create child
  if ((pid = fork()) == 0) {
    // create granchild
    if ((pid = fork()) == 0) {
      // call ps
      if (fork() == 0) {
        execve(args[0], args, __environ);
      }
      wait(NULL);
    } else {
      // let child die
      printf("grandchild pid is %d\n", pid);
    }
  } else {
    printf("parent pid is %d\n", getpid());
    printf("child pid is %d\n", pid);
    // wait(NULL); // wait for granchild
    sleep(1);
  }
}

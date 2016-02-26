#include "ucode.c"

main(int argc; char *argv[ ])
{
  int pid, status;
  pid = vfork();
  if (pid){
     printf("parent %d waits\n", getpid());
     pid = wait(&status);
     printf("parent %d waited, dead child=%d\n", getpid(),pid);
  }
  else{
    printf("child %d of vfork() begins in segment=0x%x\n", getpid(), getcs());
    printf("child %d exec\n");
    syscall(8, "u2 one two three");
    printf("exec failed\n");
  } 
}

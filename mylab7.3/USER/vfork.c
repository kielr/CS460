/********************************************************************
Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#include "ucode.c"

main(int argc, char *argv[ ])
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

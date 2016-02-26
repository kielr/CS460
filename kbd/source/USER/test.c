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

  printf("parent %d forks\n", getpid());
  pid = fork();

  if (pid){
      printf("parent %d waits\n", getpid());
      pid = wait(&status);
      printf("parent waited\n");
  }
  else{
    printf("child %d exec to u2 file", getpid()); 
    getc();
    printf("child %d exec\n", getpid());
    exec("u2 one two three");
    printf("exec failed");
  }
}

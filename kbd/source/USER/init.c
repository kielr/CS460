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
#include "../type.h"
#include "ucode.c"

int child, pid, status;

main(int argc, char *argv[])
{
  printf("init forks a sh process\n");
  child = fork();

  if (child)
     parent(); 
  else
     sh();
}

int parent()
{
    printf("init waits\n"); 
    while(1){
      pid = wait(&status);

      if (pid==child){
	 child = fork();

         if(child)
	    continue;
         else
	   sh();
      }
      printf("init : I just buried an orphan process %d\n", pid);
    }
}

int sh()
{
  printf("child exec to sh\n");
  exec("sh");
}
              

#include "ucode.c"
int color;

main(int argc, char *argv[])
{
  char name[64]; int pid, cmd, segment, i;

  while(1){
       pid = getpid();
       color = 0x000B + (pid % 5);
       segment = (pid+1)*0x1000;
       printf("==============================================\n");
       printf("I am proc %din U mode: segment=%x\n", pid, segment);
       show_menu();
       printf("Command ? ");
       gets(name);
       if (name[0]==0)
           continue;

       cmd = find_cmd(name);

       switch(cmd){
           case 0 : getpid();   break;
           case 1 : ps();       break;
           case 2 : chname();   break;
           case 3 : kmode();    break;
           case 4 : kswitch();  break;
           case 5 : wait();     break;

           case 6 : exit();     break;
           case 7 : fork();     break;
           case 8 : exec();     break;
           case 9 : do_itimer(); break;

           default: invalid(name); break;
       }
  }
}

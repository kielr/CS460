#include "ucode.c"
int color;

main(int argc, char *argv[])
{ 
  char name[64]; int pid, cmd, segment, i;

  while(1){
       pid = getpid();
       segment = (pid+1)*0x1000;
       color = 0x0000B + (pid % 5);

       printf("==============================================\n");
       printf("Das ist Prozess %d in der U Weise: Segment=%x\n", pid,segment);
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
           case 9 : do_vfork(); break;
           default: invalid(name); break;
       } 
  }
}





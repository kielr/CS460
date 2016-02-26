/*kfork() creates a child process ready to run in Kmode but
  with a Umode image "filename" loaded to its segment AND
  ready to go back to Umode to execute the Umode image.
  PROC *kfork(char *filename)
 {
 (1). create a child PROC ready to run from body()in Kmode
 (2). segment=(child pid + 1)*0x1000; //child Umode segment
 if (filename){
 (3). load filename to child segment as its Umode image;
 (4). set up child's ustack for it to return to Umode to execute
 the loaded image;
 }
 (5). return child PROC pointer;
 }

  */
PROC *kfork(char *filename)
{
    int i,segment;
    PROC *p = get_proc(&freeList); //Need to get a proc from the freeList
    if(!p) //If p is NULL then we failed to get a free PROC from the freeList
    {
        printf("kfork(): failed, no more PROCs in freeList.\n"); //Print an error and
        return 0; // return.
    }
    //If we get here we have a free process, we need to set it to READY
    p->status = READY;
    //Priority is 1 for processes except P0
    p->priority = 1;
    //This is the child process, so set its ppid to the current running process' pid
    p->ppid = running->ppid;
    //Now we need to initialize the child's execution stack.
    for(i=1; i < 10; i++)
    {
        p->kstack[SSIZE-i] = 0;
    }

    p->kstack[SSIZE-1] = (int) body; //This is the return address for body
    p->ksp = &p->kstack[SSIZE-9]; //This stores a local reference to the current proc's stack pointer for later use

    //Now we must load "filename" into p
    segment = (p->pid + 1) * 0x1000;
    if(filename)
    {
        load(filename, segment);
    }

    enqueue(readyQueue, p); //PROC p is now ready to be entered into the readyQueue.
    nproc++;
    return p;
}

int body()
{
  char c;
  printf("proc %d resumes to body()\n", running->pid);
  while(1){
    printf("-----------------------------------------\n");
    printList(freeList);
    printList(readyQueue);
    printf("-----------------------------------------\n");

    printf("proc %d running: parent = %d  enter a char [s|f|w|q|u] : ",
	   running->pid, running->parent->pid);
    c = getc(); printf("%c\n", c);
    switch(c){
       //----------------------------------------
       case 's' : do_tswitch();   break;
       case 'f' : do_kfork();     break;
       case 'w' : do_wait();      break;
       case 'q' : do_exit();      break;
    //--- above commands are same as in LAB#2. The 'u' command is NEW -------

       case 'u' : goUmode();      break; // <=== go back to Umode
    }
  }
}

int do_ps()
{
  //print ALL proc information, i.e. name, pid, ppid, status
  printf("RUNNING PROCESS INFORMATION:\n");
  printf("running->name=%s\n", running->name);
  printf("running->pid=%d\n", running->pid);
  printf("running->ppid=%d\n", running->ppid);
  printf("running->status=%d\n", running->status);
}
int chname(char *newname)
{
  //change process name to newname
  char *c = newname;
  int i = 0;
  while(c)
  {
      running->name[i] = *c;
      i++;
      c++;
  }
  return 1;
}

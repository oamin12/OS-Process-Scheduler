#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

///==============================
//don't mess with this variable//
int *shmaddr; //
//===============================
int *shmaddr2;

int getClk()
{
    return *shmaddr;
}

/*
 * All processes call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All processes call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
void destroyRemTime()
{
    shmdt(shmaddr);
    
}

void destroyMsgQueue(int msgid)
{
    msgctl(msgid, IPC_RMID, (struct msqids_ds *)0);
}

int power(int m, int n)
{
    int x = 1;
    for(int i = 0; i < n; ++i)
        x *= m;
        
    return x;
}

struct Process {
  int id;
  int arrival;
  int runtime;
  int priority;
  int memsize;
  int priorityNew;
  int remainingtime;
  int starttime;
  int wait;
  int TA;
  int WTA;
};

struct msgbuff {
    int mtype;
    struct Process mprocess;
};

struct pcb {
    struct Process PCBprocess;
    int remTime;
    int pid;

    int memorySize;
    int memoryStart;
    int memoryEnd;
};

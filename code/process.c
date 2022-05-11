#include "headers.h"


void continue_handler(int);
/* Modify this file as needed*/
int remainingtime, id, old_clk;

int main(int agrc, char *argv[])
{
    
    initClk();
    id = atoi(argv[1]);
    remainingtime = atoi(argv[2]);
    //TODO The process needs to get the remaining time from somewhere
    //remainingtime = ??;
    signal (SIGCONT, continue_handler);
    old_clk = getClk();

    printf("process id = %d and running time = %d \n", id, remainingtime);
        

    printf("my id = %d and start at = %d\n",id, getClk());
    while (remainingtime > 0)
    {
        if(getClk() - old_clk >= 1)
        {
            printf("hi my id = %d and my rem time = %d\n",id,remainingtime);
            remainingtime--;
            old_clk = getClk();
        }
     
    }

    printf("my id = %d and finished at = %d\n",id, getClk());
    kill(getppid(),SIGUSR1);
    destroyClk(false);

    return 0;
}

void continue_handler(int signum)
{
    old_clk = getClk();
    signal (SIGCONT, continue_handler);
}
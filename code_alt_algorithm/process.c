#include "headers.h"


void continue_handler(int);
/* Modify this file as needed*/
int remainingtime, id, old_clk;

int main(int agrc, char *argv[])
{
    
    initClk();
    id = atoi(argv[1]);
    remainingtime = atoi(argv[2]);
    signal (SIGCONT, continue_handler);
    old_clk = getClk();

    
    while (remainingtime > 0)
    {
        //setRemTime(remainingtime,id);
        if(getClk() - old_clk >= 1)
        {
            remainingtime--;
            old_clk = getClk();
        }
        if(remainingtime <= 0)
        {
            kill(getppid(),SIGUSR1);
        }

    }
    printf("my id = %d and finished at = %d\n",id, getClk());
    destroyClk(false);

    return 0;
}

void continue_handler(int signum)
{
    old_clk = getClk();
    signal (SIGCONT, continue_handler);
}

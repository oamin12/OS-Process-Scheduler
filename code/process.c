#include "headers.h"

/* Modify this file as needed*/
int remainingtime, id, old_clk;

int main(int agrc, char *argv[])
{
    initClk();
    id = atoi(argv[1]);
    remainingtime = atoi(argv[2]);
    //TODO The process needs to get the remaining time from somewhere
    //remainingtime = ??;

    old_clk = getClk();

    printf("process id = %d", id);
    if(id != 1)
    {
        raise(SIGTSTP); //stoping the process when being created
    }
        

    printf("start at = %d \n", getClk());
    while (remainingtime > 0)
    {
        if(getClk() - old_clk >= 1)
        {
            remainingtime--;
            old_clk = getClk();
        }
        
    }

    printf("finished at = %d and old_clk = %d \n", getClk(), old_clk);
    destroyClk(false);

    return 0;
}

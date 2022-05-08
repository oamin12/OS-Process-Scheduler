#include "headers.h"

/* Modify this file as needed*/
int remainingtime, id;

int main(int agrc, char *argv[])
{
    
    initClk();
    id = atoi(argv[1]);
    remainingtime = atoi(argv[2]);
    printf("id = %d, rem = %d\n", id, remainingtime);
    //TODO The process needs to get the remaining time from somewhere
    //remainingtime = ??;
    while (remainingtime > 0)
    {
        // remainingtime = ??;
    }

    destroyClk(false);

    return 0;
}

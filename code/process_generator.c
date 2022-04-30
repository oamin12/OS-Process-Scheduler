#include "headers.h"

void clearResources(int);

int main(int argc, char **argv)
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    if (argc<4) {	 
		printf("Too few arguments. Exiting!\n");
		exit(1);
	}
    int q;
    if (argc==6) {
        q=atoi(argv[5]);	 
	}
    //printf("%d \n",q);
    FILE* file = fopen(argv[1], "r");
    int arr[1000];
    int num;
    int i=0;
    fscanf(file, "%*[^\n]");
    while(fscanf(file, "%d", &num) > 0) {
        arr[i] = num;
        i++;
    }
    int arrsize=i;
    int nProcess=arrsize/4;
    fclose(file);
    

    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    int schAlgo=atoi(argv[3]);


    // 3. Initiate and create the scheduler and clock processes.
    

    // 4. Use this function after creating the clock process to initialize clock.
    ////initClk();
    // To get time use this function. 
    //int x = getClk();
    //printf("Current Time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    struct Process parr[nProcess];
    for(int i=0;i<nProcess;i++)
    {
        parr[i].id=arr[i*4];
        parr[i].arrival=arr[i*4+1];
        parr[i].runtime=arr[i*4+2];
        parr[i].priority=arr[i*4+3];
    }

    // printf("%d \n",parr[0].id);
    // printf("%d \n",parr[0].arrival);
    // printf("%d \n",parr[0].runtime);
    // printf("%d \n",parr[0].priority);

    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    //destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}

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
    if ((argc==6) && (atoi(argv[3])==3 || atoi(argv[3])==4)) {
        q=atoi(argv[5]);
	}
    else if((argc<6) && (atoi(argv[3])==3 || atoi(argv[3])==4))
    {
        printf("The scheduling algorithm needs a quantum. Exiting!\n");
		exit(1);
    }

    
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
    char * schAlgo=argv[3];
    // 3. Initiate and create the scheduler and clock processes.
    pid_t  pid;
    pid = fork();
 
   if (pid == -1){
 
      // pid == -1 means error occurred
      printf("error in forking occured\n");
      exit(EXIT_FAILURE);
   }
   else if (pid == 0){
 
      char * argv_list[] = {"./clk.out",NULL};
 
      execv("./clk.out",argv_list);
      exit(0);
   }
    pid = fork();
    if (pid == -1){
 
      // pid == -1 means error occurred
      printf("error in forking occured\n");
      exit(EXIT_FAILURE);
   }
   else if (pid == 0){
        char temp_nProcess[10];
        sprintf(temp_nProcess, "%d", nProcess);
        
        if((argc==6) && (atoi(argv[3])==3 || atoi(argv[3])==4))
        {
            char * argv_list[] = {"./scheduler.out",schAlgo, temp_nProcess,argv[5],NULL};  
            execv("./scheduler.out",argv_list);
  
        }
        else
        {   
            char * argv_list[] = {"./scheduler.out",schAlgo, temp_nProcess,NULL};
            execv("./scheduler.out",argv_list);
        }
        
        exit(0);
   }
    
    //system("./clk.out &");
    printf("hello there\n");
    // 4. Use this function after creating the clock process to initialize clock.
    initClk();
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
        parr[i].remainingtime=arr[i*4+2];
    }

    // printf("%d \n",parr[0].id);
    // printf("%d \n",parr[0].arrival);
    // printf("%d \n",parr[0].runtime);
    // printf("%d \n",parr[0].priority);

    // 6. Send the information to the scheduler at the appropriate time.
    key_t key_id;
    int  msgq_id, process_order, send_val;
    process_order = 0;

    key_id = ftok("key", 65); //create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); //create message queue and return id
    printf("Message Queue ID in pg = %d\n", msgq_id);


    while(process_order != nProcess){
        //printf("the time is: %d \n",getClk());
        if(parr[process_order].arrival == getClk())
        {
            struct msgbuff msg;
            msg.mtype = 7;
            msg.mprocess.id = parr[process_order].id;
            msg.mprocess.arrival = parr[process_order].arrival;
            msg.mprocess.runtime = parr[process_order].runtime;
            msg.mprocess.priority = parr[process_order].priority;
            msg.mprocess.remainingtime= parr[process_order].remainingtime;

            send_val = msgsnd(msgq_id, &msg, sizeof(msg.mprocess), !IPC_NOWAIT);
            //printf("process is: %d\n",parr[process_order].id);
            printf("time is: %d \n",getClk());
            //printf("arrived at: %d",msg.mprocess.arrival);
            process_order++;
        }
        
    }
    
    int status;
    pid = wait(&status);
    // 7. Clear clock resources
    //destroyMsgQueue(msgq_id);
    
    if (WIFEXITED(status))
    {
        destroyClk(true);
        exit(0);
    }     

    exit(0);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
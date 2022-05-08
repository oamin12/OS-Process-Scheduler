#include "headers.h"

void check_arrival (int );


int rec_val, msgq_id;
struct pcb pcb_table[100]; // first Process at index 1

int main(int argc, char *argv[])
{
    initClk();
    //Queue q = initQueue();
    printf("the algo is: %d \n",atoi(argv[1]));
    key_t key_id;
        

        key_id = ftok("key", 65);               //create unique key
        msgq_id = msgget(key_id, 0666 | IPC_CREAT); //create message queue and return id

        if (msgq_id == -1)
        {
            perror("Error in create");
            exit(-1);
        }
        printf("Message Queue ID = %d\n", msgq_id);

        
        while(1)
        {
            //printf("Entered MSG REC");
            /* receive all types of messages */
            //rec_val = msgrcv(msgq_id, &message, sizeof(message.mprocess), 0, !IPC_NOWAIT);
            //int priority=message.mprocess.priority;

                //printf("\nMessage received: %d\n", message.mprocess.id);
                //push to Queue/(DS be sefa 3ama) according to algo
                if (atoi(argv[1])==1)
                {
                    check_arrival(1);
                    printf("SJF \n");
                    //pEnqueue(q,message.mprocess.id, priority);
                    //if(message.mprocess.id==5)
                    //{
                    //printf("HIGHEST PRIORITY PROCESS: %d \n",dequeue(q));
                    //printf("HIGHEST PRIORITY PROCESS: %d \n",dequeue(q));
                    //printf("HIGHEST PRIORITY PROCESS: %d \n",dequeue(q));
                    //printf("HIGHEST PRIORITY PROCESS: %d \n",dequeue(q));
                    //printf("HIGHEST PRIORITY PROCESS: %d \n",dequeue(q));
                    
                   // }
                }
                else if (atoi(argv[1])==2)
                {

                }
                else if (atoi(argv[1])==3)
                {

                }
                else
                {

                }
        } 
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.
    //destroyClk(true);
}

void check_arrival (int algo_num)
{
    struct msgbuff message;
    int m_pid;

    rec_val = msgrcv(msgq_id, &message, sizeof(message.mprocess), 0, !IPC_NOWAIT);
    if (rec_val == -1)
    {
        perror("Error in receive");
    }
    else
    {
        m_pid = fork();
        if (m_pid == -1){
 
            // pid == -1 means error occurred
            printf("error in forking occured\n");
            exit(EXIT_FAILURE);
        }
        else if (m_pid == 0){
            //creating process
            char temp_id[10];
            char temp_runtime[10];
            
            //converts id and runtime to strings instead of integers
            sprintf(temp_id, "%d", message.mprocess.id);
            sprintf(temp_runtime, "%d", message.mprocess.runtime);

            char * argv_list[] = {"./process.out", temp_id, temp_runtime, NULL};

            execv("./process.out",argv_list);
            exit(0);
        }

        //Adding the arrived process to the pcb table
        int x = message.mprocess.id;
        pcb_table[x].pid = m_pid;
        pcb_table[x].PCBprocess.id = message.mprocess.id;
        pcb_table[x].PCBprocess.runtime = message.mprocess.runtime;
        pcb_table[x].PCBprocess.arrival = message.mprocess.arrival;
        pcb_table[x].PCBprocess.priority = message.mprocess.priority;
        pcb_table[x].PCBprocess.remainingtime = message.mprocess.remainingtime;

    }
}
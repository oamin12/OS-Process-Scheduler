#include "headers.h"

void check_arrival (int );
void child_exit_handler(int );

int rec_val, msgq_id, num_processes, current_process_id, choosed_algo;
struct pcb pcb_table[100]; // first Process at index 1
struct Queue *priority_q;
struct Node *Node_to_beinserted;

int main(int argc, char *argv[])
{
    initClk();
    signal(SIGCHLD, child_exit_handler);

    priority_q = createQueue();
    num_processes = atoi(argv[2]);

    choosed_algo = atoi(argv[1]);

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

                    if(is_empty(priority_q) && num_processes == 0)
                    {
                        break;
                    }
                    
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
    destroyClk(true);
}

void check_arrival (int algo_num)
{
    struct msgbuff message;
    int m_pid;

    int flag_waiting = 0;

    do{
        rec_val = msgrcv(msgq_id, &message, sizeof(message.mprocess), 0, IPC_NOWAIT);
        
        if (rec_val == -1 && is_empty(priority_q))
        {
            rec_val = msgrcv(msgq_id, &message, sizeof(message.mprocess), 0, !IPC_NOWAIT);
            flag_waiting = 1;
        }
        
        if(rec_val != -1)
        {
            num_processes--;
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

            printf("flag == %d", flag_waiting);
            if(flag_waiting == 1) // if the first process or late process
            {
                kill(pcb_table[message.mprocess.id].pid, SIGCONT);
                current_process_id = message.mprocess.id;
            }

            //depending on the scheduling algo. we fill the right data structure
            if(algo_num == 1)
            {
                Node_to_beinserted = newNode(message.mprocess.id, message.mprocess.runtime);
                enQueue(priority_q, Node_to_beinserted); // enqueue this process
            }

            flag_waiting = 0;

        }

     }while(rec_val != -1);
}

void child_exit_handler(int signum)
{
    if(choosed_algo == 1)
    {
        deQueue(priority_q);
        if(!is_empty(priority_q))
        {
            struct Node *next_process;
            next_process = peek_queue(priority_q);
            current_process_id = next_process->process_id;
            kill(pcb_table[current_process_id].pid, SIGCONT); //continue the new process
        }
    }
}

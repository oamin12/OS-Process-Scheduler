#include "headers.h"

void check_arrival (int );
void child_exit_handler(int );

int rec_val, msgq_id, num_processes, current_process_id, choosed_algo, old_clk;
struct pcb pcb_table[100]; // first Process at index 1
struct Queue *priority_q;
struct Node *Node_to_beinserted;

struct Node *running_process; //for Round Robin

int main(int argc, char *argv[])
{
    initClk();
    signal (SIGUSR1, child_exit_handler);

    priority_q = createQueue();
    num_processes = atoi(argv[2]);

    choosed_algo = atoi(argv[1]);
    current_process_id = -1;

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

            if (atoi(argv[1])==1) // (SJF)
            {
                check_arrival(1);

                if(current_process_id == -1 && !is_empty(priority_q))
                {
                    struct Node *next_process;
                    next_process = peek_queue(priority_q);
                    current_process_id = next_process->process_id;
                    printf("current_process_id = %d", current_process_id);
                    kill(pcb_table[current_process_id].pid, SIGCONT); //continue the new process
                    deQueue(priority_q);
                }

                
                if(is_empty(priority_q) && num_processes == 0 && current_process_id == -1)
                {
                    break;
                }
                
            }
            else if (atoi(argv[1])==2)
            {
                


            }
            else if (atoi(argv[1])==3) // (RR)
            {
                int quantum;
                check_arrival(1);
                // if no process is running
                //then run the first the process that comes
                if(current_process_id == -1 && !is_empty(priority_q)) 
                {
                    running_process = peek_queue(priority_q);
                    current_process_id = running_process->process_id;
                    printf("current_process_id = %d", current_process_id);
                    kill(pcb_table[current_process_id].pid, SIGCONT); //continue the new process
                    deQueue(priority_q);
                    old_clk = getClk();
                }

                //if the currentlly running process has reached the end of its quatumn
                //push it back in the queue(at the end) and run the next one
                //N.B any new process will be automatically at the end of the queue
                if(getClk() - old_clk == quantum)
                {
                    kill(pcb_table[running_process.id].pid, 20);
                    enQueue(running_process, Node_to_beinserted); //enqueuing running process again


                    running_process = peek_queue(priority_q);
                    current_process_id = running_process->process_id;
                    printf("current_process_id = %d", current_process_id);
                    kill(pcb_table[current_process_id].pid, SIGCONT); //continue the new process
                    deQueue(priority_q);
                    old_clk = getClk();
                }

                if(is_empty(priority_q) && num_processes == 0 && current_process_id == -1)
                {
                    break;
                }

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
            printf("flag == %d\n", flag_waiting);
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
            printf("stoping process id = %d and pid = %d\n", message.mprocess.id, m_pid);
            kill(m_pid, 20);
            //Adding the arrived process to the pcb table
            int x = message.mprocess.id;
            pcb_table[x].pid = m_pid;
            pcb_table[x].PCBprocess.id = message.mprocess.id;
            pcb_table[x].PCBprocess.runtime = message.mprocess.runtime;
            pcb_table[x].PCBprocess.arrival = message.mprocess.arrival;
            pcb_table[x].PCBprocess.priority = message.mprocess.priority;
            pcb_table[x].PCBprocess.remainingtime = message.mprocess.remainingtime;


            //depending on the scheduling algo. we fill the right data structure
            if(algo_num == 1)
            {
                Node_to_beinserted = newNode(message.mprocess.id, message.mprocess.runtime);
                enQueue(priority_q, Node_to_beinserted); // enqueue this process
            }
            else if(algo_num == 2)
            {

            }
            else if(algo_num == 3)
            {
                Node_to_beinserted = newNode(message.mprocess.id, 1);
                enQueue(priority_q, Node_to_beinserted); // enqueue this process
            }

        }
        //printf("flag waiting = %d", flag_waiting);
        flag_waiting = 0;

     }while(rec_val != -1);
}

void child_exit_handler(int signum)
{
    if(choosed_algo == 1)
    {
        current_process_id = -1;
    }
    else if(choosed_algo == 2)
    {

    }
    else if(choosed_algo == 3)
    {
        current_process_id = -1;
    }
}

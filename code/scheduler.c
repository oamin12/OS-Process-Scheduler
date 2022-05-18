//#include "headers.h"
#include "queue.h"
#include "headers.h"

void check_arrival (int );
void child_exit_handler(int );
int getBuddySize(int size);
void CreateMemoryQueue();
void AllocateProcessToMemory(int, int);
void DeallocateProcessToMemory(int );
void printfreelist();


int rec_val, msgq_id, num_processes, current_process_id, choosed_algo,old_clk, quantum;
int total_wait=0;
float total_WTA=0;
int total_runtime=0;
//memory allocation
struct Queue *free_memory[8];
int memory_allocated[1000];

struct pcb pcb_table[100]; // first Process at index 1
struct Queue *priority_q;
struct Queue *q1;
struct Queue *q2;
struct Queue *q3;
struct Queue *q4;
struct Node *Node_to_beinserted;
struct Node *running_process; //for Round Robin
FILE* file;


int main(int argc, char *argv[])
{
    file = fopen("scheduler.log", "w");
    fprintf(file, "#At time x process y state arr w total z remian y wait k \n");
    initClk();
    signal (SIGUSR1, child_exit_handler);
    priority_q = createQueue();
    CreateMemoryQueue();
    
    q1= createQueue();
    q2= createQueue();
    q3= createQueue();
    q4= createQueue();
    num_processes = atoi(argv[2]);

    choosed_algo = atoi(argv[1]);
    if(choosed_algo==3 ||choosed_algo==4)
    {
        quantum=atoi(argv[3]);
    }
    current_process_id = -1;

    printf("the algo is: %d\n",atoi(argv[1]));

        key_t key_id;


        key_id = ftok("key", 65);               //create unique key
        msgq_id = msgget(key_id, 0666 | IPC_CREAT); //create message queue and return id

        if (msgq_id == -1)
        {
            perror("Error in create");

            exit(-1);
        }


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
                    //printf("current_process_mem = %d",current_process_id);
                    int wait = getClk() - pcb_table[current_process_id].PCBprocess.arrival;
                    total_wait=total_wait+wait;
                    total_runtime=total_runtime+pcb_table[current_process_id].PCBprocess.runtime;
                    pcb_table[current_process_id].PCBprocess.wait=wait;
                    fprintf(file,"At time %d process %d started arr %d total %d remain %d wait %d \n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.runtime,wait);
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
                check_arrival(2);

                if(current_process_id == -1 && !is_empty(priority_q))
                {
                    struct Node *next_process;
                    next_process = peek_queue(priority_q);
                    current_process_id = next_process->process_id;
                    //printf("current_process_id = %d", current_process_id);
                    if(pcb_table[current_process_id].PCBprocess.remainingtime==pcb_table[current_process_id].PCBprocess.runtime)
                    {
                        pcb_table[current_process_id].PCBprocess.wait = getClk() - pcb_table[current_process_id].PCBprocess.arrival;
                        total_wait=total_wait+pcb_table[current_process_id].PCBprocess.wait;

                        pcb_table[current_process_id].PCBprocess.starttime=getClk();
                        pcb_table[current_process_id].PCBprocess.remainingtime=pcb_table[current_process_id].PCBprocess.runtime-(getClk()-pcb_table[current_process_id].PCBprocess.starttime);
                        fprintf(file,"At time %d process %d started arr %d total %d remain %d wait %d \n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.remainingtime,pcb_table[current_process_id].PCBprocess.wait);

                    }
                    else{
                        pcb_table[current_process_id].PCBprocess.wait = getClk() - pcb_table[current_process_id].PCBprocess.starttime;
                        total_wait=total_wait+pcb_table[current_process_id].PCBprocess.wait;
                        //pcb_table[current_process_id].PCBprocess.starttime=getClk();
                        pcb_table[current_process_id].PCBprocess.starttime=getClk();
                        fprintf(file,"At time %d process %d resumed arr %d total %d remain %d wait %d \n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.remainingtime,pcb_table[current_process_id].PCBprocess.wait);
                        }
                    

                    kill(pcb_table[current_process_id].pid, SIGCONT); //continue the new process
        
                    deQueue(priority_q);
                }


                if(is_empty(priority_q) && num_processes == 0 && current_process_id == -1)
                {
                    break;
                }


            }
            else if (atoi(argv[1])==3) // (RR)
            {
               check_arrival(3);
                // if no process is running
                //then run the first the process that comes
                if(current_process_id == -1 && !is_empty(priority_q)) 
                {
                    
                    running_process = peek_queue(priority_q);
                    current_process_id = running_process->process_id;
                    //printf("current_process_id = %d at time = %d\n", current_process_id, getClk());
                    pcb_table[current_process_id].PCBprocess.wait = getClk() - pcb_table[current_process_id].PCBprocess.arrival;
                    total_wait=total_wait+pcb_table[current_process_id].PCBprocess.wait;
                    pcb_table[current_process_id].PCBprocess.starttime=getClk();
                    fprintf(file,"At time %d process %d started arr %d total %d remain %d wait %d \n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.remainingtime,pcb_table[current_process_id].PCBprocess.wait);
                    kill(pcb_table[current_process_id].pid, SIGCONT); //continue the new process
                    old_clk = getClk();
                    deQueue(priority_q);
                    
                    //printqueue(priority_q);
                }

                //if the currentlly running process has reached the end of its quatumn
                //push it back in the queue(at the end) and run the next one
                //N.B any new process will be automatically at the end of the queue
                if(getClk() - old_clk >= quantum && (getClk() - old_clk)%quantum == 0 && !is_empty(priority_q) && current_process_id != -1)
                {
                    pcb_table[current_process_id].PCBprocess.remainingtime=(pcb_table[current_process_id].PCBprocess.remainingtime-quantum);
                    fprintf(file,"At time %d process %d stopped arr %d total %d remain %d wait %d \n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.remainingtime,pcb_table[current_process_id].PCBprocess.wait);
                    kill(pcb_table[current_process_id].pid, 20);

                    //if a new a process arrives at the same time the currently running process finishes
                    //the new should be placed first before the last one that was running
                    //--------------------------------------------------------------------------------------------------------
                    struct msgbuff message;
                    int m_pid;
                    rec_val = msgrcv(msgq_id, &message, sizeof(message.mprocess), 0, IPC_NOWAIT);
                    
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
                        kill(m_pid, 20);
                        //Adding the arrived process to the pcb table
                        int x = message.mprocess.id;
                        pcb_table[x].pid = m_pid;
                        pcb_table[x].PCBprocess.id = message.mprocess.id;
                        pcb_table[x].PCBprocess.runtime = message.mprocess.runtime;
                        pcb_table[x].PCBprocess.arrival = message.mprocess.arrival;
                        pcb_table[x].PCBprocess.priority = message.mprocess.priority;
                        pcb_table[x].PCBprocess.remainingtime = message.mprocess.remainingtime;
                        
                        Node_to_beinserted = newNode(message.mprocess.id, 1);
                        enQueue(priority_q, Node_to_beinserted); // enqueue this process
                    }

                    //---------------------------------------------------------------------------------------------------------
                    Node_to_beinserted = newNode(current_process_id, 1);
                    enQueue(priority_q, Node_to_beinserted); //enqueuing running process again


                    running_process = peek_queue(priority_q);
                    current_process_id = running_process->process_id;
                    //printf("current_process_id = %d at time = %d\n", current_process_id, getClk());
                    if(pcb_table[current_process_id].PCBprocess.remainingtime==pcb_table[current_process_id].PCBprocess.runtime)
                    {   
                        pcb_table[current_process_id].PCBprocess.wait = getClk() - pcb_table[current_process_id].PCBprocess.arrival;
                        total_wait=total_wait+pcb_table[current_process_id].PCBprocess.wait;
                        pcb_table[current_process_id].PCBprocess.starttime=getClk();
                        fprintf(file,"At time %d process %d started arr %d total %d remain %d wait %d \n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.remainingtime,pcb_table[current_process_id].PCBprocess.wait);

                    }
                    else
                    {   pcb_table[current_process_id].PCBprocess.wait = getClk() - pcb_table[current_process_id].PCBprocess.starttime;
                        total_wait=total_wait+pcb_table[current_process_id].PCBprocess.wait;
                        pcb_table[current_process_id].PCBprocess.starttime=getClk();
                        fprintf(file,"At time %d process %d resumed arr %d total %d remain %d wait %d \n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.remainingtime,pcb_table[current_process_id].PCBprocess.wait);
                    }
                    kill(pcb_table[current_process_id].pid, SIGCONT); //continue the new process
                    old_clk = getClk();
                    deQueue(priority_q);
                    
                }

                if(is_empty(priority_q) && num_processes == 0 && current_process_id == -1)
                {
                    break;
                }

            }
            else if(atoi(argv[1])==4)
            {
                check_arrival(4);
             
               // if no process is running
                //then run the first the process that comes
                if(current_process_id == -1 && !is_empty(priority_q))
                {

                    running_process = peek_queue(priority_q);
                    current_process_id = running_process->process_id;
                    printf("current_process_id = %d\n", current_process_id);
                    
                    pcb_table[current_process_id].PCBprocess.wait = getClk() - pcb_table[current_process_id].PCBprocess.arrival;
                    total_wait=total_wait+pcb_table[current_process_id].PCBprocess.wait;
                    pcb_table[current_process_id].PCBprocess.starttime=getClk();
                    fprintf(file,"At time %d process %d started arr %d total %d remain %d wait %d \n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.remainingtime,pcb_table[current_process_id].PCBprocess.wait);
                    

                    kill(pcb_table[current_process_id].pid, SIGCONT); //continue the new process
                    old_clk = getClk();
                    deQueue(priority_q);

                    printqueue(priority_q);
                }

                //if the currentlly running process has reached the end of its quatumn
                //push it back in the queue(at the end) and run the next one
                //N.B any new process will be automatically at the end of the queue
                if(getClk() - old_clk >= quantum  && (getClk() - old_clk)%quantum == 0 && !is_empty(priority_q) && current_process_id != -1)
                {
                    //pcb_table[current_process_id].PCBprocess.wait = getClk() - pcb_table[current_process_id].PCBprocess.arrival;
                    //total_wait=total_wait+pcb_table[current_process_id].PCBprocess.wait;

                    pcb_table[current_process_id].PCBprocess.remainingtime=(pcb_table[current_process_id].PCBprocess.remainingtime-quantum);
                    fprintf(file,"At time %d process %d stopped arr %d total %d remain %d wait %d \n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.remainingtime,pcb_table[current_process_id].PCBprocess.wait);
                    kill(pcb_table[current_process_id].pid, 20);
                    
                    //--------------------------------------------------------------------------------------------------------
                    struct msgbuff message;
                    int m_pid;
                    rec_val = msgrcv(msgq_id, &message, sizeof(message.mprocess), 0, IPC_NOWAIT);
                    
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
                        kill(m_pid, 20);
                        //Adding the arrived process to the pcb table
                        int x = message.mprocess.id;
                        pcb_table[x].pid = m_pid;
                        pcb_table[x].PCBprocess.id = message.mprocess.id;
                        pcb_table[x].PCBprocess.runtime = message.mprocess.runtime;
                        pcb_table[x].PCBprocess.arrival = message.mprocess.arrival;
                        pcb_table[x].PCBprocess.priority = message.mprocess.priority;
                        pcb_table[x].PCBprocess.priorityNew = message.mprocess.priority;
                        pcb_table[x].PCBprocess.remainingtime = message.mprocess.remainingtime;
                        
                        Node_to_beinserted = newNode(message.mprocess.id,message.mprocess.priority);
                        enQueue(priority_q, Node_to_beinserted); // enqueue this process
                    }

                    //---------------------------------------------------------------------------------------------------------

                    if(pcb_table[current_process_id].PCBprocess.priorityNew>10)
                    {
                        pcb_table[current_process_id].PCBprocess.priorityNew=pcb_table[current_process_id].PCBprocess.priority;
                    }
                    else
                    {
                       pcb_table[current_process_id].PCBprocess.priorityNew++; 
                    }
                    
                    Node_to_beinserted = newNode(current_process_id,pcb_table[current_process_id].PCBprocess.priorityNew);
                    enQueue(priority_q, Node_to_beinserted); //enqueuing running process again

                    // int temp = getClk();
                    // while(getClk() - temp == 0);


                    running_process = peek_queue(priority_q);
                    current_process_id = running_process->process_id;
                    printf("current_process_id = %d\n", current_process_id);
                    if(pcb_table[current_process_id].PCBprocess.remainingtime==pcb_table[current_process_id].PCBprocess.runtime)
                    {   
                        pcb_table[current_process_id].PCBprocess.wait = getClk() - pcb_table[current_process_id].PCBprocess.arrival;
                        total_wait=total_wait+pcb_table[current_process_id].PCBprocess.wait;
                        pcb_table[current_process_id].PCBprocess.starttime=getClk();
                        fprintf(file,"At time %d process %d started arr %d total %d remain %d wait %d \n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.remainingtime,pcb_table[current_process_id].PCBprocess.wait);

                    }
                    else
                    {   pcb_table[current_process_id].PCBprocess.wait = getClk() - pcb_table[current_process_id].PCBprocess.starttime;
                        total_wait=total_wait+pcb_table[current_process_id].PCBprocess.wait;
                        pcb_table[current_process_id].PCBprocess.starttime=getClk();
                        fprintf(file,"At time %d process %d resumed arr %d total %d remain %d wait %d \n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.remainingtime,pcb_table[current_process_id].PCBprocess.wait);
                    }
                    kill(pcb_table[current_process_id].pid, SIGCONT); //continue the new process
                    old_clk = getClk();
                    deQueue(priority_q);

                }

                if(is_empty(priority_q) && num_processes == 0 && current_process_id == -1)
                {
                    break;
                }
            }
        }
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.
    fclose(file);
    FILE *fperf;
    fperf = fopen("scheduler.perf", "w");

    float utilization=((float)total_runtime/(float)getClk())*100;
    float avg_WTA=((float)total_WTA)/((float)atoi(argv[2]));
    float avg_wait=total_wait/(float)atoi(argv[2]);
    fprintf(fperf, "CPU utilization = %.2f %% \n", utilization);
    fprintf(fperf, "Avg WTA = %.2f\n", avg_WTA);
    fprintf(fperf, "Avg Waiting = %.2f\n", avg_wait);
    fclose(fperf);

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
            //printf("flag == %d\n", flag_waiting);
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
                char temp_algo[10];

                //converts id and runtime to strings instead of integers
                sprintf(temp_id, "%d", message.mprocess.id);
                sprintf(temp_runtime, "%d", message.mprocess.runtime);
                sprintf(temp_algo, "%d", choosed_algo);

                char * argv_list[] = {"./process.out", temp_id, temp_runtime, temp_algo, NULL};

                execv("./process.out",argv_list);
                exit(0);
            }
            //printf("stoping process id = %d and pid = %d\n", message.mprocess.id, m_pid);
            kill(m_pid, 20);
            //Adding the arrived process to the pcb table
            int x = message.mprocess.id;
            pcb_table[x].pid = m_pid;
            pcb_table[x].PCBprocess.id = message.mprocess.id;
            pcb_table[x].PCBprocess.runtime = message.mprocess.runtime;
            pcb_table[x].PCBprocess.arrival = message.mprocess.arrival;
            pcb_table[x].PCBprocess.priority = message.mprocess.priority;
            pcb_table[x].PCBprocess.priorityNew = message.mprocess.priority;
            pcb_table[x].PCBprocess.remainingtime = message.mprocess.remainingtime;
            pcb_table[x].PCBprocess.memsize=message.mprocess.memsize;
            printf("Process id = %d wants to allocate memory = %d \n", message.mprocess.id, message.mprocess.memsize);
            //depending on the scheduling algo. we fill the right data structure
            if(algo_num == 1)
            {
                Node_to_beinserted = newNode(pcb_table[x].PCBprocess.id, pcb_table[x].PCBprocess.runtime);
                AllocateProcessToMemory(pcb_table[x].PCBprocess.memsize,pcb_table[x].PCBprocess.id);
                enQueue(priority_q, Node_to_beinserted); // enqueue this process
            }
            else if(algo_num == 2)
            {
                total_runtime=total_runtime+message.mprocess.runtime;
                if((current_process_id != -1) && ((message.mprocess.priority)<pcb_table[current_process_id].PCBprocess.priority))
                {
                    pcb_table[current_process_id].PCBprocess.remainingtime=pcb_table[current_process_id].PCBprocess.remainingtime-(message.mprocess.arrival-pcb_table[current_process_id].PCBprocess.starttime);
                    fprintf(file,"At time %d process %d stopped arr %d total %d remain %d wait %d \n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.remainingtime,pcb_table[current_process_id].PCBprocess.wait);
                    kill(pcb_table[current_process_id].pid, 20);
                    //deQueue(priority_q);
                    Node_to_beinserted = newNode(current_process_id,pcb_table[current_process_id].PCBprocess.priority );
                    enQueue(priority_q, Node_to_beinserted); //enqueuing running process again
                    current_process_id=-1;

                }
                Node_to_beinserted = newNode(message.mprocess.id, message.mprocess.priority);
                enQueue(priority_q, Node_to_beinserted); // enqueue this process
                printqueue(priority_q);
            }
            else if(algo_num == 3)
            {
                total_runtime=total_runtime+message.mprocess.runtime;
                Node_to_beinserted = newNode(message.mprocess.id, 1);
                enQueue(priority_q, Node_to_beinserted); // enqueue this process
            }
            else if(algo_num == 4)
            {
                total_runtime=total_runtime+message.mprocess.runtime;
                Node_to_beinserted = newNode(message.mprocess.id, message.mprocess.priority);
                enQueue(priority_q, Node_to_beinserted); // enqueue this process
            }
        }
        //printf("flag waiting = %d", flag_waiting);
        flag_waiting = 0;

     }while(rec_val != -1);
}

void child_exit_handler(int signum)
{
    DeallocateProcessToMemory(current_process_id);
    int TA=getClk()-pcb_table[current_process_id].PCBprocess.arrival;
    float WTA = (float)TA / (float)pcb_table[current_process_id].PCBprocess.runtime;
    total_WTA = total_WTA + WTA;
    fprintf(file, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d  WTA %.2f\n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,0,pcb_table[current_process_id].PCBprocess.wait,TA,WTA);
    current_process_id = -1;
    signal (SIGUSR1, child_exit_handler);
}

//---------------------------------------------------MEMORY-------------------------------------------------------
void CreateMemoryQueue()
{
    for(int i = 0; i < 8; ++i)
    {
        free_memory[i] = createQueue();
        
    }
    struct pair pair_temp;
    pair_temp.first = 0;
    pair_temp.second = 1023;

    struct Node *temp = newNode_memory(0,pair_temp);
    enQueue(free_memory[7], temp);
    
    for(int i = 0; i < num_processes+1; ++i)
    {
        pcb_table[i].memorySize = -1;
    }
    printfreelist();
}

//Allocating memory for process
void AllocateProcessToMemory(int size, int id)
{
    int process_size = getBuddySize(size);
    
    if(!is_empty(free_memory[process_size]))
    {
        
        struct Node *temp = peek_queue(free_memory[process_size]);
        deQueue(free_memory[process_size]);

        printf("Process id = %d Memory from %d to %d\n", id, temp->memory_index.first, temp->memory_index.second);
        
        pcb_table[id].memorySize = temp->memory_index.second - temp->memory_index.first + 1;
        pcb_table[id].memoryStart = temp->memory_index.first;
        pcb_table[id].memoryEnd = temp->memory_index.second;
    }
    else
    {
        
        int i;
        for(i = process_size+1; i < 8; ++i)
        {
            if(!is_empty(free_memory[i]))
                break;
            
        }

        if(i == 8)
        {
            printf("Sorry, failed to allocate memory \n");
        }
        else
        {
            struct Node *temp = peek_queue(free_memory[i]);
            deQueue(free_memory[i]);
            i--;

            for(; i>= process_size; --i)
            {
                struct pair pair1;
                struct pair pair2;

                pair1.first = temp->memory_index.first;
                pair1.second = temp->memory_index.first + (temp->memory_index.second - temp->memory_index.first)/2;

                pair2.first = temp->memory_index.first + (temp->memory_index.second - temp->memory_index.first + 1)/2;
                pair2.second = temp->memory_index.second;

                struct Node *temp1 = newNode_memory(pair1.first,pair1);
                struct Node *temp2 = newNode_memory(pair2.first,pair2);
                enQueue(free_memory[i], temp1);
                enQueue(free_memory[i], temp2);

                deQueue(free_memory[i]);
            }

            printf("Process id = %d Memory from %d to %d\n", id, temp->memory_index.first, temp->memory_index.second);

            pcb_table[id].memorySize = temp->memory_index.second - temp->memory_index.first + 1;
            pcb_table[id].memoryStart = temp->memory_index.first;
            pcb_table[id].memoryEnd = temp->memory_index.second;
        }
    }
}

//Deallocating memory for process
void DeallocateProcessToMemory(int id)
{
    //checking if the process exists
    if(pcb_table[id].memorySize == -1)
    {
        printf("ERROR, cannot free this memory");
        return;
    }
    
    int n = getBuddySize(pcb_table[id].memorySize);

    struct pair pair1;
    pair1.first = pcb_table[id].memoryStart;
    pair1.second = pcb_table[id].memoryEnd;

    struct Node *temp = newNode_memory(pcb_table[id].memoryStart, pair1);

    printf("De-Allocating memory for id %d from %d to %d\n", id, pair1.first, pair1.second);

    enQueue(free_memory[n], temp);

    int buddyNumber, buddyAddress;

    buddyNumber = pcb_table[id].memoryStart / pcb_table[id].memorySize;

    if (buddyNumber % 2 != 0)
        buddyAddress = pcb_table[id].memoryStart - power(2, n+3);
    else
        buddyAddress = pcb_table[id].memoryStart + power(2, n+3);

    
    //Traversing queue to find any buddy
    printf("n = %d  buddyAddress = %d  buddyNumber = %d\n",n, buddyAddress, buddyNumber);
    struct Node* ptr = peek_queue(free_memory[n]);
    
    int counter = 0;

    while(ptr != NULL)
    {
        if (ptr->memory_index.first == buddyAddress)
        {
            
            
            if (buddyNumber % 2 == 0)
            {
                //-------------------------------------------
                struct pair pair2;
                pair2.first = pcb_table[id].memoryStart;
                pair2.second = ptr->memory_index.second;//pcb_table[id].memoryStart + 2* (power(2, n+3) - 1);

                struct Node *temp = newNode_memory(pcb_table[id].memoryStart, pair2);
                //--------------------------------------------

                enQueue(free_memory[n+1], temp);
                printf("Merging of blocks starting at %d to %d \n", pair2.first, pair2.second);
                    
            }
            else
            {
                //-------------------------------------------
                struct pair pair2;
                pair2.first = ptr->memory_index.first; 
                pair2.second = pcb_table[id].memoryEnd;//pcb_table[id].memoryStart + 2* (power(2, n+3));

                struct Node *temp = newNode_memory(pcb_table[id].memoryStart, pair2);
                //--------------------------------------------

                enQueue(free_memory[n+1], temp);
                printf("Merging of blocks starting at %d to %d \n", pair2.first, pair2.second);
            }
            
            erase_node(free_memory[n], counter);
            // erase_node(free_memory[n], counter+1);
            if (buddyNumber % 2 != 0)
                erase_node(free_memory[n], counter);
            else
                erase_node(free_memory[n], counter-1);

            break;
        }

        ptr = ptr->next;
        counter++;
    }

    pcb_table[id].memorySize = -1;
    printfreelist();
}


int getBuddySize(int size)
{
    size = size - 1;

    int clog2;

    for (clog2 = 0; size > 0; clog2 = clog2 + 1) //getting the log2 celling
        size = size >> 1;

    int buddySize = 1;

    for (int i = 0; i < clog2; i++) //2 ^ clog2
    {
        buddySize *= 2;
    }

    if(buddySize <= 8)
    return 0;
    else if (buddySize==16)
    return 1;
    else if (buddySize ==32)
    return 2;
    else if (buddySize==64)
    return 3;
    else if(buddySize==128)
    return 4;
    else if (buddySize==256)
    return 5;
    else if (buddySize == 512)
    return 6;
    else 
    return 7;
    
    //return clog2;
    //return (buddySize < 8) ? 8 : buddySize; //as our least width size is 8=2^3
}

void printfreelist()
{
    printf("8's queue: ");
    printqueue_memory(free_memory[0]);

    printf("16's queue: ");
    printqueue_memory(free_memory[1]);

    printf("32's queue: ");
    printqueue_memory(free_memory[2]);

    printf("64's queue: ");
    printqueue_memory(free_memory[3]);

    printf("128's queue: ");
    printqueue_memory(free_memory[4]);

    printf("256's queue: ");
    printqueue_memory(free_memory[5]);

    printf("512's queue: ");
    printqueue_memory(free_memory[6]);

    printf("1024's queue: ");
    printqueue_memory(free_memory[7]);
}
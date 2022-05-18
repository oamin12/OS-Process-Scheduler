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
void printMemory();
int MainMemory[1024];
void printMemory();
void deallocate(int );
void allocate(int , int );
int is_unallocated(int , int );
void Buddy();


int rec_val, msgq_id, num_processes, current_process_id, choosed_algo,old_clk, quantum;
int total_wait=0;
float total_WTA=0;
int total_runtime=0;
//memory allocation
struct Queue *free_memory[8];
int memory_allocated[1000];

struct pcb pcb_table[100]; // first Process at index 1
struct Queue *priority_q;
struct Queue *priority_q_temp;

struct Node *Node_to_beinserted;
struct Node *running_process; //for Round Robin
FILE* file;
FILE* memlog;


int main(int argc, char *argv[])
{
    Buddy();
    file = fopen("scheduler.log", "w");
    memlog= fopen("memory.log","w");
    fprintf(file, "#At time x process y state arr w total z remian y wait k \n");
    fprintf(memlog,"#At time x allocated y bytes for process z from i to j\n");
    initClk();
    signal (SIGUSR1, child_exit_handler);
    priority_q = createQueue();
    priority_q_temp = createQueue();
    //CreateMemoryQueue();
    
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
                    pcb_table[current_process_id].PCBprocess.wait = getClk() - pcb_table[current_process_id].PCBprocess.arrival;
                    total_wait=total_wait+pcb_table[current_process_id].PCBprocess.wait;
                    total_runtime=total_runtime+pcb_table[current_process_id].PCBprocess.runtime;
                    fprintf(file,"At time %d process %d started arr %d total %d remain %d wait %d \n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.wait);
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
                        pcb_table[current_process_id].PCBprocess.remainingtime=pcb_table[current_process_id].PCBprocess.runtime;
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
                        pcb_table[x].PCBprocess.remainingtime = message.mprocess.remainingtime;
                        
                        Node_to_beinserted = newNode(message.mprocess.id, 1);
                        enQueue(priority_q, Node_to_beinserted); // enqueue this process
                    }

                    //---------------------------------------------------------------------------------------------------------

                    
                    
                    Node_to_beinserted = newNode(current_process_id, 1);
                    enQueue(priority_q, Node_to_beinserted); //enqueuing running process again


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
                        pcb_table[x].PCBprocess.memsize=message.mprocess.memsize;

                        allocate( pcb_table[x].PCBprocess.id, pcb_table[x].PCBprocess.memsize);

                        Node_to_beinserted = newNode(message.mprocess.id,message.mprocess.priority);
                        enQueue(priority_q, Node_to_beinserted); // enqueue this process
                    }

                    //---------------------------------------------------------------------------------------------------------

                    if(pcb_table[current_process_id].PCBprocess.priorityNew>10)
                    {
                        pcb_table[current_process_id].PCBprocess.priorityNew=pcb_table[current_process_id].PCBprocess.priority;
                        Node_to_beinserted = newNode(current_process_id,pcb_table[current_process_id].PCBprocess.priorityNew);
                        enQueue(priority_q_temp, Node_to_beinserted); //enqueuing running process again
                    }
                    else
                    {
                       pcb_table[current_process_id].PCBprocess.priorityNew++; 
                        Node_to_beinserted = newNode(current_process_id,pcb_table[current_process_id].PCBprocess.priorityNew);
                        enQueue(priority_q, Node_to_beinserted); //enqueuing running process again
                    }
                    

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
                if(is_empty(priority_q) && !is_empty(priority_q_temp))
                {
                    while(!is_empty(priority_q_temp))
                    {
                        //roo7
                        struct Node * temp = peek_queue(priority_q_temp);
                        Node_to_beinserted = newNode(temp->process_id,pcb_table[temp->process_id].PCBprocess.priority);
                        enQueue(priority_q,Node_to_beinserted);
                        deQueue(priority_q_temp);
                    }

                }
                if(is_empty(priority_q) && is_empty(priority_q_temp) && num_processes == 0 && current_process_id == -1)
                {
                    break;
                }
            }
        }
    
    fclose(file);
    fclose(memlog);
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

    do{
        rec_val = msgrcv(msgq_id, &message, sizeof(message.mprocess), 0, IPC_NOWAIT);

        if (rec_val == -1 && is_empty(priority_q))
        {
            rec_val = msgrcv(msgq_id, &message, sizeof(message.mprocess), 0, !IPC_NOWAIT);
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
                char temp_algo[10];

                //converts id and runtime to strings instead of integers
                sprintf(temp_id, "%d", message.mprocess.id);
                sprintf(temp_runtime, "%d", message.mprocess.runtime);
                sprintf(temp_algo, "%d", choosed_algo);

                char * argv_list[] = {"./process.out", temp_id, temp_runtime, temp_algo, NULL};

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
            pcb_table[x].PCBprocess.memsize=message.mprocess.memsize;

            //depending on the scheduling algo. we fill the right data structure
            
            //Memory Allocation for received processes
            allocate( pcb_table[x].PCBprocess.id, pcb_table[x].PCBprocess.memsize);
            
            if(algo_num == 1)
            {
                Node_to_beinserted = newNode(pcb_table[x].PCBprocess.id, pcb_table[x].PCBprocess.runtime);
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
                //printqueue(priority_q);
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
        

     }while(rec_val != -1);
}

void child_exit_handler(int signum)
{
    //DeallocateProcessToMemory(current_process_id);
    int TA=getClk()-pcb_table[current_process_id].PCBprocess.arrival;
    float WTA = (float)TA / (float)pcb_table[current_process_id].PCBprocess.runtime;
    deallocate( pcb_table[current_process_id].PCBprocess.id);
    fprintf(memlog,"At time %d freed %d bytes from process %d from %d to %d\n",getClk(),pcb_table[current_process_id].PCBprocess.memsize,current_process_id,pcb_table[current_process_id].memoryStart,pcb_table[current_process_id].memoryEnd);
    total_WTA = total_WTA + WTA;
    fprintf(file, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d  WTA %.2f\n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,0,pcb_table[current_process_id].PCBprocess.wait,TA,WTA);
    current_process_id = -1;
    signal (SIGUSR1, child_exit_handler);
}

//---------------------------------------------------MEMORY-------------------------------------------------------


void Buddy()
{
    for(int i=0; i<1024; i++)
        MainMemory[i]=-1;
}

int is_unallocated(int start, int end)
{
    for(int i=start; i < end; i++)
    {
        if(MainMemory[i]!=-1)
            return 0;
    }
    return 1;
}

void allocate(int id, int size)
{
    int clog2;
    int realSize=size;
    size = size - 1;
    for (clog2 = 0; size > 0; clog2 = clog2 + 1) //getting the log2 celling
        size = size >> 1;

    int block = 1;

    for (int i = 0; i < clog2; i++) //2 ^ clog2
    {
        block *= 2;
    }
    int start=-1;
    for(int i=0; i<1024; i+=block)
    {
        if(is_unallocated(i, i+block)==1)
        {
            start = i;
            for(int j=i; j<i+block; j++)
                MainMemory[j]=id; 
            break;
        }
    }

    fprintf(memlog,"At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),realSize,id,start,start+block-1);
    pcb_table[id].memoryStart = start;
    pcb_table[id].memoryEnd=start+block-1;

}

void deallocate(int id)
{
    for(int i=0; i<1024; i++)
    {
        if(MainMemory[i]==id)
        {
            MainMemory[i]=-1;
        }
    }
}

void printMemory()
{
    for(int i=0; i<512; i++)
    {
        printf("\n@i= %d, = '%d'\n", i, MainMemory[i]);
    }
}
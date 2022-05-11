#include "headers.h"

void check_arrival (int );
void child_exit_handler(int );

int rec_val, msgq_id, num_processes, current_process_id, choosed_algo,old_clk, quantum;
int total_wait=0;
float total_WTA=0;
int total_runtime=0;

struct pcb pcb_table[100]; // first Process at index 1
struct Queue *priority_q;
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
    num_processes = atoi(argv[2]);

    choosed_algo = atoi(argv[1]);
    if(choosed_algo==3 ||choosed_algo==3)
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
                    //printf("current_process_id = %d", current_process_id);
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
                    kill(pcb_table[current_process_id].pid, SIGCONT); //continue the new process
                    old_clk = getClk();
                    deQueue(priority_q);

                    printqueue(priority_q);
                }

                //if the currentlly running process has reached the end of its quatumn
                //push it back in the queue(at the end) and run the next one
                //N.B any new process will be automatically at the end of the queue
                if(getClk() - old_clk >= quantum  && !is_empty(priority_q) && current_process_id != -1)
                {
                    kill(pcb_table[current_process_id].pid, 20);
                    Node_to_beinserted = newNode(current_process_id, 1);
                    enQueue(priority_q, Node_to_beinserted); //enqueuing running process again

                    old_clk = getClk();
                    // int temp = getClk();
                    // while(getClk() - temp == 0);


                    running_process = peek_queue(priority_q);
                    current_process_id = running_process->process_id;
                    printf("current_process_id = %d\n", current_process_id);
                    kill(pcb_table[current_process_id].pid, SIGCONT); //continue the new process

                    deQueue(priority_q);

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
        int TA=getClk()-pcb_table[current_process_id].PCBprocess.arrival;
        float WTA = (float)TA / (float)pcb_table[current_process_id].PCBprocess.runtime;
        total_WTA = total_WTA + WTA;
        fprintf(file, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d  WTA %.2f\n",getClk(),current_process_id,pcb_table[current_process_id].PCBprocess.arrival,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.runtime,pcb_table[current_process_id].PCBprocess.wait,TA,WTA);
        current_process_id = -1;
    }
    else if(choosed_algo == 2)
    {

    }
    else if(choosed_algo == 3)
    {
        // int temp = getClk();
        // while(getClk() - temp == 0);
        current_process_id = -1;
    }
}

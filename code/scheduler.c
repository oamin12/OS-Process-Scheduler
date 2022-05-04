#include "headers.h"

int main(int argc, char *argv[])
{
    initClk();

    printf("the algo is: %d \n",atoi(argv[1]));
    key_t key_id;
        int rec_val, msgq_id;

        key_id = ftok("key", 65);               //create unique key
        msgq_id = msgget(key_id, 0666 | IPC_CREAT); //create message queue and return id

        if (msgq_id == -1)
        {
            perror("Error in create");
            exit(-1);
        }
        printf("Message Queue ID = %d\n", msgq_id);

        struct msgbuff message;
        while(1)
        {
            //printf("Entered MSG REC");
            /* receive all types of messages */
            rec_val = msgrcv(msgq_id, &message, sizeof(message.mprocess), 0, !IPC_NOWAIT);

            if (rec_val == -1)
                perror("Error in receive");
            else
                printf("\nMessage received: %d\n", message.mprocess.id);
                //push to Queue/(DS be sefa 3ama) according to algo
        }
    if (atoi(argv[1])==1)
    {
        
    }    
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.

    //destroyClk(true);
}

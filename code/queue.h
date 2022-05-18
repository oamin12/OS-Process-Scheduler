// A C program to demonstrate linked list based implementation of queue
#include <stdio.h>
#include <stdlib.h>

enum STATUS
{
    RUNNING,
    WAITING,
    STOPPED,
    COUNTIUNE,
    FINSIHED
};
typedef short bool;
#define true 1
#define false 0
// A linked list (LL) node to store a queue entry
struct pair {
    int first;
    int second;
};

struct Node
{
    struct Node *next;
    int priority;
    int process_id;


    //for the output file
    int finish_time;
    int waiting_time;
    int start_time;
    int stopped_time;
    int running_time;
    int Remaining_time;

    //used for memory allocation
    struct pair memory_index;
};

// The queue, front stores the front node of LL and rear stores the
// last node of LL
struct Queue
{
    struct Node *Head;
};

// A utility function to create a new linked list node.
struct Node *newNode(int p_id, int p_priority)
{
    struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
    temp->next = NULL;
    temp->process_id = p_id;
    temp->priority = p_priority;
    return temp;
}

// A utility function to create a new linked list node.
struct Node *newNode_memory(int p_priority, struct pair index)
{
    struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
    temp->next = NULL;
    temp->priority = p_priority;
    temp->memory_index.first = index.first;
    temp->memory_index.second = index.second;
    return temp;
}

// A utility function to create an empty queue
struct Queue *createQueue()
{
    struct Queue *q = (struct Queue *)malloc(sizeof(struct Queue));
    q->Head = NULL;
    return q;
}

// The function to add a key k to q
void enQueue(struct Queue *q,  struct Node* newNode)
{
    // Create a new LL node
    struct Node *temp = newNode;

    // If queue is empty, then new node is front and rear both
    if (q->Head == NULL)
    {
        q->Head = temp;
        return;
    }

    struct Node *p = q->Head;
    if (p->priority > temp->priority)
    {

        // Insert New Node before head
        temp->next = q->Head;
        q->Head = temp;
    }
    else
    {
        // Traverse the list and find a
        // position to insert new node
        while (p->next != NULL && p->next->priority <= temp->priority)
            p = p->next;

        // Either at the ends of the list
        // or at required position
        temp->next = p->next;
        p->next = temp;
    }
}

// Function to remove a key from given queue q
void deQueue(struct Queue *q)
{
    // If queue is empty, return NULL.
    if (q->Head == NULL)
        return;

    // Store previous front and move front one node ahead
    struct Node *temp = q->Head;

    q->Head = q->Head->next;

    // If front becomes NULL, then change rear also as NULL

    free(temp);
}
struct Node* pop_queue(struct Queue* q)
{
    if (q->Head ==NULL)
        return NULL;
    struct Node *temp = q->Head;
    q->Head =q->Head->next;
    return temp;
}

void printqueue(struct Queue *q)
{
    struct Node *p = q->Head;
    while (p != NULL)
    {
        printf("process ID:%d ->", p->process_id);
        p = p->next;
    }
    printf("NULL\n");
}
bool is_empty(struct Queue *q)
{
    return (q->Head == NULL) ? true : false;
}
struct Node* peek_queue(struct Queue* q)
{
    if (q->Head ==NULL)
        return NULL;
    struct Node *temp = q->Head;
    return temp;
}

void erase_node(struct Queue *q, int num)
{
    struct Node *ptr = q->Head;

    if(ptr == NULL)
        return;

    struct Node *ptr_previous = q->Head;
    ptr = ptr->next;

    for(int i = 1; i < num; ++i)
    {
        if(ptr == NULL)
            break;

        ptr = ptr->next;
        ptr_previous = ptr_previous->next;
    }

    ptr_previous->next = ptr->next;
    free(ptr);
}


void printqueue_memory(struct Node *q)
{
    struct Node *p = q;
    while (p != NULL)
    {
        printf("Memory Location:(%d, %d) ->", p->memory_index.first, p->memory_index.second);
        p = p->next;
    }
    printf("NULL\n");
}


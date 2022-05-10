#include <stdio.h>
#include <stdlib.h>
typedef short bool;
#define true 1
#define false 0
enum STATUS
{
    RUNNING,
    WAITING
};

typedef struct node
{

    // Lower values indicate higher priority
    int priority;


    int process_id;
    int process_time;
    int remaining_time;
    int arrival_time;
    enum STATUS stat;
    struct node *next;

} Node;


// Function to Create A New Node
Node *newNode(int d, int p, int pt, int a, int r)
{
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->process_id = d;
    temp->priority = p;
    temp->next = NULL;
    temp->stat = WAITING;
    temp->remaining_time = r;
    temp->arrival_time = a;
    temp->process_time = pt;
    return temp;
}

// Return the value at head
int peek(Node **head)
{
    return (*head)->process_id;
}

// Removes the element with the
// highest priority form the list
void pop(Node **head)
{
    Node *temp = *head;
    (*head) = (*head)->next;
    free(temp);
}

// Function to push according to priority
void push(Node **head, int d, int p, int pt, int a, int r)
{
    Node *start = (*head);

    // Create new Node
    Node *temp = newNode(d, p,pt,a,r);

    // Special Case: The head of list has lesser
    // priority than new node. So insert new
    // node before head node and change head node.
    if ((*head)->priority > p)
    {

        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;
    }
    else
    {

        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL &&
               start->next->priority < p)
        {
            start = start->next;
        }

        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

// Function to check is list is empty
int isEmpty(Node **head)
{
    return (*head) == NULL;
}

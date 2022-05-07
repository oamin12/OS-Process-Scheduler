#include <stdio.h>
#include <stdlib.h>

const int MAX_PRIORITY = 999;

typedef struct
{
  int num;
  struct Qnode *next;
  int prty;

} Qnode, *QnodePtr;

typedef struct Queue
{
    QnodePtr top;
    QnodePtr tail;

} Queuetype, *Queue;


Queue initQueue()
{
    Queue newQueue = malloc(sizeof(Queuetype));
    newQueue->top = NULL;
    newQueue->tail = NULL;
    return newQueue;
}

int isEmpty(Queue q)
{return q->top == NULL;}


void pEnqueue(Queue q, int num, int prty)
{
   QnodePtr newNode = malloc(sizeof(Qnode));
   newNode->num = num;
   newNode->next = NULL;
   if(prty > MAX_PRIORITY) {prty = MAX_PRIORITY;}
   newNode->prty = prty;
   if(!isEmpty(q))
   {
	   if(newNode->prty < q->top->prty) {newNode->next = q->top; q->top = newNode;}
	   else
	   {
		   QnodePtr prevTemp = q->top;
		   QnodePtr temp = q->top->next;
		   while(temp->next != NULL && temp->prty < newNode->prty)
		   {  
			   prevTemp = temp;
		   	temp = temp->next;
	   	}
   
		   if(temp->next == NULL) {temp->next = newNode; q->tail = newNode;}
	   	else {newNode->next = temp; prevTemp->next = newNode;}
   
	   }
   }
   else
   {
	   q->top = newNode;
 	   q->tail = newNode;
   }
}

int dequeue(Queue q)
{

    if(isEmpty(q)) {printf("Queue is already empty, no nodes to remove"); return;}
    QnodePtr temp = q->top;
    int tempNum = q->top->num;
    q->top = q->top->next;
    free(temp);
    return tempNum;

}
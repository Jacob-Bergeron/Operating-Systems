#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct job{
    int id; 
    int length; 
    // other meta data here
    int responseTime;
    int turnaroundTime;
    int waitTime;
    struct job *next;
};


//* Helper Functions for Jobs

/*
createJob()
*/
struct job* createJob(int id, int length, int responseTime, int turnaroundTime, int waitTime){
    struct job* newJob = (struct job*)malloc(sizeof(struct job));
    if (newJob == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    newJob->id = id;
    newJob->length = length;
    newJob->responseTime = responseTime;
    newJob->turnaroundTime = turnaroundTime;
    newJob->waitTime = waitTime;
    newJob->next = NULL;
    return newJob;
}

/*
insertJob()
*/
void insertJob(struct job** headReference, int id, int length, int responseTime, int turnaroundTime, int waitTime){
    struct job* newJob = createJob(id, length, responseTime, turnaroundTime, waitTime);
    if(*headReference == NULL){
        *headReference = newJob; 
        return;
    }

    struct job* temp = *headReference;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newJob;
}

/*
printLinkedList()
*/
void printLinkedList(struct job* head){
    while (head != NULL) {
        printf("Job %d ran for: %d\n", head->id, head->length);
        head = head->next;
    }
}

/*
printRR()
*/
void printRR(struct job* head, int timeSlice){
    while(head != NULL){
        // run time is LESS (or equal) than timeSlice, print out the head->length 
        if(head->length <= timeSlice){
            printf("Job %d ran for: %d\n", head->id, head->length);
        }
        // else run time is over timeSlice
        else {
            printf("Job %d ran for: %d\n", head->id, timeSlice);
            int newlength = head->length - timeSlice;
            head->length = newlength;
            insertJob(&head, head->id, head->length, head->responseTime, head->turnaroundTime, head->waitTime);
        }
        head = head->next;
    }
}


//* Helper Functions for Bubble sort

/*
swap()
*/
struct job* swap(struct job* ptr1, struct job* ptr2)
{
    struct job* tmp = ptr2->next;
    ptr2->next = ptr1;
    ptr1->next = tmp;
    return ptr2;
}

/*
getSize()
*/
int getSize(struct job* head) {
    int size = 0;
    while (head != NULL) {
        size++;
        head = head->next;
    }
    return size;
}

/*
bubbleSort()
Bubble sort for linked list
*/
int bubbleSort(struct job** head, int count)
{
    struct job** h;
    int i, j, swapped;
 
    for (i = 0; i <= count; i++) {
 
        h = head;
        swapped = 0;
 
        for (j = 0; j < count - i - 1; j++) {
 
            struct job* p1 = *h;
            struct job* p2 = p1->next;
 
            if (p1->length > p2->length) {
 
                /* update the link after swapping */
                *h = swap(p1, p2);
                swapped = 1;
            }
            if(p1->length == p2->length){
                if(p1->id > p2->id){
                    *h = swap(p1, p2);
                    swapped = 1;
                }
            }
 
            h = &(*h)->next;
        }
 
        /* break if the loop ended without any swap */
        if (swapped == 0)
            break;
    }
}

//* Functions for Policy Analysis

/*
calcAvg()
*/
void calcAvg(struct job* head){

    float totalResponse = 0;
    float totalTurnAround = 0;
    float totalWait = 0;

    int size = getSize(head);
    
    while (head != NULL) {
        totalResponse += head->responseTime;
        totalTurnAround += head->turnaroundTime;
        totalWait += head->waitTime;
        head = head->next;
    }

    float avgResponse = totalResponse/size;
    float avgTurnAround = totalTurnAround/size;
    float avgWait = totalWait/size;
    printf("Average -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n", avgResponse, avgTurnAround, avgWait);
}


/*
calcAnalytics()
Passed a second argument which determines which determines which implementation to use 
based on the policy.
FIFO and SJF are the same implementation
RR is different
*/
void calcAnalytics(struct job* head, int policy){

    int timer = 0;

    while(head != NULL){
        // FIFO and SJF implementation 
        timer += head->length; 

        head->responseTime = timer - head->length;
        head->turnaroundTime = timer;
        head->waitTime = head->responseTime;
        
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", 
        head->id, head->responseTime, head->turnaroundTime, head->waitTime);
        head = head->next;

    }
}

/*
Separate function for RR
calcRR()
    response time = the time it takes for a job to start
    turnaround time = the difference in time between when a job starts and when it finishes
    turaround time = wait time + log length
    waiting time = the total time the job spends waiting instead of running.
*/
void calcRR(struct job* head, int timeSlice){
    
    // TODO: correct logic for the times

    int timer = 0;

    while(head != NULL){
        // run time is LESS (or equal) than timeSlice
        if(head->length <= timeSlice){
            timer += head->length;
            head->turnaroundTime = timer;

        }
        // else run time is over timeSlice
        else { 
            timer += timeSlice;

            int newlength = head->length - timeSlice;
            head->length = newlength;
            insertJob(&head, head->id, head->length, head->responseTime, head->turnaroundTime, head->waitTime);
        }

        head->responseTime = timer;
        head->waitTime = head->turnaroundTime - head->length;
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", 
        head->id, head->responseTime, head->turnaroundTime, head->waitTime);
        head = head->next;
    }
}



//* Scheduling Functions

/*
scheduler_FIFO()

Arguments: 
- the head job of a linked list of jobs

What it does: 
FIFO = First In First Out
- Schedules the jobs in the order of arrival
- Each job runs to completion
*/
int scheduler_FIFO(struct job* head){

    // execution
    printf("Execution trace with FIFO:\n");
    printLinkedList(head);
    printf("End of execution with FIFO.\n");

    // analysis  
    printf("Begin analyzing FIFO:\n");
    calcAnalytics(head,0);
    calcAvg(head);
    printf("End analyzing FIFO.\n");

}


/*
scheduler_SJF()

Arguments: 
- the head job of a linked list of jobs

What it does:
SJF = Shortest Job First
- Schedules the jobs by length. 
- Each job runs to completion 
*/
int scheduler_SJF(struct job* head){

    struct job **ptr = &head;
    
    // execution
    printf("Execution trace with SJF:\n");
    bubbleSort(ptr, getSize(head));
    printLinkedList(*ptr);
    printf("End of execution with SJF.\n");

    // analysis
    printf("Begin analyzing SJF:\n");
    calcAnalytics(head,0);
    calcAvg(head);
    printf("End analyzing SJF.\n");
}


/*
scheduler_RR()

Arguments:
- the head job of a linked list of jobs
- timeSlice value 

What it does:
RR = Round Robin
- Schedules jobs in order of arrival
- Each job only runs for a fixed duration (time slice)
- 
*/
int scheduler_RR(struct job* head, int timeSlice){

    // execution
    printf("Execution trace with RR:\n");
    printRR(head, timeSlice);
    printf("End of execution with RR.\n");

    //analysis
    printf("Begin analyzing RR:\n");
    calcRR(head, timeSlice);
    calcAvg(head);
    printf("End analyzing RR.\n");

}


//* MAIN Function

int main(int argc, char **argv){

    // variables for the command line args
    char* schedulingPolicy;
    char* workloadFile;
    int timeSlice;

    schedulingPolicy = argv[1];
    workloadFile = argv[2];
    timeSlice = atoi(argv[3]);

    /*
    Logic for creating linked list of jobs
    */
    // Open an input file
    FILE *fptr = fopen(workloadFile, "r");
    // Quick check to see if a valid file is passed
    if(fptr == NULL){
        perror("Error opening file\n");
        return -1;
    }

    struct job* head = NULL;

    // var to hold the scanned integers
    int input;

    int id = 0;

    // while(not end of file)
    // read input line
    while(fscanf(fptr,"%d", &input) != EOF){
        insertJob(&head, id, input, 0,0,0);
        id++;
    }
    /*
    end of linked list logic
    */

    // check what scheduling policy was given at run time
    if(strcmp(schedulingPolicy,"FIFO") == 0){
        scheduler_FIFO(head);
    }
    else if(strcmp(schedulingPolicy,"SJF") == 0){
        scheduler_SJF(head);
    }
    else if(strcmp(schedulingPolicy,"RR") == 0){
        scheduler_RR(head, timeSlice);
    }
    else{
        printf("No scheduling policy detected\n");
    }

    // Close the file
    fclose(fptr);

    // Free the memory allocated for the linked list
    struct job* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}
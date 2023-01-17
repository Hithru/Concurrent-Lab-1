#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>

unsigned bit;

int *operationsArray;
float *resultsArray;

int memberOpCount;
int insertOpCount;
int deleteOpCount;

unsigned genUniqueRandNum(unsigned short *lfsr)
{
    bit = ((*lfsr >> 0) ^ (*lfsr >> 2) ^ (*lfsr >> 3) ^ (*lfsr >> 5)) & 1;
    return *lfsr = (*lfsr >> 1) | (bit << 15);
}

struct list_node_s
{
    int data;
    struct list_node_s *next;
};

struct thread_args
{
    long numOperations;
    unsigned short threadId;
    long numThreads;
    short unsigned seed;
    struct list_node_s **head;
};

int Member(int value, struct list_node_s *head_p)
{
    struct list_node_s *curr_p = head_p;

    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;

    if (curr_p == NULL || curr_p->data > value)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int Insert(int value, struct list_node_s **head_pp)
{
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;
    struct list_node_s *temp_p;

    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }
    if (curr_p == NULL || curr_p->data > value)
    {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;
        if (pred_p == NULL)
            *head_pp = temp_p;
        else
            pred_p->next = temp_p;
        return 1;
    }
    else
    {
        return 0;
    }
}

int Delete(int value, struct list_node_s **head_pp)
{
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;

    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value)
    {
        if (pred_p == NULL)
        {
            *head_pp = curr_p->next;
            free(curr_p);
        }
        else
        {
            pred_p->next = curr_p->next;
            free(curr_p);
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

void Traverse(struct list_node_s *node)
{
    struct list_node_s *traversingNode = node;
    while (traversingNode != NULL)
    {
        printf("%d ", traversingNode->data);
        traversingNode = traversingNode->next;
    }
    printf("\n");
}

void populateLinkedList(struct list_node_s **head, int n)
{
    unsigned short lfsr = time(0);
    for (int i = 0; i < n; ++i)
    {
        int a = genUniqueRandNum(&lfsr);
        Insert(a, head);
    }
}

void createArray(int numOperations)
{
    operationsArray = malloc(sizeof(int) * numOperations);
    for (int i = 0; i < memberOpCount; i++)
    {
        operationsArray[i] = 0;
    }
    for (int i = memberOpCount; i < memberOpCount + insertOpCount; i++)
    {
        operationsArray[i] = 1;
    }
    for (int i = memberOpCount + insertOpCount; i < memberOpCount + insertOpCount + deleteOpCount; i++)
    {
        operationsArray[i] = 2;
    }
}

void shuffleArray(int n)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand48(usec);
    if (n > 1)
    {
        int i;
        for (i = n - 1; i > 0; i--)
        {
            size_t j = (unsigned int)(drand48() * (i + 1));
            int t = operationsArray[j];
            operationsArray[j] = operationsArray[i];
            operationsArray[i] = t;
        }
    }
}

void assignOperationCounts(int numOperations, float probMember, float probInsert, float probDelete)
{
    memberOpCount = round(numOperations * probMember);
    insertOpCount = round(numOperations * probInsert);
    deleteOpCount = round(numOperations * probDelete);
}

double runSerialProgram(struct list_node_s **header, int numOperations)
{
    struct list_node_s **head = header;

    clock_t start, end;
    double cpu_time_used;
    start = clock();

    for (int i = 0; i < numOperations; ++i)
    {
        unsigned short lfsr = rand();
        int randNum = genUniqueRandNum(&lfsr);
        if (operationsArray[i] == 0)
        {
            Member(randNum, *head);
        }
        else if (operationsArray[i] == 1)
        {
            Insert(randNum, head);
        }
        else if (operationsArray[i] == 2)
        {
            Delete(randNum, head);
        }
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    return cpu_time_used;
}

int main()
{
    int numKeys = 1000;
    long numOperations = 10000;
    int caseNum;
    int n;

    float probMember;
    float probInsert;
    float probDelete;

    printf("Enter case number : ");
    scanf("%d", &caseNum);

    printf("Enter test run count: ");
    scanf("%d", &n);

    if (caseNum == 1)
    {
        probMember = 0.99;
        probInsert = 0.005;
        probDelete = 0.005;
    }
    else if (caseNum == 2)
    {
        probMember = 0.90;
        probInsert = 0.05;
        probDelete = 0.05;
    }

    else if (caseNum == 3)
    {
        probMember = 0.50;
        probInsert = 0.25;
        probDelete = 0.25;
    }
    assignOperationCounts(numOperations, probMember, probInsert, probDelete);
    resultsArray = malloc(sizeof(int) * n);

    for (int r = 0; r < n; r++)
    {
        createArray(numOperations);
        shuffleArray(numOperations);
        struct list_node_s *head = NULL;
        populateLinkedList(&head, numKeys);
        resultsArray[r] = runSerialProgram(&head, numOperations);
    }

    double sum;
    for (int p = 0; p < n; p++)
    {
        sum = sum + resultsArray[p];
    }
    printf("Mean is %f\n", sum / n);
    double mean = sum / n;

    double sd = 0;
    for (int q = 0; q < n; q++)
        sd += pow(resultsArray[q] - mean, 2);
    sd = sqrt(sd / n);
    printf("Standard Deviation is %f\n", sd);
    return 0;
}
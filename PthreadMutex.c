#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>

unsigned bit;
pthread_mutex_t lock;

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

void *mutexProgram(void *ptr)
{
    struct thread_args *args = (struct thread_args *)ptr;
    long numOperations = args->numOperations;
    unsigned short threadId = args->threadId;
    long numThreads = args->numThreads;
    short unsigned seed = args->seed;
    struct list_node_s **head = args->head;

    for (int i = threadId; i < numOperations; i = i + numThreads)
    {
        int randNum = genUniqueRandNum(&seed);
        if (operationsArray[i] == 0)
        {
            pthread_mutex_lock(&lock);
            Member(randNum, *head);
            pthread_mutex_unlock(&lock);
        }
        else if (operationsArray[i] == 1)
        {
            pthread_mutex_lock(&lock);
            Insert(randNum, head);
            pthread_mutex_unlock(&lock);
        }
        else if (operationsArray[i] == 2)
        {
            pthread_mutex_lock(&lock);
            Delete(randNum, head);
            pthread_mutex_unlock(&lock);
        }
    }
    return EXIT_SUCCESS;
}

double runMutexProgram(struct list_node_s **header, int numOperations, long numThreads)
{
    struct list_node_s **head = header;
    pthread_t *threadHandles;

    pthread_mutex_init(&lock, NULL);

    threadHandles = malloc(numThreads * sizeof(pthread_t));
    unsigned short thread;

    clock_t start, end;
    double cpu_time_used;
    start = clock();

    for (thread = 0; thread < numThreads; ++thread)
    {
        struct thread_args *thread_args_struct = malloc(sizeof(struct thread_args));
        thread_args_struct->numOperations = numOperations;
        thread_args_struct->head = head;
        thread_args_struct->threadId = thread;
        thread_args_struct->numThreads = numThreads;
        thread_args_struct->seed = thread + 1000;
        pthread_create(&threadHandles[thread], NULL, mutexProgram, (void *)thread_args_struct);
    }
    int t;
    for (t = 0; t < numThreads; ++t)
    {
        pthread_join(threadHandles[t], NULL);
    }
    free(threadHandles);
    pthread_mutex_destroy(&lock);

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
    long numThreads;

    float probMember;
    float probInsert;
    float probDelete;

    printf("Enter case number : ");
    scanf("%d", &caseNum);

    printf("Enter test run count: ");
    scanf("%d", &n);

    printf("Enter number of threads: ");
    scanf("%ld", &numThreads);

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
        resultsArray[r] = runMutexProgram(&head, numOperations, numThreads);
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
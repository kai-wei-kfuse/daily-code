#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_JOBS 20
#define MAX_JOBS_QUEUE 5
#define MAX_RESOURCES 3

typedef struct {
    int A;
    int B;
    int C;
} SystemResources;

typedef struct {
    int id;                     // ��ҵ���
    int arrival_time;           // ����ʱ��
    int execution_time;         // ִ��ʱ��
    int menory_size;            // �����ڴ��С
    SystemResources resources;  // �������ϵͳ��Դ
} Job;

typedef struct Process {
    int id;                             // ���̱��
    int arrivalTime;                    // ����ʱ��
    int serviceTime;                    // ����ʱ��
    int leftServiceTime;                // ʣ�����ʱ��
    int startTime;                      // ��ʼʱ��
    int endTime;                        // ����ʱ��
    int turnaroundTime;                 // ��תʱ��
    float weightedTurnaroundTime;       // ��Ȩ��תʱ��
    int memorySize;                     // �����ڴ��С
    SystemResources resources;          // ����ϵͳ��Դ
    char status;                        // ״̬
    int Allocation[MAX_RESOURCES + 1];  // �ѷ������Դ��
    int Need[MAX_RESOURCES + 1];        // ����Ҫ����Դ��
    struct Process* next;
} Process;

typedef struct MemoryBlock {
    int size;
    int startAddress;
    int processId;
    struct MemoryBlock* next;
} MemoryBlock;

Process* readyQueue = NULL;      // ��������
Process* runningQueue = NULL;    // ���ж���
Process* finishedQueue = NULL;   // ����ɶ���
Process* blockQueue = NULL;      // ��������
MemoryBlock* memoryList = NULL;  // �ڴ������
int nowTime = 0;

SystemResources resources;

int Request[3];         // ������Դ��
int Work[3];            // ϵͳ�ɷ�����Դ��
bool Finish[5];         // �������
int Security_queue[5];  // ��ȫ��������
bool flag = true;       // ����Ƿ�Ϊ��ȫ״̬
// ��������

// ��������ҵ����
void createBackupJobQueue();
// ��ʼ��ϵͳ��Դ
void initSystemResources(SystemResources* resources);
// ��ʼ���ڴ����
MemoryBlock* createMemoryBlock(int size);
// ��ʼ���ڴ����
void initMemoryManagement(MemoryBlock** memoryList);
// ��ʾ�ڴ������
void displayMemoryBlocks(MemoryBlock* memoryList);
// �����ڴ����
void cleanupMemoryManagement(MemoryBlock* memoryList);
// ��������
Process* createProcess(int id, int arrivalTime, int serviceTime);
// ��ʼ�����̵���
void initProcessScheduling(Process** readyQueue, Process** runningQueue);
// ��ʾ���̶���
void displayProcessQueue(const char* queueName, Process* queue);
// ������̶���
void cleanupProcessQueue(Process* queue);
// ��ʼ������ϵͳ
void initos();
// �����̼����������
void addToReadyQueue(Process** readyQueue, Process* Process);
// ����г���
int length(Process* queue);
// �Ӻ���ҵ�����ļ����Ƴ���ѡ�����ҵ
void removeJobFromBackupJobQueue(int id);
// �����ڴ�(��̬���������㷨���״���Ӧ�㷨)
void initJobScheduling(Process** readyQueue);
// �Ӻ���ҵ������ѡ��һ������ҵ����������У����5����ҵ
void allocationAllocationMemory(Process* Process, MemoryBlock** memoryList);
// �Ӿ���������ѡ��һ����ҵ�������ж��У�����ҵ�����㷨,��ռʽ��
void selectJobToRun(Process** readyQueue, Process** runningQueue);
// ������ж����е���ҵ�Ƿ������
void checkJobFinished(Process** runningQueue);
// �ͷ��ڴ�
void freeMemory(Process* Process, MemoryBlock** memoryList);
// ������������ѡ��һ����ҵ�����������
void selectJobToReady(Process** blockQueue, Process** readyQueue);
// �Ƿ����㹻��ϵͳ��Դ
bool hasEnoughResources(Process* Process, SystemResources* resources);
// ��̽�Է�����Դ
void soundout(Process* runningQueue);
// ����ȫ״̬
void non_soundout(Process* runningQueue);
// ��ȫ���㷨
bool safe(Process* runningQueue);
// ���м��㷨
bool banker(Process* runningQueue);
// ����������Դ
void requestResources(Process* runningQueue);
// �ͷ���Դ
void releaseResources(Process* runningQueue);

void createBackupJobQueue() {
    FILE* file = fopen("backup_job_queue.txt", "w");
    if (file == NULL) {
        printf("�޷����ļ���\n");
        return;
    } else {
        printf("�ļ��Ѵ򿪡�\n");
    }

    Job backup_job_queue[MAX_JOBS];
    srand(time(NULL));

    // ������ҵ����
    for (int i = 0; i < MAX_JOBS; i++) {
        backup_job_queue[i].id = i + 1;
        backup_job_queue[i].arrival_time = rand() % 10;  // ���������ҵ����ʱ��
        backup_job_queue[i].execution_time =
            rand() % 10 + 1;  // ���������ҵִ��ʱ��
        backup_job_queue[i].menory_size =
            rand() % 100 + 1;  // ���������ҵ�����ڴ��С
        backup_job_queue[i].resources.A =
            rand() % 5 + 1;  // ���������ҵ����ϵͳ��ԴA
        backup_job_queue[i].resources.B =
            rand() % 5 + 1;  // ���������ҵ����ϵͳ��ԴB
        backup_job_queue[i].resources.C =
            rand() % 5 + 1;  // ���������ҵ����ϵͳ��ԴC
    }
    // ������ʱ������
    for (int i = 0; i < MAX_JOBS - 1; i++) {
        for (int j = 0; j < MAX_JOBS - 1 - i; j++) {
            if (backup_job_queue[j].arrival_time >
                backup_job_queue[j + 1].arrival_time) {
                Job temp = backup_job_queue[j];
                backup_job_queue[j] = backup_job_queue[j + 1];
                backup_job_queue[j + 1] = temp;
            }
        }
    }
    for (int i = 0; i < MAX_JOBS; i++) {
        fprintf(
            file, "%d,%d,%d,%d,%d,%d,%d\n", backup_job_queue[i].id,
            backup_job_queue[i].arrival_time,
            backup_job_queue[i].execution_time, backup_job_queue[i].menory_size,
            backup_job_queue[i].resources.A, backup_job_queue[i].resources.B,
            backup_job_queue[i].resources.C);
    }
    fclose(file);
    printf("����ҵ�����Ѵ��������浽�����ļ���\n");
}
// ���������Ϣ
void displayProcess(Process* process) {
    printf("������Ϣ:");
    printf("[id:%d,����ʱ�䣺%d,����ʱ��:%d,ʣ�����ʱ��:%d,�ڴ��С��%d] ",
           process->id, process->arrivalTime, process->serviceTime,
           process->leftServiceTime, process->memorySize);
    printf("�������������Դ:[%d,%d,%d] ", process->resources.A,
           process->resources.B, process->resources.C);
    printf("�����ѷ�����Դ:[%d,%d,%d] ", process->Allocation[0],
           process->Allocation[1], process->Allocation[2]);
    printf("���̻�����Դ:[%d,%d,%d]\n", process->Need[0], process->Need[1],
           process->Need[2]);
}
// ��ʼ��ϵͳ��Դ

void initSystemResources(SystemResources* resources) {
    resources->A = 20;
    resources->B = 8;
    resources->C = 12;

    printf("ϵͳ��Դ�ѳ�ʼ����\n");
    printf("A: %d\n", resources->A);
    printf("B: %d\n", resources->B);
    printf("C: %d\n", resources->C);
}

// �����Դʣ����
void displaySystemResources(SystemResources* resources) {
    printf("ϵͳ��Դʣ����:\n");
    printf("A: %d\n", resources->A);
    printf("B: %d\n", resources->B);
    printf("C: %d\n", resources->C);
}

MemoryBlock* createMemoryBlock(int size) {
    MemoryBlock* block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    block->size = size;
    block->next = NULL;
    block->processId = -1;
    block->startAddress = 0;
    return block;
}

void initMemoryManagement(MemoryBlock** memoryList) {
    *memoryList = createMemoryBlock(1024);  // ����һ���СΪ1024K���ڴ��
}

void displayMemoryBlocks(MemoryBlock* memoryList) {
    printf("\n��ʱ���ڴ������:\n");
    printf("%-15s %-15s %-15s %-15s\n", "��ʼ��ַ", "������ַ", "����ID",
           "״̬");

    while (memoryList != NULL) {
        int endAddress = memoryList->startAddress + memoryList->size - 1;
        if (memoryList->processId != -1) {
            printf("%-15d %-15d %-15d %-15s\n", memoryList->startAddress,
                   endAddress, memoryList->processId, "�ѷ���");
        } else {
            printf("%-15d %-15d %-15s %-15s\n", memoryList->startAddress,
                   endAddress, "-", "δ����");
        }

        memoryList = memoryList->next;
    }
}

void cleanupMemoryManagement(MemoryBlock* memoryList) {
    while (memoryList != NULL) {
        MemoryBlock* current = memoryList;
        memoryList = memoryList->next;
        free(current);
    }
}

Process* createProcess(int id,
                       int arrivalTime,
                       int serviceTime,
                       int memorySize,
                       SystemResources resources) {
    Process* process = (Process*)malloc(sizeof(Process));
    process->id = id;
    process->arrivalTime = arrivalTime;
    process->serviceTime = serviceTime;
    process->startTime = nowTime;
    process->leftServiceTime = serviceTime;
    process->endTime = nowTime + serviceTime;
    process->turnaroundTime = nowTime + serviceTime;
    process->weightedTurnaroundTime =
        (float)(nowTime + serviceTime) / (float)serviceTime;  // ��Ȩ��תʱ��
    process->memorySize = memorySize;
    process->resources.A = resources.A;
    process->resources.B = resources.B;
    process->resources.C = resources.C;
    process->Allocation[0] = 0;
    process->Allocation[1] = 0;
    process->Allocation[2] = 0;
    process->Need[0] = resources.A;
    process->Need[1] = resources.B;
    process->Need[2] = resources.C;
    process->next = NULL;
    return process;
}

void initProcessScheduling(Process** readyQueue, Process** runningQueue) {
    *readyQueue = NULL;
    *runningQueue = NULL;
}

void displayProcessQueue(const char* queueName, Process* queue) {
    printf("%s����: \n", queueName);
    while (queue != NULL) {
        displayProcess(queue);
        queue = queue->next;
        printf("?");
    }
    printf("NULL\n");
}

void cleanupProcessQueue(Process* queue) {
    while (queue != NULL) {
        Process* current = queue;
        queue = queue->next;
        free(current);
    }
}

void initos() {
    // ��������ҵ����
    createBackupJobQueue();
    // ��ʼ��ϵͳ��Դ

    initSystemResources(&resources);
    // ��ʼ���ڴ����
    memoryList = NULL;

    initMemoryManagement(&memoryList);
    displayMemoryBlocks(memoryList);

    // cleanupMemoryManagement(memoryList);
    readyQueue = NULL;
    runningQueue = NULL;

    initProcessScheduling(&readyQueue, &runningQueue);
    displayProcessQueue("����", readyQueue);
    displayProcessQueue("����", runningQueue);
    displayProcessQueue("����", blockQueue);
    displayProcessQueue("�����", finishedQueue);

    // cleanupProcessQueue(readyQueue);
    // cleanupProcessQueue(runningQueue);
    printf("����ϵͳ�ѳ�ʼ��!\n");
    printf("�����������...\n");
    getchar();
}
// �����̼����������
void addToReadyQueue(Process** readyQueue, Process* process) {
    if (*readyQueue == NULL) {
        *readyQueue = process;
    } else {
        Process* p = *readyQueue;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = process;
    }
    printf("��ҵ%d�Ѽ����������:\n", process->id);
    displayProcess(process);
}
// ���̼�����������
void addToBlockQueue(Process** blockQueue, Process* process) {
    if (*blockQueue == NULL) {
        *blockQueue = process;
    } else {
        Process* p = *blockQueue;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = process;
    }
    printf("hh");
    printf("��ҵ%d�Ѽ�����������:\n", process->id);
    displayProcess(process);
}

// ����г���

int length(Process* queue) {
    int len = 0;
    while (queue != NULL) {
        len++;
        queue = queue->next;
    }
    return len;
}
// �Ӻ���ҵ�����ļ����Ƴ���ѡ�����ҵ
void removeJobFromBackupJobQueue(int id) {
    FILE* file = fopen("backup_job_queue.txt", "r");
    if (file == NULL) {
        printf("�޷��򿪺���ҵ�����ļ���\n");
        return;
    }

    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("�޷�������ʱ�ļ���\n");
        fclose(file);
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        int jobId;
        int arrivalTime;
        int serviceTime;
        int memorySize;
        SystemResources resources;
        if (sscanf(line, "%d,%d,%d,%d,%d,%d,%d", &jobId, &arrivalTime,
                   &serviceTime, &memorySize, &resources.A, &resources.B,
                   &resources.C) == 7) {
            if (jobId != id) {
                fprintf(tempFile, "%d,%d,%d,%d,%d,%d,%d\n", jobId, arrivalTime,
                        serviceTime, memorySize, resources.A, resources.B,
                        resources.C);
            }
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("backup_job_queue.txt");              // ɾ��ԭ�ļ�
    rename("temp.txt", "backup_job_queue.txt");  // �滻ԭ�ļ�
}

// �����ڴ�(��̬���������㷨���״���Ӧ�㷨)
void allocationAllocationMemory(Process* Process, MemoryBlock** memoryList) {
    MemoryBlock* block = *memoryList;
    MemoryBlock* prev = NULL;

    while (block != NULL) {
        if (block->processId == -1 && block->size >= Process->memorySize) {
            // ����ڴ��δ�����Ҵ�С������ҵ�����ڴ��С
            if (block->size == Process->memorySize) {
                // ����ڴ���С����ҵ�����ڴ��С��ȣ�����ҵ���䵽���ڴ��
                block->processId = Process->id;
                block->size = Process->memorySize;
                Process->startTime = nowTime;
                Process->endTime = nowTime + Process->serviceTime;
                Process->turnaroundTime =
                    Process->endTime - Process->arrivalTime;
                Process->weightedTurnaroundTime =
                    (float)Process->turnaroundTime /
                    (float)Process->serviceTime;
                return;
            } else {
                // ����ڴ���С������ҵ�����ڴ��С������ҵ���䵽���ڴ�飬����Ҫ�����ڴ��ָ�������ڴ��
                MemoryBlock* newBlock = createMemoryBlock(Process->memorySize);
                newBlock->processId = Process->id;
                newBlock->startAddress = block->startAddress;
                block->size -= Process->memorySize;
                block->startAddress += Process->memorySize;
                // ���µ��ڴ����뵽������
                newBlock->next = block;
                if (prev == NULL) {
                    *memoryList = newBlock;
                } else {
                    prev->next = newBlock;
                }
                Process->startTime = nowTime;
                Process->endTime = nowTime + Process->serviceTime;
                Process->turnaroundTime =
                    Process->endTime - Process->arrivalTime;
                Process->weightedTurnaroundTime =
                    (float)Process->turnaroundTime /
                    (float)Process->serviceTime;
                return;
            }
        }
        prev = block;
        block = block->next;
    }

    printf("�޷�Ϊ��ҵ %d �����ڴ档\n", Process->id);
}

// �Ӻ���ҵ������ѡ��һ������ҵ����������У����5����ҵ
void initJobScheduling(Process** readyQueue) {
    // �Ӻ���ҵ������ѡ��һ������ҵ����������У����5����ҵ
    FILE* file = fopen("backup_job_queue.txt", "r");
    if (file == NULL) {
        printf("�޷��򿪺���ҵ�����ļ���\n");
        return;
    } else {
        printf("�ļ��Ѵ򿪡�\n");
    }

    int count = length(*readyQueue);
    char line[100];
    while (count < MAX_JOBS_QUEUE && fgets(line, sizeof(line), file)) {
        int id;
        int arrivalTime;
        int serviceTime;
        int memorySize;
        SystemResources resources;
        if (sscanf(line, "%d,%d,%d,%d,%d,%d,%d", &id, &arrivalTime,
                   &serviceTime, &memorySize, &resources.A, &resources.B,
                   &resources.C) == 7) {
            if (arrivalTime > nowTime) {
                break;
            }
            Process* Process = createProcess(id, arrivalTime, serviceTime,
                                             memorySize, resources);
            displayProcess(Process);
            addToReadyQueue(readyQueue, Process);
            allocationAllocationMemory(Process, &memoryList);
            displayMemoryBlocks(memoryList);
            count++;
        }
    }

    fclose(file);
    // ���Ѿ�����������е���ҵ�Ӻ���ҵ�������Ƴ�
    Process** p = readyQueue;
    for (int i = 0; i < count; i++) {
        removeJobFromBackupJobQueue((*p)->id);
        p = &((*p)->next);
    }
    printf("����ҵ�����Ѹ��¡�\n");
}

// �Ӿ���������ѡ��һ����ҵ�������ж��У�����ҵ�����㷨,��ռʽ��
void selectJobToRun(Process** readyQueue, Process** runningQueue) {
    if (*readyQueue == NULL) {
        printf("��������Ϊ�ա�\n");
        return;
    }

    Process* prev = NULL;
    Process* prev2 = NULL;
    Process* p = *readyQueue;
    Process* selectedProcess = p;
    while (p != NULL) {
        if (p->serviceTime < selectedProcess->serviceTime) {
            selectedProcess = p;
            prev2 = prev;
        }
        prev = p;
        p = p->next;
    }
    printf("��ʱ�������ҵ:\n");
    displayProcess(selectedProcess);
    // ������ж��е���ҵ�ķ���ʱ����ھ��������е���ҵ�ķ���ʱ�䣬������ж����е���ҵ�����������
    if (*runningQueue != NULL &&
        (*runningQueue)->serviceTime > selectedProcess->serviceTime) {
        addToReadyQueue(readyQueue, *runningQueue);
        *runningQueue = NULL;
        printf("ʵ����ռ\n");
    }
    if (*runningQueue == NULL) {
        if (prev2 == NULL) {  // �����������ֻ��һ����ҵ
            *readyQueue = selectedProcess->next;
        } else {
            prev2->next = selectedProcess->next;
        }
        selectedProcess->next = NULL;
        *runningQueue = selectedProcess;
        printf("��ҵ%d�Ѽ������ж���:\n", selectedProcess->id);
        displayProcess(selectedProcess);
    } else {
        printf("��ʵ����ռ\n");
    }
}

// �ͷ��ڴ�
void freeMemory(Process* Process, MemoryBlock** memoryList) {
    MemoryBlock* block = *memoryList;
    MemoryBlock* prev = NULL;
    while (block != NULL) {
        if (block->processId == Process->id) {
            block->processId = -1;
            if (prev != NULL && prev->processId == -1) {
                prev->size += block->size;
                prev->next = block->next;
                free(block);
                block = prev;
            }
            if (block->next != NULL && block->next->processId == -1) {
                block->size += block->next->size;
                MemoryBlock* temp = block->next;
                block->next = block->next->next;
                free(temp);
            }
        }
        prev = block;
        block = block->next;
    }
}

// �Ƿ����㹻��ϵͳ��Դ
bool hasEnoughResources(Process* process, SystemResources* resources) {
    if (process->resources.A > resources->A) {
        return false;
    }
    if (process->resources.B > resources->B) {
        return false;
    }
    if (process->resources.C > resources->C) {
        return false;
    }
    return true;
}

// ������������ѡ��һ����ҵ�����������
void selectJobToReady(Process** blockQueue, Process** readyQueue) {
    if (*blockQueue == NULL) {
        printf("��������Ϊ�ա�\n");
        return;
    }
    Process* prev = NULL;
    Process* prev2 = NULL;
    Process* p = *blockQueue;
    Process* selectedProcess = p;
    while (p != NULL) {
        if (p->serviceTime < selectedProcess->serviceTime) {
            selectedProcess = p;
            prev2 = prev;
        }
        prev = p;
        p = p->next;
    }
    printf("��ʱ�������ҵ:\n[id:%d,����ʱ�䣺%d,����ʱ��%d,�ڴ��С��%d]\n",
           selectedProcess->id, selectedProcess->arrivalTime,
           selectedProcess->serviceTime, selectedProcess->memorySize);
    if (prev2 == NULL) {  // �����������ֻ��һ����ҵ
        *blockQueue = selectedProcess->next;
    } else {
        prev2->next = selectedProcess->next;
    }
    selectedProcess->next = NULL;
    addToReadyQueue(readyQueue, selectedProcess);
    printf("��ҵ%d�Ѽ����������:\n ", selectedProcess->id);
    displayProcess(selectedProcess);
}

// ��̽�Է�����Դ
void soundout(Process* runningQueue) {  // ��ȫ״̬
    Process* i = runningQueue;
    resources.A = resources.A - Request[0];
    resources.B = resources.B - Request[1];
    resources.C = resources.C - Request[2];
    for (int j = 0; j < MAX_RESOURCES; j++) {
        i->Allocation[j] = i->Allocation[j] + Request[j];
        i->Need[j] = i->Need[j] - Request[j];
    }
}

void non_soundout(Process* runningQueue) {  // ����ȫ״̬
    Process* i = runningQueue;
    resources.A = resources.A + Request[0];
    resources.B = resources.B + Request[1];
    resources.C = resources.C + Request[2];
    for (int j = 0; j < MAX_RESOURCES; j++) {
        i->Allocation[j] = i->Allocation[j] - Request[j];
        i->Need[j] = i->Need[j] + Request[j];
    }
}
bool safe(Process* runningQueue) {  // ��ȫ���㷨
    // ͳ�ƽ�����
    if (hasEnoughResources(runningQueue, &resources)) {
        printf("ϵͳ��Դ�㹻\n");
        return true;
    }
    int m = 0;
    Process* w = runningQueue;
    while (w != NULL) {
        w = w->next;
        m++;
    }
    // ��ʼ��Work����
    Work[0] = resources.A;
    Work[1] = resources.B;
    Work[2] = resources.C;
    // ��ʼ��Finish����
    for (int i = 0; i < MAX_JOBS_QUEUE; i++) {
        Finish[i] = false;
    }
    // �ڵ�λ�ã��жϽ�������
    int pnum = 0;
    int apply;
    int num = 0;
    // ��ȫ����
    Process* p = runningQueue;
    while (p != NULL && pnum <= m) {
        apply = 1;
        for (int i = 0; i < MAX_RESOURCES; i++) {
            // ��ǰ����δ��ɲ��Ҹ�����ԴС�ڻ����ϵͳ��Դ��
            if (Finish[pnum] == false && p->Need[i] <= Work[i]) {
                apply++;
                // ֱ��ÿ����Դ��������С��ϵͳ��������Դ���ſɷ���
                if (apply == MAX_RESOURCES) {
                    for (int x = 0; x < MAX_RESOURCES; x++) {
                        // ���µ�ǰϵͳ�ɷ�����Դ
                        Work[x] += p->Allocation[x];
                    }
                    // ���±������
                    Finish[pnum] = true;
                    // ���°�ȫ��������
                    Security_queue[num++] = p->id;
                    // ��֤ÿ�β�ѯ���ӵ�һ�����̿�ʼ
                    pnum = 0;
                    p = runningQueue;
                }
            }
        }
        // ��ѯ��һ����
        pnum++;
        p = p->next;
    }
    for (int i = 0; i < m; i++) {
        if (Finish[i] == false) {
            // ϵͳ���ڲ���ȫ״̬
            return false;
        }
    }
    return true;
}
bool banker(Process* runningQueue) {
    Process* p = runningQueue;
    int n = 0;
    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (Request[i] == 0) {
            n++;
        }
    }
    if (n == MAX_RESOURCES) {
        flag = false;
        return false;
    }
    int num = 1;
    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (Request[i] > p->Need[i]) {
            return false;
        } else {
            if (Request[i] > resources.A || Request[i] > resources.B ||
                Request[i] > resources.C) {
                return false;
            }
        }
    }
    soundout(runningQueue);
    if (!safe(runningQueue)) {
        non_soundout(runningQueue);
        return false;
    }
    return true;
}
// ����������Դ
void requestResources(Process* runningQueue) {
    if (runningQueue == NULL) {
        printf("���ж���Ϊ�ա�\n");
        return;
    }

    // ������ɽ����������Դ��
    Process* p = runningQueue;
    srand(time(NULL));
    SystemResources Need;
    Need.A = p->resources.A - p->Allocation[0];
    Need.B = p->resources.B - p->Allocation[1];
    Need.C = p->resources.C - p->Allocation[2];
    if (Need.A == 0 && Need.B == 0 && Need.C == 0) {
        printf("��ҵ%d�����������Դ\n", p->id);
        // releaseResources(runningQueue);
        // printf("��ҵ%d��Դʹ�ò��ͷ����\n", p->id);
        return;
    }
    Request[0] = rand() % (Need.A + 1);
    Request[1] = rand() % (Need.B + 1);
    Request[2] = rand() % (Need.C + 1);
    printf("��ҵ%d������Դ��:\n", p->id);
    printf("A: %d\n", Request[0]);
    printf("B: %d\n", Request[1]);
    printf("C: %d\n", Request[2]);
    printf("��ҵ%d������Դ��:\n", p->id);
    printf("A: %d\n", p->Need[0]);
    printf("B: %d\n", p->Need[1]);
    printf("C: %d\n", p->Need[2]);
    if (banker(runningQueue) || safe(runningQueue)) {
        printf("��ҵ%d������Դ�ɹ�\n", p->id);
        displayProcess(p);
    } else {
        printf("��ҵ%d������Դʧ��\n", p->id);
        displayProcess(p);
        addToBlockQueue(&blockQueue, p);
        runningQueue = NULL;
    }
}
// �ͷ���Դ
void releaseResources(Process* runningQueue) {
    Process* p = runningQueue;
    resources.A += p->Allocation[0];
    resources.B += p->Allocation[1];
    resources.C += p->Allocation[2];
    printf("��ҵ%d���ͷ���Դ\n", p->id);
    displayProcess(p);
}

// ������ж����е���ҵ�Ƿ������
void checkJobFinished(Process** runningQueue) {
    if (*runningQueue != NULL) {
        if ((*runningQueue)->leftServiceTime == 0) {
            printf("��ҵ%d�����\n", (*runningQueue)->id);
            // �ͷ��ڴ�
            freeMemory(*runningQueue, &memoryList);
            (*runningQueue)->endTime = nowTime;
            (*runningQueue)->turnaroundTime =
                (*runningQueue)->endTime - (*runningQueue)->arrivalTime;
            (*runningQueue)->weightedTurnaroundTime =
                (float)(*runningQueue)->turnaroundTime /
                (float)(*runningQueue)->serviceTime;
            Process* process = *runningQueue;
            // �ͷ���Դ
            releaseResources(*runningQueue);
            // ������ɵ���ҵ��������ɶ���
            if (finishedQueue == NULL) {
                finishedQueue = process;
            } else {
                Process* p = finishedQueue;
                while (p->next != NULL) {
                    p = p->next;
                }
                p->next = process;
            }
            *runningQueue = NULL;
            // �������������е���ҵ
            if (blockQueue != NULL) {
                selectJobToReady(&blockQueue, &readyQueue);
            }
        }
    }
}
// �����תʱ��ʹ�Ȩ��תʱ�䣬����ƽ����תʱ���ƽ����Ȩ��תʱ��
void displayTurnaroundTime(Process* finishedQueue) {
    int sumTurnaroundTime = 0;
    float sumWeightedTurnaroundTime = 0;
    int count = 0;
    while (finishedQueue != NULL) {
        printf("��ҵ%d����תʱ��Ϊ:%d\n", finishedQueue->id,
               finishedQueue->turnaroundTime);
        printf("��ҵ%d�Ĵ�Ȩ��תʱ��Ϊ:%f\n", finishedQueue->id,
               finishedQueue->weightedTurnaroundTime);
        sumTurnaroundTime += finishedQueue->turnaroundTime;
        sumWeightedTurnaroundTime += finishedQueue->weightedTurnaroundTime;
        count++;
        finishedQueue = finishedQueue->next;
    }
    printf("ƽ����תʱ��Ϊ:%f\n", (float)sumTurnaroundTime / (float)count);
    printf("ƽ����Ȩ��תʱ��Ϊ:%f\n",
           (float)sumWeightedTurnaroundTime / (float)count);
}

int main() {
    initos();
    while (1) {
        printf("-------------��ǰʱ�䣺%d-------------\n", nowTime);
        if (readyQueue == NULL && runningQueue == NULL && nowTime > 10) {
            displayTurnaroundTime(finishedQueue);
            printf("������ҵ����ɣ���������˳�...\n");
            getchar();
            break;
        }
        requestResources(runningQueue);
        initJobScheduling(&readyQueue);
        displayProcessQueue("\n����", readyQueue);
        displayProcessQueue("\n����", blockQueue);
        displayProcessQueue("\n����", runningQueue);
        if (runningQueue != NULL) {
            printf("\n��ҵ%d��������\n", runningQueue->id);
            if (runningQueue->leftServiceTime > 0)
                runningQueue->leftServiceTime -= 1;
            checkJobFinished(&runningQueue);
        }
        selectJobToRun(&readyQueue, &runningQueue);
        printf("\n���Ⱥ�Ķ������(�����������)...\n");
        // getchar();
        displayMemoryBlocks(memoryList);
        displaySystemResources(&resources);
        displayProcessQueue("\n����", readyQueue);
        displayProcessQueue("\n����", runningQueue);
        displayProcessQueue("\n����", blockQueue);
        displayProcessQueue("\n�����", finishedQueue);
        // displayMemoryBlocks(memoryList);
        printf("\n�����������...\n");
        // getchar();
        nowTime += 1;
    }
    return 0;
}

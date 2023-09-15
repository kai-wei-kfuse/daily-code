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
    int id;                     // 作业编号
    int arrival_time;           // 到达时间
    int execution_time;         // 执行时间
    int menory_size;            // 所需内存大小
    SystemResources resources;  // 最大所需系统资源
} Job;

typedef struct Process {
    int id;                             // 进程编号
    int arrivalTime;                    // 到达时间
    int serviceTime;                    // 服务时间
    int leftServiceTime;                // 剩余服务时间
    int startTime;                      // 开始时间
    int endTime;                        // 结束时间
    int turnaroundTime;                 // 周转时间
    float weightedTurnaroundTime;       // 带权周转时间
    int memorySize;                     // 所需内存大小
    SystemResources resources;          // 所需系统资源
    char status;                        // 状态
    int Allocation[MAX_RESOURCES + 1];  // 已分配的资源数
    int Need[MAX_RESOURCES + 1];        // 还需要的资源数
    struct Process* next;
} Process;

typedef struct MemoryBlock {
    int size;
    int startAddress;
    int processId;
    struct MemoryBlock* next;
} MemoryBlock;

Process* readyQueue = NULL;      // 就绪队列
Process* runningQueue = NULL;    // 运行队列
Process* finishedQueue = NULL;   // 已完成队列
Process* blockQueue = NULL;      // 阻塞队列
MemoryBlock* memoryList = NULL;  // 内存块链表
int nowTime = 0;

SystemResources resources;

int Request[3];         // 请求资源数
int Work[3];            // 系统可分配资源数
bool Finish[5];         // 标记数组
int Security_queue[5];  // 安全序列数组
bool flag = true;       // 标记是否为安全状态
// 函数声明

// 创建后备作业队列
void createBackupJobQueue();
// 初始化系统资源
void initSystemResources(SystemResources* resources);
// 初始化内存管理
MemoryBlock* createMemoryBlock(int size);
// 初始化内存管理
void initMemoryManagement(MemoryBlock** memoryList);
// 显示内存块链表
void displayMemoryBlocks(MemoryBlock* memoryList);
// 清理内存管理
void cleanupMemoryManagement(MemoryBlock* memoryList);
// 创建进程
Process* createProcess(int id, int arrivalTime, int serviceTime);
// 初始化进程调度
void initProcessScheduling(Process** readyQueue, Process** runningQueue);
// 显示进程队列
void displayProcessQueue(const char* queueName, Process* queue);
// 清理进程队列
void cleanupProcessQueue(Process* queue);
// 初始化操作系统
void initos();
// 将进程加入就绪队列
void addToReadyQueue(Process** readyQueue, Process* Process);
// 求队列长度
int length(Process* queue);
// 从后备作业队列文件中移除已选择的作业
void removeJobFromBackupJobQueue(int id);
// 分配内存(动态分区分配算法，首次适应算法)
void initJobScheduling(Process** readyQueue);
// 从后备作业队列中选择一部分作业加入就绪队列，最多5个作业
void allocationAllocationMemory(Process* Process, MemoryBlock** memoryList);
// 从就绪队列中选择一个作业加入运行队列（短作业优先算法,抢占式）
void selectJobToRun(Process** readyQueue, Process** runningQueue);
// 检查运行队列中的作业是否已完成
void checkJobFinished(Process** runningQueue);
// 释放内存
void freeMemory(Process* Process, MemoryBlock** memoryList);
// 从阻塞队列中选择一个作业加入就绪队列
void selectJobToReady(Process** blockQueue, Process** readyQueue);
// 是否有足够的系统资源
bool hasEnoughResources(Process* Process, SystemResources* resources);
// 试探性分配资源
void soundout(Process* runningQueue);
// 不安全状态
void non_soundout(Process* runningQueue);
// 安全性算法
bool safe(Process* runningQueue);
// 银行家算法
bool banker(Process* runningQueue);
// 进程请求资源
void requestResources(Process* runningQueue);
// 释放资源
void releaseResources(Process* runningQueue);

void createBackupJobQueue() {
    FILE* file = fopen("backup_job_queue.txt", "w");
    if (file == NULL) {
        printf("无法打开文件。\n");
        return;
    } else {
        printf("文件已打开。\n");
    }

    Job backup_job_queue[MAX_JOBS];
    srand(time(NULL));

    // 生成作业数据
    for (int i = 0; i < MAX_JOBS; i++) {
        backup_job_queue[i].id = i + 1;
        backup_job_queue[i].arrival_time = rand() % 10;  // 随机生成作业到达时间
        backup_job_queue[i].execution_time =
            rand() % 10 + 1;  // 随机生成作业执行时间
        backup_job_queue[i].menory_size =
            rand() % 100 + 1;  // 随机生成作业所需内存大小
        backup_job_queue[i].resources.A =
            rand() % 5 + 1;  // 随机生成作业所需系统资源A
        backup_job_queue[i].resources.B =
            rand() % 5 + 1;  // 随机生成作业所需系统资源B
        backup_job_queue[i].resources.C =
            rand() % 5 + 1;  // 随机生成作业所需系统资源C
    }
    // 按到达时间排序
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
    printf("后备作业队列已创建并保存到磁盘文件。\n");
}
// 输出进程信息
void displayProcess(Process* process) {
    printf("进程信息:");
    printf("[id:%d,到达时间：%d,服务时间:%d,剩余服务时间:%d,内存大小：%d] ",
           process->id, process->arrivalTime, process->serviceTime,
           process->leftServiceTime, process->memorySize);
    printf("进程最大需求资源:[%d,%d,%d] ", process->resources.A,
           process->resources.B, process->resources.C);
    printf("进程已分配资源:[%d,%d,%d] ", process->Allocation[0],
           process->Allocation[1], process->Allocation[2]);
    printf("进程还需资源:[%d,%d,%d]\n", process->Need[0], process->Need[1],
           process->Need[2]);
}
// 初始化系统资源

void initSystemResources(SystemResources* resources) {
    resources->A = 20;
    resources->B = 8;
    resources->C = 12;

    printf("系统资源已初始化。\n");
    printf("A: %d\n", resources->A);
    printf("B: %d\n", resources->B);
    printf("C: %d\n", resources->C);
}

// 输出资源剩余量
void displaySystemResources(SystemResources* resources) {
    printf("系统资源剩余量:\n");
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
    *memoryList = createMemoryBlock(1024);  // 创建一块大小为1024K的内存块
}

void displayMemoryBlocks(MemoryBlock* memoryList) {
    printf("\n此时的内存块链表:\n");
    printf("%-15s %-15s %-15s %-15s\n", "起始地址", "结束地址", "进程ID",
           "状态");

    while (memoryList != NULL) {
        int endAddress = memoryList->startAddress + memoryList->size - 1;
        if (memoryList->processId != -1) {
            printf("%-15d %-15d %-15d %-15s\n", memoryList->startAddress,
                   endAddress, memoryList->processId, "已分配");
        } else {
            printf("%-15d %-15d %-15s %-15s\n", memoryList->startAddress,
                   endAddress, "-", "未分配");
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
        (float)(nowTime + serviceTime) / (float)serviceTime;  // 带权周转时间
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
    printf("%s队列: \n", queueName);
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
    // 创建后备作业队列
    createBackupJobQueue();
    // 初始化系统资源

    initSystemResources(&resources);
    // 初始化内存管理
    memoryList = NULL;

    initMemoryManagement(&memoryList);
    displayMemoryBlocks(memoryList);

    // cleanupMemoryManagement(memoryList);
    readyQueue = NULL;
    runningQueue = NULL;

    initProcessScheduling(&readyQueue, &runningQueue);
    displayProcessQueue("就绪", readyQueue);
    displayProcessQueue("运行", runningQueue);
    displayProcessQueue("阻塞", blockQueue);
    displayProcessQueue("已完成", finishedQueue);

    // cleanupProcessQueue(readyQueue);
    // cleanupProcessQueue(runningQueue);
    printf("操作系统已初始化!\n");
    printf("按任意键继续...\n");
    getchar();
}
// 将进程加入就绪队列
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
    printf("作业%d已加入就绪队列:\n", process->id);
    displayProcess(process);
}
// 进程加入阻塞队列
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
    printf("作业%d已加入阻塞队列:\n", process->id);
    displayProcess(process);
}

// 求队列长度

int length(Process* queue) {
    int len = 0;
    while (queue != NULL) {
        len++;
        queue = queue->next;
    }
    return len;
}
// 从后备作业队列文件中移除已选择的作业
void removeJobFromBackupJobQueue(int id) {
    FILE* file = fopen("backup_job_queue.txt", "r");
    if (file == NULL) {
        printf("无法打开后备作业队列文件。\n");
        return;
    }

    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("无法创建临时文件。\n");
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

    remove("backup_job_queue.txt");              // 删除原文件
    rename("temp.txt", "backup_job_queue.txt");  // 替换原文件
}

// 分配内存(动态分区分配算法，首次适应算法)
void allocationAllocationMemory(Process* Process, MemoryBlock** memoryList) {
    MemoryBlock* block = *memoryList;
    MemoryBlock* prev = NULL;

    while (block != NULL) {
        if (block->processId == -1 && block->size >= Process->memorySize) {
            // 如果内存块未分配且大小大于作业所需内存大小
            if (block->size == Process->memorySize) {
                // 如果内存块大小与作业所需内存大小相等，则将作业分配到该内存块
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
                // 如果内存块大小大于作业所需内存大小，则将作业分配到该内存块，且需要将该内存块分割成两个内存块
                MemoryBlock* newBlock = createMemoryBlock(Process->memorySize);
                newBlock->processId = Process->id;
                newBlock->startAddress = block->startAddress;
                block->size -= Process->memorySize;
                block->startAddress += Process->memorySize;
                // 将新的内存块插入到链表中
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

    printf("无法为作业 %d 分配内存。\n", Process->id);
}

// 从后备作业队列中选择一部分作业加入就绪队列，最多5个作业
void initJobScheduling(Process** readyQueue) {
    // 从后备作业队列中选择一部分作业加入就绪队列，最多5个作业
    FILE* file = fopen("backup_job_queue.txt", "r");
    if (file == NULL) {
        printf("无法打开后备作业队列文件。\n");
        return;
    } else {
        printf("文件已打开。\n");
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
    // 把已经加入就绪队列的作业从后备作业队列中移除
    Process** p = readyQueue;
    for (int i = 0; i < count; i++) {
        removeJobFromBackupJobQueue((*p)->id);
        p = &((*p)->next);
    }
    printf("后备作业队列已更新。\n");
}

// 从就绪队列中选择一个作业加入运行队列（短作业优先算法,抢占式）
void selectJobToRun(Process** readyQueue, Process** runningQueue) {
    if (*readyQueue == NULL) {
        printf("就绪队列为空。\n");
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
    printf("此时的最短作业:\n");
    displayProcess(selectedProcess);
    // 如果运行队列的作业的服务时间大于就绪队列中的作业的服务时间，则把运行队列中的作业加入就绪队列
    if (*runningQueue != NULL &&
        (*runningQueue)->serviceTime > selectedProcess->serviceTime) {
        addToReadyQueue(readyQueue, *runningQueue);
        *runningQueue = NULL;
        printf("实行抢占\n");
    }
    if (*runningQueue == NULL) {
        if (prev2 == NULL) {  // 如果就绪队列只有一个作业
            *readyQueue = selectedProcess->next;
        } else {
            prev2->next = selectedProcess->next;
        }
        selectedProcess->next = NULL;
        *runningQueue = selectedProcess;
        printf("作业%d已加入运行队列:\n", selectedProcess->id);
        displayProcess(selectedProcess);
    } else {
        printf("不实行抢占\n");
    }
}

// 释放内存
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

// 是否有足够的系统资源
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

// 从阻塞队列中选择一个作业加入就绪队列
void selectJobToReady(Process** blockQueue, Process** readyQueue) {
    if (*blockQueue == NULL) {
        printf("阻塞队列为空。\n");
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
    printf("此时的最短作业:\n[id:%d,到达时间：%d,服务时间%d,内存大小：%d]\n",
           selectedProcess->id, selectedProcess->arrivalTime,
           selectedProcess->serviceTime, selectedProcess->memorySize);
    if (prev2 == NULL) {  // 如果阻塞队列只有一个作业
        *blockQueue = selectedProcess->next;
    } else {
        prev2->next = selectedProcess->next;
    }
    selectedProcess->next = NULL;
    addToReadyQueue(readyQueue, selectedProcess);
    printf("作业%d已加入就绪队列:\n ", selectedProcess->id);
    displayProcess(selectedProcess);
}

// 试探性分配资源
void soundout(Process* runningQueue) {  // 安全状态
    Process* i = runningQueue;
    resources.A = resources.A - Request[0];
    resources.B = resources.B - Request[1];
    resources.C = resources.C - Request[2];
    for (int j = 0; j < MAX_RESOURCES; j++) {
        i->Allocation[j] = i->Allocation[j] + Request[j];
        i->Need[j] = i->Need[j] - Request[j];
    }
}

void non_soundout(Process* runningQueue) {  // 不安全状态
    Process* i = runningQueue;
    resources.A = resources.A + Request[0];
    resources.B = resources.B + Request[1];
    resources.C = resources.C + Request[2];
    for (int j = 0; j < MAX_RESOURCES; j++) {
        i->Allocation[j] = i->Allocation[j] - Request[j];
        i->Need[j] = i->Need[j] + Request[j];
    }
}
bool safe(Process* runningQueue) {  // 安全性算法
    // 统计进程数
    if (hasEnoughResources(runningQueue, &resources)) {
        printf("系统资源足够\n");
        return true;
    }
    int m = 0;
    Process* w = runningQueue;
    while (w != NULL) {
        w = w->next;
        m++;
    }
    // 初始化Work数组
    Work[0] = resources.A;
    Work[1] = resources.B;
    Work[2] = resources.C;
    // 初始化Finish数组
    for (int i = 0; i < MAX_JOBS_QUEUE; i++) {
        Finish[i] = false;
    }
    // 节点位置，判断进程运行
    int pnum = 0;
    int apply;
    int num = 0;
    // 求安全序列
    Process* p = runningQueue;
    while (p != NULL && pnum <= m) {
        apply = 1;
        for (int i = 0; i < MAX_RESOURCES; i++) {
            // 当前进程未完成并且各类资源小于或等于系统资源数
            if (Finish[pnum] == false && p->Need[i] <= Work[i]) {
                apply++;
                // 直到每类资源尚需数都小于系统可利用资源数才可分配
                if (apply == MAX_RESOURCES) {
                    for (int x = 0; x < MAX_RESOURCES; x++) {
                        // 更新当前系统可分配资源
                        Work[x] += p->Allocation[x];
                    }
                    // 更新标记数组
                    Finish[pnum] = true;
                    // 更新安全序列数组
                    Security_queue[num++] = p->id;
                    // 保证每次查询均从第一个进程开始
                    pnum = 0;
                    p = runningQueue;
                }
            }
        }
        // 查询下一进程
        pnum++;
        p = p->next;
    }
    for (int i = 0; i < m; i++) {
        if (Finish[i] == false) {
            // 系统处于不安全状态
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
// 进程请求资源
void requestResources(Process* runningQueue) {
    if (runningQueue == NULL) {
        printf("运行队列为空。\n");
        return;
    }

    // 随机生成进程请求的资源数
    Process* p = runningQueue;
    srand(time(NULL));
    SystemResources Need;
    Need.A = p->resources.A - p->Allocation[0];
    Need.B = p->resources.B - p->Allocation[1];
    Need.C = p->resources.C - p->Allocation[2];
    if (Need.A == 0 && Need.B == 0 && Need.C == 0) {
        printf("作业%d已请求最大资源\n", p->id);
        // releaseResources(runningQueue);
        // printf("作业%d资源使用并释放完毕\n", p->id);
        return;
    }
    Request[0] = rand() % (Need.A + 1);
    Request[1] = rand() % (Need.B + 1);
    Request[2] = rand() % (Need.C + 1);
    printf("作业%d请求资源数:\n", p->id);
    printf("A: %d\n", Request[0]);
    printf("B: %d\n", Request[1]);
    printf("C: %d\n", Request[2]);
    printf("作业%d所需资源数:\n", p->id);
    printf("A: %d\n", p->Need[0]);
    printf("B: %d\n", p->Need[1]);
    printf("C: %d\n", p->Need[2]);
    if (banker(runningQueue) || safe(runningQueue)) {
        printf("作业%d请求资源成功\n", p->id);
        displayProcess(p);
    } else {
        printf("作业%d请求资源失败\n", p->id);
        displayProcess(p);
        addToBlockQueue(&blockQueue, p);
        runningQueue = NULL;
    }
}
// 释放资源
void releaseResources(Process* runningQueue) {
    Process* p = runningQueue;
    resources.A += p->Allocation[0];
    resources.B += p->Allocation[1];
    resources.C += p->Allocation[2];
    printf("作业%d已释放资源\n", p->id);
    displayProcess(p);
}

// 检查运行队列中的作业是否已完成
void checkJobFinished(Process** runningQueue) {
    if (*runningQueue != NULL) {
        if ((*runningQueue)->leftServiceTime == 0) {
            printf("作业%d已完成\n", (*runningQueue)->id);
            // 释放内存
            freeMemory(*runningQueue, &memoryList);
            (*runningQueue)->endTime = nowTime;
            (*runningQueue)->turnaroundTime =
                (*runningQueue)->endTime - (*runningQueue)->arrivalTime;
            (*runningQueue)->weightedTurnaroundTime =
                (float)(*runningQueue)->turnaroundTime /
                (float)(*runningQueue)->serviceTime;
            Process* process = *runningQueue;
            // 释放资源
            releaseResources(*runningQueue);
            // 把已完成的作业加入已完成队列
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
            // 唤醒阻塞队列中的作业
            if (blockQueue != NULL) {
                selectJobToReady(&blockQueue, &readyQueue);
            }
        }
    }
}
// 输出周转时间和带权周转时间，计算平均周转时间和平均带权周转时间
void displayTurnaroundTime(Process* finishedQueue) {
    int sumTurnaroundTime = 0;
    float sumWeightedTurnaroundTime = 0;
    int count = 0;
    while (finishedQueue != NULL) {
        printf("作业%d的周转时间为:%d\n", finishedQueue->id,
               finishedQueue->turnaroundTime);
        printf("作业%d的带权周转时间为:%f\n", finishedQueue->id,
               finishedQueue->weightedTurnaroundTime);
        sumTurnaroundTime += finishedQueue->turnaroundTime;
        sumWeightedTurnaroundTime += finishedQueue->weightedTurnaroundTime;
        count++;
        finishedQueue = finishedQueue->next;
    }
    printf("平均周转时间为:%f\n", (float)sumTurnaroundTime / (float)count);
    printf("平均带权周转时间为:%f\n",
           (float)sumWeightedTurnaroundTime / (float)count);
}

int main() {
    initos();
    while (1) {
        printf("-------------当前时间：%d-------------\n", nowTime);
        if (readyQueue == NULL && runningQueue == NULL && nowTime > 10) {
            displayTurnaroundTime(finishedQueue);
            printf("所有作业已完成，按任意键退出...\n");
            getchar();
            break;
        }
        requestResources(runningQueue);
        initJobScheduling(&readyQueue);
        displayProcessQueue("\n就绪", readyQueue);
        displayProcessQueue("\n阻塞", blockQueue);
        displayProcessQueue("\n运行", runningQueue);
        if (runningQueue != NULL) {
            printf("\n作业%d正在运行\n", runningQueue->id);
            if (runningQueue->leftServiceTime > 0)
                runningQueue->leftServiceTime -= 1;
            checkJobFinished(&runningQueue);
        }
        selectJobToRun(&readyQueue, &runningQueue);
        printf("\n调度后的队列情况(按任意键继续)...\n");
        // getchar();
        displayMemoryBlocks(memoryList);
        displaySystemResources(&resources);
        displayProcessQueue("\n就绪", readyQueue);
        displayProcessQueue("\n运行", runningQueue);
        displayProcessQueue("\n阻塞", blockQueue);
        displayProcessQueue("\n已完成", finishedQueue);
        // displayMemoryBlocks(memoryList);
        printf("\n按任意键继续...\n");
        // getchar();
        nowTime += 1;
    }
    return 0;
}

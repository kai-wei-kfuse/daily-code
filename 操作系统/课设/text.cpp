#include <stdio.h>
#include <stdlib.h>

#define MAX_JOBS 5

typedef struct Process {
    int id;
    int arrivalTime;
    int serviceTime;
    int startTime;
    int endTime;
    int turnaroundTime;
    float weightedTurnaroundTime;
    struct Process* next;
} Process;

void createJobQueueFile() {
    // 创建后备作业队列文件并保存到磁盘中
    FILE* file = fopen("job_queue.txt", "w");
    if (file == NULL) {
        printf("无法创建后备作业队列文件。\n");
        return;
    }

    // 写入作业到文件中
    fprintf(file, "作业1\n");
    fprintf(file, "作业2\n");
    // ...
    fprintf(file, "作业20\n");

    fclose(file);
}

void addToReadyQueue(Process** readyQueue, Process* process) {
    // 将进程加入就绪队列
    if (*readyQueue == NULL) {
        *readyQueue = process;
    } else {
        Process* temp = *readyQueue;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = process;
    }
}

void removeFromJobQueue(const char* jobQueueFile, int jobId) {
    // 从后备作业队列文件中移除已选择的作业
    FILE* file = fopen(jobQueueFile, "r");
    if (file == NULL) {
        printf("无法打开后备作业队列文件。\n");
        return;
    }

    // 创建临时文件
    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("无法创建临时文件。\n");
        fclose(file);
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        int id;
        if (sscanf(line, "作业%d", &id) == 1) {
            if (id != jobId) {
                fprintf(tempFile, "%s", line);
            }
        } else {
            fprintf(tempFile, "%s", line);
        }
    }

    fclose(file);
    fclose(tempFile);

    // 替换原文件
    remove(jobQueueFile);
    rename("temp.txt", jobQueueFile);
}

void initJobScheduling(Process** readyQueue) {
    // 从后备作业队列中选择一部分作业加入就绪队列，最多5个作业
    FILE* file = fopen("job_queue.txt", "r");
    if (file == NULL) {
        printf("无法打开后备作业队列文件。\n");
        return;
    }

    int count = 0;
    char line[100];
    while (count < MAX_JOBS && fgets(line, sizeof(line), file)) {
        int id;
        if (sscanf(line, "作业%d", &id) == 1) {
            Process* process = (Process*)malloc(sizeof(Process));
            process->id = id;
            process->arrivalTime = 0;
            process->serviceTime = 0;
            process->startTime = 0;
            process->endTime = 0;
            process->turnaroundTime = 0;
            process->weightedTurnaroundTime = 0.0;
            process->next = NULL;
            addToReadyQueue(readyQueue, process);
            count++;
        }
    }

    fclose(file);
}

void displayProcessQueue(const char* queueName, Process* queue) {
    printf("%s队列: ", queueName);
    while (queue != NULL) {
        printf("P%d(AT:%d ST:%d) -> ", queue->id, queue->arrivalTime,
               queue->serviceTime);
        queue = queue->next;
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

int main() {
    Process* readyQueue = NULL;

    // 创建后备作业队列文件并保存到磁盘中
    createJobQueueFile();

    // 从后备作业队列中选择一部分作业加入就绪队列
    initJobScheduling(&readyQueue);

    // 显示初始化信息
    displayProcessQueue("就绪", readyQueue);

    // 从后备作业队列中移除已选择的作业
    removeFromJobQueue("job_queue.txt", readyQueue->id);

    // 显示移除作业后的后备作业队列
    printf("后备作业队列: ");
    FILE* file = fopen("job_queue.txt", "r");
    if (file == NULL) {
        printf("无法打开后备作业队列文件。\n");
        return 0;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);

    // 清理资源
    cleanupProcessQueue(readyQueue);

    return 0;
}

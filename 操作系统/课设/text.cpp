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
    // ��������ҵ�����ļ������浽������
    FILE* file = fopen("job_queue.txt", "w");
    if (file == NULL) {
        printf("�޷���������ҵ�����ļ���\n");
        return;
    }

    // д����ҵ���ļ���
    fprintf(file, "��ҵ1\n");
    fprintf(file, "��ҵ2\n");
    // ...
    fprintf(file, "��ҵ20\n");

    fclose(file);
}

void addToReadyQueue(Process** readyQueue, Process* process) {
    // �����̼����������
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
    // �Ӻ���ҵ�����ļ����Ƴ���ѡ�����ҵ
    FILE* file = fopen(jobQueueFile, "r");
    if (file == NULL) {
        printf("�޷��򿪺���ҵ�����ļ���\n");
        return;
    }

    // ������ʱ�ļ�
    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("�޷�������ʱ�ļ���\n");
        fclose(file);
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        int id;
        if (sscanf(line, "��ҵ%d", &id) == 1) {
            if (id != jobId) {
                fprintf(tempFile, "%s", line);
            }
        } else {
            fprintf(tempFile, "%s", line);
        }
    }

    fclose(file);
    fclose(tempFile);

    // �滻ԭ�ļ�
    remove(jobQueueFile);
    rename("temp.txt", jobQueueFile);
}

void initJobScheduling(Process** readyQueue) {
    // �Ӻ���ҵ������ѡ��һ������ҵ����������У����5����ҵ
    FILE* file = fopen("job_queue.txt", "r");
    if (file == NULL) {
        printf("�޷��򿪺���ҵ�����ļ���\n");
        return;
    }

    int count = 0;
    char line[100];
    while (count < MAX_JOBS && fgets(line, sizeof(line), file)) {
        int id;
        if (sscanf(line, "��ҵ%d", &id) == 1) {
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
    printf("%s����: ", queueName);
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

    // ��������ҵ�����ļ������浽������
    createJobQueueFile();

    // �Ӻ���ҵ������ѡ��һ������ҵ�����������
    initJobScheduling(&readyQueue);

    // ��ʾ��ʼ����Ϣ
    displayProcessQueue("����", readyQueue);

    // �Ӻ���ҵ�������Ƴ���ѡ�����ҵ
    removeFromJobQueue("job_queue.txt", readyQueue->id);

    // ��ʾ�Ƴ���ҵ��ĺ���ҵ����
    printf("����ҵ����: ");
    FILE* file = fopen("job_queue.txt", "r");
    if (file == NULL) {
        printf("�޷��򿪺���ҵ�����ļ���\n");
        return 0;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);

    // ������Դ
    cleanupProcessQueue(readyQueue);

    return 0;
}

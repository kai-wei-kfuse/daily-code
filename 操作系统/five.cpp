#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSES 50
#define MAX_READY_QUEUE 5

// 进程状态枚举
typedef enum {
    WAITING,
    RUNNING,
    FINISHED
} ProcessStatus;

// 进程控制块（PCB）结构体
typedef struct {
    int process_id;             // 进程ID
    int arrival_time;           // 到达时间
    int burst_time;             // 执行时间
    int remaining_time;         // 剩余执行时间
    int completion_time;        // 完成时间
    int waiting_time;           // 等待时间
    int turnaround_time;        // 周转时间
    float weighted_turnaround;  // 带权周转时间
    ProcessStatus status;       // 进程状态
} PCB;

// 全局变量
PCB processes[MAX_PROCESSES];          // 进程数组
PCB ready_queue[MAX_READY_QUEUE];      // 就绪队列
int total_processes = 0;               // 进程总数
int ready_queue_size = 0;              // 就绪队列大小
int time_quantum = 3;                  // 时间片大小

// 生成随机到达时间和执行时间
void generateRandomProcess(int process_index) {
    processes[process_index].arrival_time = rand() % 20;
    processes[process_index].burst_time = (rand() % 10) + 1;
    processes[process_index].remaining_time = processes[process_index].burst_time;
}

// 初始化进程数组
void initializeProcesses() {
    srand(time(NULL));
    processes[0].process_id = 1;
    processes[0].arrival_time = 0;
    processes[0].burst_time = (rand() % 10) + 1;
    processes[0].remaining_time = processes[0].burst_time;
    processes[0].completion_time = 0;
    processes[0].waiting_time = 0;
    processes[0].turnaround_time = 0;
    processes[0].weighted_turnaround = 0;
    processes[0].status = RUNNING;
    total_processes++;
  
    for (int i = 1; i < MAX_PROCESSES; i++) {
        processes[i].process_id = i + 1;
        generateRandomProcess(i);
        processes[i].completion_time = 0;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].weighted_turnaround = 0;
        processes[i].status = WAITING;
    }
}

// 将进程加入就绪队列
void addToReadyQueue(int process_index) {
    ready_queue[ready_queue_size] = processes[process_index];
    processes[process_index].status = WAITING;
    ready_queue_size++;
}

// 从就绪队列中移除进程
void removeFromReadyQueue(int process_index) {
    for (int i = process_index; i < ready_queue_size - 1; i++) {
        ready_queue[i] = ready_queue[i + 1];
    }
    ready_queue_size--;
}

// 短进程优先调度算法
void shortestJobFirst() {
    int current_time = 0;
  
    while (total_processes < MAX_PROCESSES) {
        int shortest_job_index = -1;
        int shortest_burst_time = INT_MAX;
      
        for (int i = 0; i < total_processes; i++) {
            if (processes[i].status == WAITING && processes[i].arrival_time <= current_time &&
                processes[i].burst_time < shortest_burst_time) {
                shortest_burst_time = processes[i].burst_time;
                shortest_job_index = i;
            }
        }
      
        if (shortest_job_index != -1) {
            processes[shortest_job_index].status = RUNNING;
            processes[shortest_job_index].waiting_time = current_time - processes[shortest_job_index].arrival_time;
          
            current_time += processes[shortest_job_index].burst_time;
          
            processes[shortest_job_index].completion_time = current_time;
            processes[shortest_job_index].turnaround_time = processes[shortest_job_index].completion_time - processes[shortest_job_index].arrival_time;
            processes[shortest_job_index].weighted_turnaround = (float)processes[shortest_job_index].turnaround_time / processes[shortest_job_index].burst_time;
          
            removeFromReadyQueue(shortest_job_index);
          
            total_processes++;
            if (total_processes < MAX_PROCESSES) {
                addToReadyQueue(total_processes);
            }
        } else {
            current_time++;
        }
      
        for (int i = 0; i < total_processes; i++) {
            if (processes[i].status == WAITING && processes[i].arrival_time <= current_time) {
                addToReadyQueue(i);
            }
        }
      
        printf("Current Time: %d\n", current_time);
        printf("Running Process: P%d\n", ready_queue[0].process_id);
        printf("Ready Queue: ");
        for (int i = 1; i < ready_queue_size; i++) {
            printf("P%d ", ready_queue[i].process_id);
        }
        printf("\n");
        printf("Finished Processes: ");
        for (int i = 0; i < total_processes; i++) {
            if (processes[i].status == FINISHED) {
                printf("P%d ", processes[i].process_id);
            }
        }
        printf("\n");
        printf("Process Information:\n");
        for (int i = 0; i < total_processes; i++) {
            printf("P%d: ", processes[i].process_id);
            printf("Arrival Time: %d, ", processes[i].arrival_time);
            printf("Burst Time: %d, ", processes[i].burst_time);
            printf("Completion Time: %d, ", processes[i].completion_time);
            printf("Waiting Time: %d, ", processes[i].waiting_time);
            printf("Turnaround Time: %d, ", processes[i].turnaround_time);
            printf("Weighted Turnaround: %.2f\n", processes[i].weighted_turnaround);
        }
      
        printf("----------------------------------------\n");
    }
}

// 时间片轮转调度算法
void roundRobin() {
    int current_time = 0;
    int quantum_time = time_quantum;
    int running_process_index = 0;
  
    while (total_processes < MAX_PROCESSES) {
        if (ready_queue[running_process_index].status == WAITING) {
            ready_queue[running_process_index].status = RUNNING;
            ready_queue[running_process_index].waiting_time = current_time - ready_queue[running_process_index].arrival_time;
        }
      
        if (ready_queue[running_process_index].remaining_time > quantum_time) {
            ready_queue[running_process_index].remaining_time -= quantum_time;
            current_time += quantum_time;
          
            if (running_process_index == ready_queue_size - 1) {
                running_process_index = 0;
            } else {
                running_process_index++;
            }
        } else {
            current_time += ready_queue[running_process_index].remaining_time;
            ready_queue[running_process_index].completion_time = current_time;
            ready_queue[running_process_index].turnaround_time = ready_queue[running_process_index].completion_time - ready_queue[running_process_index].arrival_time;
            ready_queue[running_process_index].weighted_turnaround = (float)ready_queue[running_process_index].turnaround_time / ready_queue[running_process_index].burst_time;
          
            removeFromReadyQueue(running_process_index);
            total_processes++;
            if (total_processes < MAX_PROCESSES) {
                addToReadyQueue(total_processes);
            }
          
            if (running_process_index == ready_queue_size) {
                running_process_index = 0;
            }
        }
      
        for (int i = 0; i < ready_queue_size; i++) {
            if (i != running_process_index) {
                addToReadyQueue(i);
            }
        }
      
        printf("Current Time: %d\n", current_time);
        printf("Running Process: P%d\n", ready_queue[running_process_index].process_id);
        printf("Ready Queue: ");
        for (int i = 0; i < ready_queue_size; i++) {
            if (i != running_process_index) {
                printf("P%d ", ready_queue[i].process_id);
            }
        }
        printf("\n");
        printf("Finished Processes: ");
        for (int i = 0; i < total_processes; i++) {
            if (processes[i].status == FINISHED) {
                printf("P%d ", processes[i].process_id);
            }
        }
        printf("\n");
        printf("Process Information:\n");
        for (int i = 0; i < total_processes; i++) {
            printf("P%d: ", processes[i].process_id);
            printf("Arrival Time: %d, ", processes[i].arrival_time);
            printf("Burst Time: %d, ", processes[i].burst_time);
            printf("Completion Time: %d, ", processes[i].completion_time);
            printf("Waiting Time: %d, ", processes[i].waiting_time);
            printf("Turnaround Time: %d, ", processes[i].turnaround_time);
            printf("Weighted Turnaround: %.2f\n", processes[i].weighted_turnaround);
        }
      
        printf("----------------------------------------\n");
    }
}

// 高响应比优先调度算法
void highestResponseRatioNext() {
    int current_time = 0;
  
    while (total_processes < MAX_PROCESSES) {
        int highest_response_ratio_index = -1;
        float highest_response_ratio = -1;
      
        for (int i = 0; i < total_processes; i++) {
            if (processes[i].status == WAITING && processes[i].arrival_time <= current_time) {
                float response_ratio = (float)(current_time - processes[i].arrival_time + processes[i].burst_time) / processes[i].burst_time;
                if (response_ratio > highest_response_ratio) {
                    highest_response_ratio = response_ratio;
                    highest_response_ratio_index = i;
                }
            }
        }
      
        if (highest_response_ratio_index != -1) {
            processes[highest_response_ratio_index].status = RUNNING;
            processes[highest_response_ratio_index].waiting_time = current_time - processes[highest_response_ratio_index].arrival_time;
          
            current_time += processes[highest_response_ratio_index].burst_time;
          
            processes[highest_response_ratio_index].completion_time = current_time;
            processes[highest_response_ratio_index].turnaround_time = processes[highest_response_ratio_index].completion_time - processes[highest_response_ratio_index].arrival_time;
            processes[highest_response_ratio_index].weighted_turnaround = (float)processes[highest_response_ratio_index].turnaround_time / processes[highest_response_ratio_index].burst_time;
          
            removeFromReadyQueue(highest_response_ratio_index);
          
            total_processes++;
            if (total_processes < MAX_PROCESSES) {
                addToReadyQueue(total_processes);
            }
        } else {
            current_time++;
        }
      
        for (int i = 0; i < total_processes; i++) {
            if (processes[i].status == WAITING && processes[i].arrival_time <= current_time) {
                addToReadyQueue(i);
            }
        }
      
        printf("Current Time: %d\n", current_time);
        printf("Running Process: P%d\n", ready_queue[0].process_id);
        printf("Ready Queue: ");
        for (int i = 1; i < ready_queue_size; i++) {
            printf("P%d ", ready_queue[i].process_id);
        }
        printf("\n");
        printf("Finished Processes: ");
        for (int i = 0; i < total_processes; i++) {
            if (processes[i].status == FINISHED) {
                printf("P%d ", processes[i].process_id);
            }
        }
        printf("\n");
        printf("Process Information:\n");
        for (int i = 0; i < total_processes; i++) {
            printf("P%d: ", processes[i].process_id);
            printf("Arrival Time: %d, ", processes[i].arrival_time);
            printf("Burst Time: %d, ", processes[i].burst_time);
            printf("Completion Time: %d, ", processes[i].completion_time);
            printf("Waiting Time: %d, ", processes[i].waiting_time);
            printf("Turnaround Time: %d, ", processes[i].turnaround_time);
            printf("Weighted Turnaround: %.2f\n", processes[i].weighted_turnaround);
        }
      
        printf("----------------------------------------\n");
    }
}

int main() {
    initializeProcesses();

    printf("Shortest Job First (SJF):\n");
    shortestJobFirst();

    printf("\nTime Quantum: %d\n", time_quantum);
    printf("Round Robin:\n");
    roundRobin();

    printf("\nHighest Response Ratio Next (HRRN):\n");
    highestResponseRatioNext();

    return 0;
}

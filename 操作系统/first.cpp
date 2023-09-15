#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/stdc++.h>

using namespace std;
#define MAX_PROCESSES 5
#define MAX_RUNNING 1
#define TIME_QUANTUM 5

typedef struct {
    int process_id;                  // 进程ID
    int arrival_time;                // 到达时间
    int burst_time;                  // 运行时间
    int remaining_time;              // 剩余时间
    int waiting_time;                // 等待时间
    int turnaround_time;             // 周转时间
    float weighted_turnaround_time;  // 带权周转时间
    float response_ratio;            // 响应比
    char status;                     // 状态
} PCB;

void generateProcesses(PCB* processes, int* num_processes);
// 进程生成函数，生成进程数组，进程数
void runShortestJobFirst(PCB* processes, int num_processes);
// 最短作业优先调度算法
void runRoundRobin(PCB* processes, int num_processes);
// 时间片轮转调度算法
void runHighestResponseRatioNext(PCB* processes, int num_processes);
// 最高响应比优先调度算法
void printProcessInfo(PCB process);
// 打印进程信息
void printProcessTable(PCB* processes, int num_processes);
// 打印进程表
void calculateMetrics(PCB* processes, int num_processes, float* avg_turnaround_time, float* avg_weighted_turnaround_time);
// 计算平均周转时间，平均带权周转时间
void sort_arr(PCB* p, int N);
//
void sort_ing(PCB* p, int N);

void generateProcesses(PCB* processes, int* num_processes) {
    srand(time(NULL));  // 设置随机数种子
    int i;

    for (i = 0; i < MAX_PROCESSES; i++) {
        // processes[i].process_id = i + 1;
        processes[i].arrival_time = (i == 0) ? 0 : rand() % 10;
        processes[i].burst_time = rand() % 10 + 1;
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].weighted_turnaround_time = 0.0;
        processes[i].response_ratio = 0.0;
        processes[i].status = 'W';  // W: waiting, R: running, F: finished
    }

    *num_processes = MAX_PROCESSES;
    sort_arr(processes, *num_processes);
    printf("进程ID\t到达时间\t运行时间\n");
    for (i = 0; i < MAX_PROCESSES; i++) {
        processes[i].process_id = i + 1;
        printf("%d\t%d\t\t%d\n", processes[i].process_id, processes[i].arrival_time, processes[i].burst_time);
        printf("\n");
    }
    printf("\n");
}

void sort_arr(PCB* p, int N)  // 对所有进程按照提交时间进行排序
{
    int i, j;
    PCB temp;
    for (i = 0; i < N - 1; i++) {
        for (j = 0; j < N - i - 1; j++) {
            if (p[j].arrival_time > p[j + 1].arrival_time) {
                temp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = temp;
            }
        }
    }
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

void runShortestJobFirst(PCB* processes, int num_processes) {  // 最短作业优先调度算法
    int i, j, min_index;
    int now = 0, ready_idx = 0,head=0;
    int time = 0;
    int running_count = 0;
    int running_idx = 0;
    PCB* running_processes[MAX_RUNNING];  // 运行进程数组
    PCB* p = NULL;                        // 当前进程
    PCB* ready_queue[MAX_PROCESSES];      // 就绪队列

    while (1) {
        // 检查是否有进程完成
        printf("\n%d\n", time);
        int flag_f = 0;
        if(time==0)flag_f=1;
        for (i = 0; i < running_count; i++) {
            p = running_processes[i];
            if (p->remaining_time == 0) {
                flag_f = 1;
                p->status = 'F';
                running_processes[i] = NULL;
                running_count--;
                break;
            }
        }
        cout<<1<<' ';
        for (i = now; i < num_processes; i++) {
            if (processes[i].arrival_time <= time) {
                ready_queue[ready_idx++] = &processes[i];
            }
        }
        cout<<2<<' ';
        // 就绪队列按照运行时间排序
        for (i = head; i < ready_idx - 1; i++) {
            for (j = head; j < ready_idx - i - 1; j++) {
                if (ready_queue[j]->burst_time > ready_queue[j + 1]->burst_time) {
                    PCB* temp = ready_queue[j];
                    ready_queue[j] = ready_queue[j + 1];
                    ready_queue[j + 1] = temp;
                }
            }
        }
        cout<<3<<' ';
          // 将就绪队列中的进程加入运行队列
        for (i = 0; i < ready_idx; i++) {
            cout << "x";
            if (running_count < MAX_RUNNING) {
                p = ready_queue[i];
                p->status = 'R';
                running_processes[running_idx++] = p;
                running_count++;
                running_idx %= MAX_RUNNING;
                ready_queue[i] = NULL;
                head=i+1;
            }
        }
        cout << 4 << ' ';
        // Update waiting time for all ready processes
        for (i = 0; i < num_processes; i++) {
            if (processes[i].status == 'W' && &processes[i] != p) {
                processes[i].waiting_time++;
            }
        }
        cout << 5 << ' ';
        // 更新运行进程的剩余时间
        for(i=0;i<running_count;i++){

            running_processes[i]->remaining_time--;
        }
        cout << 6 << ' ';
        // Check if all processes are finished
        int all_finished = 1;
        for (i = 0; i < num_processes; i++) {
            if (processes[i].status != 'F') {
                all_finished = 0;
                break;
            }
        }

        // Exit the loop if all processes are finished
        if (all_finished) {
            break;
        }

        if (flag_f == 1) {
            // 打印就绪队列
            printf("\n时刻: %d\n", time);
            printf("正在运行的进程: ");
            int flag = 0;
            for (i = 0; i < running_count; i++) {
                if(running_processes[i]!=NULL)
                printf("%d ", running_processes[i]->process_id);
                flag = 1;
            }
            if (flag == 0)
                printf("无");
            printf("\n");

            printf("就绪队列: ");
            for (i = head; i < ready_idx; i++) {
                printf("%d ", ready_queue[i]->process_id);
            }
            printf("\n");

            printf("进程: ");
            now += ready_idx;
            for (i = now; i < num_processes; i++) {
                printf("%d ", processes[i].process_id);
            }
            printf("\n");

            printf("完成的进程: ");
            flag = 0;
            for (i = 0; i < num_processes; i++) {
                if (processes[i].status == 'F') {
                    printf("%d ", processes[i].process_id);
                    flag = 1;
                }
            }
            if (flag == 0)
                printf("无");
            printf("\n");

           // printProcessTable(processes, num_processes);  // 打印进程表
        }

      
        time++;
       
    }
}

void runRoundRobin(PCB* processes, int num_processes) {
    int i, j;
    int time = 0;
    int running_count = 0;
    PCB* running_processes[MAX_RUNNING];
    PCB* current_process = NULL;

    while (1) {
        printf("Time: %d\n", time);
        printf("Running Processes: ");
        for (i = 0; i < running_count; i++) {
            printf("%d ", running_processes[i]->process_id);
        }
        printf("\n");

        printf("Ready Queue: ");
        for (i = 0; i < num_processes; i++) {
            if (processes[i].status == 'W') {
                printf("%d ", processes[i].process_id);
            }
        }
        printf("\n");

        printf("Finished Processes: ");
        for (i = 0; i < num_processes; i++) {
            if (processes[i].status == 'F') {
                printf("%d ", processes[i].process_id);
            }
        }
        printf("\n");

        printf("Process Table:\n");
        printProcessTable(processes, num_processes);

        // Check if any running process completes its time quantum
        for (i = 0; i < running_count; i++) {
            current_process = running_processes[i];
            if (time % TIME_QUANTUM == 0 && current_process->remaining_time > 0) {
                current_process->status = 'W';
                running_processes[i] = NULL;
                running_count--;
                break;
            }
        }

        // Schedule a new process if no process is running
        if (current_process == NULL) {
            for (i = 0; i < num_processes; i++) {
                if (processes[i].status == 'W') {
                    current_process = &processes[i];
                    current_process->status = 'R';
                    running_processes[running_count] = current_process;
                    running_count++;
                    break;
                }
            }
        }

        // Update the remaining time of the running process
        if (current_process != NULL) {
            current_process->remaining_time--;
        }

        // Update waiting time for all ready processes
        for (i = 0; i < num_processes; i++) {
            if (processes[i].status == 'W' && &processes[i] != current_process) {
                processes[i].waiting_time++;
            }
        }

        // Check if all processes are finished
        int all_finished = 1;
        for (i = 0; i < num_processes; i++) {
            if (processes[i].status != 'F') {
                all_finished = 0;
                break;
            }
        }

        // Exit the loop if all processes are finished
        if (all_finished) {
            break;
        }

        time++;
    }
}

void runHighestResponseRatioNext(PCB* processes, int num_processes) {
    int i, j;
    int time = 0;
    int running_count = 0;
    PCB* running_processes[MAX_RUNNING];
    PCB* current_process = NULL;

    while (1) {
        printf("Time: %d\n", time);
        printf("Running Processes: ");
        for (i = 0; i < running_count; i++) {
            printf("%d ", running_processes[i]->process_id);
        }
        printf("\n");

        printf("Ready Queue: ");
        for (i = 0; i < num_processes; i++) {
            if (processes[i].status == 'W') {
                printf("%d ", processes[i].process_id);
            }
        }
        printf("\n");

        printf("Finished Processes: ");
        for (i = 0; i < num_processes; i++) {
            if (processes[i].status == 'F') {
                printf("%d ", processes[i].process_id);
            }
        }
        printf("\n");

        printf("Process Table:\n");
        printProcessTable(processes, num_processes);

        // Check if any running process completes
        for (i = 0; i < running_count; i++) {
            current_process = running_processes[i];
            if (current_process->remaining_time == 0) {
                current_process->status = 'F';
                running_processes[i] = NULL;
                running_count--;
                break;
            }
        }

        // Calculate response ratio for all ready processes
        for (i = 0; i < num_processes; i++) {
            if (processes[i].status == 'W') {
                processes[i].response_ratio = (float)(time - processes[i].arrival_time + processes[i].burst_time) / processes[i].burst_time;
            }
        }

        // Find the process with the highest response ratio in the ready queue
        float max_response_ratio = 0.0;
        int max_index = -1;
        for (i = 0; i < num_processes; i++) {
            if (processes[i].status == 'W') {
                if (max_index == -1 || processes[i].response_ratio > max_response_ratio) {
                    max_response_ratio = processes[i].response_ratio;
                    max_index = i;
                }
            }
        }

        // If there is a running process, check if it needs to be preempted
        if (current_process != NULL && (max_index != -1 && processes[max_index].response_ratio > current_process->response_ratio)) {
            current_process->status = 'W';
            running_processes[i] = NULL;
            running_count--;
            current_process = NULL;
        }

        // Schedule a new process if no process is running
        if (current_process == NULL && max_index != -1) {
            current_process = &processes[max_index];
            current_process->status = 'R';
            running_processes[running_count] = current_process;
            running_count++;
        }

        // Update the remaining time of the running process
        if (current_process != NULL) {
            current_process->remaining_time--;
        }

        // Update waiting time for all ready processes
        for (i = 0; i < num_processes; i++) {
            if (processes[i].status == 'W' && &processes[i] != current_process) {
                processes[i].waiting_time++;
            }
        }

        // Check if all processes are finished
        int all_finished = 1;
        for (i = 0; i < num_processes; i++) {
            if (processes[i].status != 'F') {
                all_finished = 0;
                break;
            }
        }

        // Exit the loop if all processes are finished
        if (all_finished) {
            break;
        }

        time++;
    }
}

void printProcessInfo(PCB process) {
    printf("进程ID: %d | 到达时间: %d | 运行时间: %d | 剩余时间: %d | 等待时间: %d | 周转时间: %d | Weighted Turnaround Time: %.2f | Response Ratio: %.2f\n",
           process.process_id, process.arrival_time, process.burst_time, process.remaining_time, process.waiting_time, process.turnaround_time, process.weighted_turnaround_time, process.response_ratio);
}

void printProcessTable(PCB* processes, int num_processes) {
    int i;

    for (i = 0; i < num_processes; i++) {
        printProcessInfo(processes[i]);
    }

    printf("--------------------------------------------------------------------------------\n");
}

void calculateMetrics(PCB* processes, int num_processes, float* avg_turnaround_time, float* avg_weighted_turnaround_time) {
    int i;
    float total_turnaround_time = 0.0;
    float total_weighted_turnaround_time = 0.0;

    for (i = 0; i < num_processes; i++) {
        processes[i].turnaround_time = processes[i].waiting_time + processes[i].burst_time;
        processes[i].weighted_turnaround_time = (float)processes[i].turnaround_time / processes[i].burst_time;
        total_turnaround_time += processes[i].turnaround_time;
        total_weighted_turnaround_time += processes[i].weighted_turnaround_time;
    }

    *avg_turnaround_time = total_turnaround_time / num_processes;
    *avg_weighted_turnaround_time = total_weighted_turnaround_time / num_processes;
}

int main() {
    PCB processes[MAX_PROCESSES];  // 进程数组

    int num_processes;
    float avg_turnaround_time, avg_weighted_turnaround_time;

    generateProcesses(processes, &num_processes);  // 生成进程
    printf("最短作业优先调度算法：\n");
    runShortestJobFirst(processes, num_processes);
    calculateMetrics(processes, num_processes, &avg_turnaround_time, &avg_weighted_turnaround_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
    printf("Average Weighted Turnaround Time: %.2f\n", avg_weighted_turnaround_time);
    printf("\n");

    generateProcesses(processes, &num_processes);
    printf("Round Robin Scheduling:\n");
    runRoundRobin(processes, num_processes);
    calculateMetrics(processes, num_processes, &avg_turnaround_time, &avg_weighted_turnaround_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
    printf("Average Weighted Turnaround Time: %.2f\n", avg_weighted_turnaround_time);
    printf("\n");

    generateProcesses(processes, &num_processes);
    printf("Highest Response Ratio Next Scheduling:\n");
    runHighestResponseRatioNext(processes, num_processes);
    calculateMetrics(processes, num_processes, &avg_turnaround_time, &avg_weighted_turnaround_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
    printf("Average Weighted Turnaround Time: %.2f\n", avg_weighted_turnaround_time);

    return 0;
}

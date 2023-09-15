#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSES 50
#define MAX_RUNNING_PROCESSES 5

typedef struct {
    int process_id;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int turnaround_time;
    float weighted_turnaround_time;
    char status;
} Process;

void generateProcesses(Process processes[]);
void initializeProcesses(Process processes[]);
void updateWaitingTimes(Process processes[], int running_process);
void updateTurnaroundTimes(Process processes[], int running_process);
void updateWeightedTurnaroundTimes(Process processes[]);
void printProcessInfo(Process processes[], int running_process);
void printProcessStatus(Process processes[], int running_process);
float calculateAverageTurnaroundTime(Process processes[]);
float calculateAverageWeightedTurnaroundTime(Process processes[]);

int main() {
    Process processes[MAX_PROCESSES];
    int running_process = 0;
    int total_processes = 1;
    int current_time = 0;
    int completed_processes = 0;

    srand(time(NULL));

    generateProcesses(processes);
    initializeProcesses(processes);

    while (completed_processes < MAX_PROCESSES) {
        if (processes[running_process].burst_time > 0) {
            processes[running_process].burst_time--;
            updateWaitingTimes(processes, running_process);
            updateTurnaroundTimes(processes, running_process);
            updateWeightedTurnaroundTimes(processes);
            current_time++;
        }

        if (processes[running_process].burst_time == 0) {
            completed_processes++;
            processes[running_process].status = 'F';

            if (total_processes < MAX_PROCESSES) {
                processes[total_processes].arrival_time = current_time;
                total_processes++;
            }
        }

        running_process = 0;
        for (int i = 1; i < total_processes; i++) {
            if (processes[i].status != 'F' && processes[i].burst_time < processes[running_process].burst_time) {
                running_process = i;
            }
        }

        printProcessInfo(processes, running_process);
        printProcessStatus(processes, running_process);
    }

    float avg_turnaround_time = calculateAverageTurnaroundTime(processes);
    float avg_weighted_turnaround_time = calculateAverageWeightedTurnaroundTime(processes);

    printf("\nAverage Turnaround Time: %.2f\n", avg_turnaround_time);
    printf("Average Weighted Turnaround Time: %.2f\n", avg_weighted_turnaround_time);

    return 0;
}

void generateProcesses(Process processes[]) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].process_id = i;
        processes[i].arrival_time = i == 0 ? 0 : rand() % 20;  // Random arrival time for processes after the first one
        processes[i].burst_time = 1 + rand() % 10;  // Random burst time between 1 and 10
    }
}

void initializeProcesses(Process processes[]) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].weighted_turnaround_time = 0;
        processes[i].status = 'W';
    }
}

void updateWaitingTimes(Process processes[], int running_process) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (i != running_process && processes[i].status != 'F') {
            processes[i].waiting_time++;
        }
    }
}

void updateTurnaroundTimes(Process processes[], int running_process) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (i != running_process && processes[i].status != 'F') {
            processes[i].turnaround_time++;
        }
    }
}

void updateWeightedTurnaroundTimes(Process processes[]) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].status != 'F') {
            processes[i].weighted_turnaround_time = (float) processes[i].turnaround_time / processes[i].burst_time;
        }
    }
}

void printProcessInfo(Process processes[], int running_process) {
    printf("Current Time: %d\n", processes[running_process].arrival_time);
    printf("Running Process: %d\n", running_process);
    printf("Process ID\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time\tWeighted Turnaround Time\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t\t%.2f\n",
               processes[i].process_id, processes[i].arrival_time, processes[i].burst_time,
               processes[i].waiting_time, processes[i].turnaround_time, processes[i].weighted_turnaround_time);
    }
    printf("\n");
}

void printProcessStatus(Process processes[], int running_process) {
    printf("Process Status:\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (i == running_process) {
            printf("R ");
        } else if (processes[i].status == 'W') {
            printf("W ");
        } else {
            printf("F ");
        }
    }
    printf("\n\n");
}

float calculateAverageTurnaroundTime(Process processes[]) {
    float total_turnaround_time = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        total_turnaround_time += processes[i].turnaround_time;
    }
    return total_turnaround_time / MAX_PROCESSES;
}

float calculateAverageWeightedTurnaroundTime(Process processes[]) {
    float total_weighted_turnaround_time = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        total_weighted_turnaround_time += processes[i].weighted_turnaround_time;
    }
    return total_weighted_turnaround_time / MAX_PROCESSES;
}

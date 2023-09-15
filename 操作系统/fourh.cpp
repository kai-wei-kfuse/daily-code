#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSES 50
#define TIME_QUANTUM 5

typedef struct {
    int process_id;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int turnaround_time;
    int waiting_time;
} Process;

typedef struct {
    int front, rear;
    Process processes[MAX_PROCESSES];
} Queue;

void initialize(Queue* queue) {
    queue->front = queue->rear = -1;
}

int is_empty(Queue* queue) {
    return queue->front == -1;
}

int is_full(Queue* queue) {
    return (queue->rear + 1) % MAX_PROCESSES == queue->front;
}

void enqueue(Queue* queue, Process process) {
    if (is_full(queue)) {
        printf("Error: Queue is full\n");
        exit(1);
    }

    if (is_empty(queue)) {
        queue->front = queue->rear = 0;
    } else {
        queue->rear = (queue->rear + 1) % MAX_PROCESSES;
    }

    queue->processes[queue->rear] = process;
}

Process dequeue(Queue* queue) {
    if (is_empty(queue)) {
        printf("Error: Queue is empty\n");
        exit(1);
    }

    Process process = queue->processes[queue->front];

    if (queue->front == queue->rear) {
        initialize(queue);
    } else {
        queue->front = (queue->front + 1) % MAX_PROCESSES;
    }

    return process;
}

int get_queue_size(Queue* queue) {
    if (is_empty(queue)) {
        return 0;
    } else if (queue->front <= queue->rear) {
        return queue->rear - queue->front + 1;
    } else {
        return MAX_PROCESSES - queue->front + queue->rear + 1;
    }
}

void generate_processes(Process processes[], int max_processes) {
    srand(time(NULL));

    processes[0].process_id = 1;
    processes[0].arrival_time = 0;
    processes[0].burst_time = rand() % 10 + 1;
    processes[0].remaining_time = processes[0].burst_time;
    processes[0].turnaround_time = 0;
    processes[0].waiting_time = 0;

    for (int i = 1; i < max_processes; i++) {
        processes[i].process_id = i + 1;
        processes[i].arrival_time = processes[i - 1].arrival_time + rand() % 5 + 1;
        processes[i].burst_time = rand() % 10 + 1;
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].turnaround_time = 0;
        processes[i].waiting_time = 0;
    }
}

void print_process_info(Process process) {
    printf("Process ID: %d\tArrival Time: %d\tBurst Time: %d\tRemaining Time: %d\n",
           process.process_id, process.arrival_time, process.burst_time, process.remaining_time);
}

void print_queue(Queue* queue) {
    printf("Ready Queue: ");
    int i = queue->front;
    while (i != queue->rear) {
        print_process_info(queue->processes[i]);
        i = (i + 1) % MAX_PROCESSES;
    }
    print_process_info(queue->processes[i]);
}

void print_processes(Process processes[], int num_processes) {
    printf("All Processes:\n");
    for (int i = 0; i < num_processes; i++) {
        print_process_info(processes[i]);
    }
}

void schedule_processes(Process processes[], int num_processes) {
    Queue ready_queue;
    initialize(&ready_queue);

    int current_time = 0;
    int completed_processes = 0;
    int total_turnaround_time = 0;
    int total_waiting_time = 0;

    while (completed_processes < num_processes) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time == current_time) {
                enqueue(&ready_queue, processes[i]);
            }
        }

        if (!is_empty(&ready_queue)) {
            Process current_process = dequeue(&ready_queue);

            if (current_process.remaining_time <= TIME_QUANTUM) {
                current_time += current_process.remaining_time;
                current_process.remaining_time = 0;
                current_process.turnaround_time = current_time - current_process.arrival_time;
                current_process.waiting_time = current_process.turnaround_time - current_process.burst_time;
                total_turnaround_time += current_process.turnaround_time;
                total_waiting_time += current_process.waiting_time;
                completed_processes++;
            } else {
                current_time += TIME_QUANTUM;
                current_process.remaining_time -= TIME_QUANTUM;
                enqueue(&ready_queue, current_process);
            }
        } else {
            current_time++;
        }

        printf("Current Time: %d\n", current_time);
        printf("Running Process: None\n");
        print_queue(&ready_queue);
        printf("Completed Processes: %d/%d\n", completed_processes, num_processes);
        printf("\n");
    }

    double average_turnaround_time = (double)total_turnaround_time / num_processes;
    double average_waiting_time = (double)total_waiting_time / num_processes;

    printf("Average Turnaround Time: %.2f\n", average_turnaround_time);
    printf("Average Waiting Time: %.2f\n", average_waiting_time);
}

int main() {
    Process processes[MAX_PROCESSES];
    generate_processes(processes, MAX_PROCESSES);
    print_processes(processes, MAX_PROCESSES);
    printf("\n");

    schedule_processes(processes, MAX_PROCESSES);

    return 0;
}

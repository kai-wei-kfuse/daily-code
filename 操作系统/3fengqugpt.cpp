#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 1024  // 用户内存空间大小
#define MAX_JOBS 20       // 最大作业数量
#define TIME_QUANTUM 10   // 时间片大小

// 作业控制块结构体
typedef struct {
    int job_id;         // 作业ID
    int size;           // 作业大小
    int start_address;  // 起始地址
    int status;         // 作业状态 (0:未分配, 1:已分配)
} JobControlBlock;

// 内存分区结构体
typedef struct {
    int start_address;  // 分区起始地址
    int size;           // 分区大小
} MemoryPartition;

// 全局变量
JobControlBlock job_queue[MAX_JOBS];  // 作业队列
int job_count = 0;                    // 作业数量
MemoryPartition memory[1];            // 内存分区
int running_jobs = 0;                 // 正在运行的作业数量

// 打印内存状态图
void print_memory_status() {
    int i, j;
    printf("\nMemory Status:\n");
    for (i = 0; i < MEMORY_SIZE; i++) {
        int flag = 0;
        for (j = 0; j < job_count; j++) {
            if (i >= job_queue[j].start_address &&
                i < (job_queue[j].start_address + job_queue[j].size)) {
                printf("%d", job_queue[j].job_id);
                flag = 1;
                break;
            }
        }
        if (!flag) {
            printf("-");
        }
    }
    printf("\n");
}

// 创建作业控制块
JobControlBlock create_job(int job_id, int job_size) {
    JobControlBlock job;
    job.job_id = job_id;
    job.size = job_size;
    job.start_address = -1;
    job.status = 0;
    return job;
}

// 添加作业到队列
void add_job_to_queue(JobControlBlock job) {
    if (job_count < MAX_JOBS) {
        job_queue[job_count++] = job;
        printf("Job %d added to the queue.\n", job.job_id);
    } else {
        printf("Job queue is full. Cannot add job %d.\n", job.job_id);
    }
}

// 内存分配函数（首次适应算法）
void allocate_memory_FCFS() {
    int i, j;
    for (i = 0; i < job_count; i++) {
        if (job_queue[i].status == 0) {
            for (j = 0; j < 1; j++) {
                if (memory[j].size >= job_queue[i].size) {
                    job_queue[i].start_address = memory[j].start_address;
                    memory[j].start_address += job_queue[i].size;
                    memory[j].size -= job_queue[i].size;
                    job_queue[i].status = 1;
                    printf("Job %d allocated memory.\n", job_queue[i].job_id);
                    break;
                }
            }
        }
    }
}

// 内存回收函数
void deallocate_memory(int job_id) {
    int i;
    for (i = 0; i < job_count; i++) {
        if (job_queue[i].job_id == job_id && job_queue[i].status == 1) {
            job_queue[i].status = 0;
            printf("Memory deallocated for job %d.\n", job_id);
            break;
        }
    }
}

// 时间片轮转调度算法
void round_robin_scheduling() {
    int i, time = 0;
    int job_index = 0;
    while (running_jobs > 0) {
        if (job_queue[job_index].status == 1) {
            printf("Running job %d.\n", job_queue[job_index].job_id);
            job_queue[job_index].size -= TIME_QUANTUM;
            time += TIME_QUANTUM;
            if (job_queue[job_index].size <= 0) {
                job_queue[job_index].status = 0;
                running_jobs--;
                printf("Job %d completed.\n", job_queue[job_index].job_id);
            }
        }
        job_index = (job_index + 1) % job_count;
    }
    printf("\nAll jobs completed.\n");
}

int main() {
    int i;

    // 初始化内存分区
    memory[0].start_address = 0;
    memory[0].size = MEMORY_SIZE;

    // 创建作业队列
    JobControlBlock job1 = create_job(1, 100);
    JobControlBlock job2 = create_job(2, 200);
    JobControlBlock job3 = create_job(3, 150);
    JobControlBlock job4 = create_job(4, 250);
    JobControlBlock job5 = create_job(5, 180);
    JobControlBlock job6 = create_job(6, 120);
    JobControlBlock job7 = create_job(7, 80);
    JobControlBlock job8 = create_job(8, 300);
    JobControlBlock job9 = create_job(9, 220);
    JobControlBlock job10 = create_job(10, 190);
    JobControlBlock job11 = create_job(11, 130);
    JobControlBlock job12 = create_job(12, 350);
    JobControlBlock job13 = create_job(13, 140);
    JobControlBlock job14 = create_job(14, 170);
    JobControlBlock job15 = create_job(15, 240);
    JobControlBlock job16 = create_job(16, 110);
    JobControlBlock job17 = create_job(17, 260);
    JobControlBlock job18 = create_job(18, 210);
    JobControlBlock job19 = create_job(19, 160);
    JobControlBlock job20 = create_job(20, 280);

    // 添加作业到队列
    add_job_to_queue(job1);
    add_job_to_queue(job2);
    add_job_to_queue(job3);
    add_job_to_queue(job4);
    add_job_to_queue(job5);
    add_job_to_queue(job6);
    add_job_to_queue(job7);
    add_job_to_queue(job8);
    add_job_to_queue(job9);
    add_job_to_queue(job10);
    add_job_to_queue(job11);
    add_job_to_queue(job12);
    add_job_to_queue(job13);
    add_job_to_queue(job14);
    add_job_to_queue(job15);
    add_job_to_queue(job16);
    add_job_to_queue(job17);
    add_job_to_queue(job18);
    add_job_to_queue(job19);
    add_job_to_queue(job20);

    // 执行内存分配
    allocate_memory_FCFS();

    // 打印初始内存状态图
    print_memory_status();

    // 执行时间片轮转调度
    round_robin_scheduling();

    // 回收内存
    for (i = 0; i < job_count; i++) {
        deallocate_memory(job_queue[i].job_id);
    }

    // 打印最终内存状态图
    print_memory_status();

    return 0;
}

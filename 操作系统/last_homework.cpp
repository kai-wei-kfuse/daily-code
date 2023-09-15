#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct menorySpace {
    int num;    // 分区号
    int start;  // 起始地址
    int size;   // 分区大小
    int state;
    int wordname;
    struct menorySpace* next;
} menorySpace;

typedef struct Word {
    int wordname;     // 作业名称
    int run_time;     // 作业运行时间
    char wordstate;   // 作业状态
    int papernumber;  // 作业请求空间
    int size;         // 作业请求空间
    struct Word* next;
} Word;

enum ProcessState {
    Process_wait = 'W',
    Process_run = 'R',
    Process_block = 'B',
    Process_finish = 'F'
};
typedef struct process {  // PCB
    int id;
    int Max[3];
    int Need[3];
    int Allocate[3];
    int run_time;
    char Process_state;
    process* next;

} process;
char Name[3] = {'A', 'B', 'C'};
int Available[3] = {10, 15, 8};
int Request[50];               // 请求资源数组
int Work[50];                  // 工作数组
bool Finish[100];              // 标记数组
int Security_queue[50];        // 安全序列
int Process_run_number = 5;    // 进程数
int Resources_number = 3;      // 资源数
int Process_wait_number = 10;  // 进程数
bool flag;                     // 标记是否存在安全序列
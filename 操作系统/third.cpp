#include <stdio.h>
#include <stdlib.h>

#define TIME_SLICE 2

// 进程状态：就绪 等待 完成
enum process_type{
    process_type_wait = 'W',
    process_type_run = 'R',
    process_type_finish = 'F'
};

// 进程控制块结构体
typedef struct PCB_Type{
    // 进程的名字
    char *name;
    // 进程到达时间
    int arrive_time; 
    // 进程响应比
    float priority; 
    // 仍需运行时间
    int stillneed_time;
    // 进程的状态：就绪 等待 完成
    char state;
    // 时间片
    int time_slice;
    // cpu时间 ->已运行时间
    int cpu_time;
    // 计数器
    int time_count;
    // 服务时间
    int serve_time; 
    // 指向下一个要执行的进程
    struct PCB_Type *next;
}PCB;

// 函数声明
// 菜单
int menu(); 
// 创建进程队列 
void create_list(PCB * running_list,int p_number); 
// 创建新的进程
void create_process(PCB *running_list,char *name,int arrive_time,int stillneed_time);
// 将新进程放入就绪队列中
void insert(PCB *running_list,PCB *newProcess);
// 找到当前队列中第一个进程，将它状态变为运行 
void set_run(PCB *running_list);
// 展示当前就绪队列状态
void show(PCB *running_list); 
// 调用调度算法
int invoke_algorithm(PCB *running_list,int number);
// 短进程优先调度算法
int shortest_process_first(PCB *running_list);
// 时间片轮转调度算法
int round_robin(PCB *running_list);
// 高响应比优先调度算法
int highest_priority(PCB *running_list);

int main(int argc, const char * argv[]) {
        
    // 调用菜单 ——返回操作对应编号 
    int number = menu();
    
    // 创建进程数目 
    int p_number=50;
    
    // 运行(就绪)队列(头结点不储存信息)
    PCB *running_list = (PCB *)malloc(sizeof(PCB));
    running_list->next = NULL;
    
    // 创建进程队列 
    create_list(running_list,p_number); 
    
    // 调用调度算法 
    int total_cometofinish_time = invoke_algorithm(running_list,number);
    printf("该进程调度算法的进程平均周转时间：%0.2f\n",(float)total_cometofinish_time/p_number);
    
    return 0;
}
// 菜单
int menu(){

    // 选用进程调度算法
    printf("请选用进程调度算法：\n\n");
    printf("    1.短进程优先调度算法\n");
    printf("    2.时间片轮转调度算法\n");
    printf("    3.高响应比优先调度算法\n\n");
    printf("请输入选择（输入编号）：");
    
    int number;
    scanf("%d",&number); 
    printf("\n");
    
    return number; 
} 

// 创建进程队列 
void create_list(PCB * running_list,int p_number){
    
    // 循环创建进程 
    for (int i = 1; i <= p_number; i++) {   
        // 进程名 
        char *name = (char *)malloc(sizeof(char));
        name[0] = (char)(i+'0');
        if(i == p_number){
            printf("\n");
        }
         
        // 进程到达时间
        int arrive_time; 
        if(1 == i){
            // 第一个进程到达时间为0
            arrive_time = 0; 
        }else{
            // 其它进程随机产生到达时间 (0~20) 
            arrive_time = rand()%11;    
        }
        
        // 仍需运行时间
        int stillneed_time;
        // 随机产生进程执行所需时间 (1~10) 
        stillneed_time = 1 + rand()%10;   
        
        create_process(running_list,name,arrive_time,stillneed_time);
    }
}  

// 创建新的进程
void create_process(PCB *running_list,char *name,int arrive_time,int stillneed_time){
    
    // 申请一个内存控制块的空间
    PCB *p = (PCB *)malloc(sizeof(PCB));
    // 动态内存是否分配成功 
    if(p == NULL){
        printf("内存分配失败！");
    }
    
    // 设置该控制块的值
    p->name = name;
    // 到达时间 
    p->arrive_time = arrive_time; 
    // 仍需运行时间 
    p->stillneed_time = stillneed_time;
    // 总需用时
    p->serve_time = stillneed_time;
    
    // 响应比 
    p->priority = 1;
    // 状态
    p->state = process_type_wait;
    // 时间片
    p->time_slice = 0;
    // cpu时间
    p->cpu_time = 0;
    // 计数器
    p->time_count = 0;
    // 下个进程
    p->next = NULL;
    
    // 放入就绪队列中
    insert(running_list,p);
}

// 展示当前队列状态
void show(PCB *running_list){
    PCB *p = running_list->next;
    if (p == NULL) {
        printf("当前队列中无进程\n");
        return;
    }
    
    printf("进程名  到达时间  响应比  时间片  cpu时间  仍需时间  进程状态  计数器\n");
    while (p != NULL) {
printf("   %s     %4d      %0.2f   %4d    %4d     %4d          %c     %4d\n",p->name,p->arrive_time,p->priority,p->time_slice,p->cpu_time,p->stillneed_time,p->state,p->time_count);
        
        p = p->next;
    }
    printf("\n");
}

// 调用调度算法 
int invoke_algorithm(PCB *running_list,int number){
    
    int time = 0;
    switch(number){
        case 1:
            // 短进程优先调度算法 
            printf("进程调度前:\n");
            printf("---------------------------------------------------------------------\n");
            show(running_list);
            printf("---------------------------------------------------------------------\n");
            printf("进程调度后:\n");
            time = shortest_process_first(running_list);
            break;
        case 2:
            // 时间片轮转调度算法 
            set_run(running_list);
            printf("进程调度前:\n");
            printf("---------------------------------------------------------------------\n");
            show(running_list);
            printf("---------------------------------------------------------------------\n");
            printf("进程调度后:\n");
            time = round_robin(running_list);
            break;
        case 3:
            // 高响应比优先调度算法 
            printf("进程调度前:\n");
            printf("---------------------------------------------------------------------\n");
            show(running_list);
            printf("---------------------------------------------------------------------\n");
            printf("进程调度后:\n");
            time = highest_priority(running_list);
            break;
        default:
            exit(0);
            break;
    }
    return time;
} 

// 插入新进程 
void insert(PCB *running_list,PCB *newProcess){
    
    // 判断进程队列是否为空 
    if(running_list->next == NULL){
        running_list->next = newProcess;
        return;
    }
    
    // 新进程到达时间 
    int arrive_time = newProcess->arrive_time;
    // 移动指针
    PCB *move = running_list->next;
    while(move->next != NULL){
        if(move->next->arrive_time > arrive_time){
            newProcess->next = move->next;
            move->next = newProcess;
            return;
        } 
        move = move->next;
    } 
    move->next = newProcess;
}

// 找到当前队列中第一个进程，将它状态变为运行 
void set_run(PCB *running_list){
    PCB *s = running_list->next;
    if (s == NULL) {
        printf("当前队列已空\n");
        return;
    }
    
    if (s->state != process_type_finish) {
        s->state = process_type_run;
        return;
    }
}

// 短进程优先调度算法
int shortest_process_first(PCB *running_list){
    /*
        在已到达进程中找服务时间最短的进程，将进程状态更改为R并执行，
        如果所有已到达进程已执行完毕，则等待新进程到达，等到进程到达，
        如果进程完成则将该进程状态更改为F并调整到队列尾部 
     */
    // 记录第一个结点的位置
    PCB *shortest = running_list->next;
    
    // 判断进程是否到达，进程调度已过多长时间  
    int total_run_time = 0;
    // 记录算法的进程总周转时间 
    int total_cometofinish_time = 0; 
    
    // index定位队列的第一个进程 
    PCB *index = shortest;
    PCB *p = shortest;
    while(index != NULL && index->state != process_type_finish){// 进程是否到达
        // 记录第一个结点的服务时间
        int shortest_serve_time = index->serve_time; 
         
        // 在已到达进程中找服务时间最短的进程
        while(p != NULL){ 
            if (p->arrive_time <= total_run_time && p->serve_time < shortest_serve_time && p->state != process_type_finish) {
                shortest = p;
            }
            p = p->next;
        }
    
        // 如果上面已找出服务时间最短的进程，判断该进程是否到达
        // 排除没有进程到达的情况
        if (shortest->arrive_time > total_run_time) {
            // 当前无新进程到达 
            // 当前进程全部执行完，等待下一进程到达
            printf("当前进程调度执行时间：%d\n",total_run_time);
            printf("当前没有新进程到达，等待新进程到达...\n");
            printf("等待时间：%d\n",running_list->next->arrive_time - total_run_time);
            // 更新进程调度总运行时间
            total_run_time += running_list->next->arrive_time - total_run_time; 
            printf("当前进程调度执行时间：%d\n",total_run_time);
        } else {
            // 运行当前就绪队列中服务时间最短的进程
            shortest->state = process_type_run;
            
            printf("---------------------------------------------------------------------\n");
            show(running_list);
            printf("---------------------------------------------------------------------\n");
            
            // 仍需运行时间
            shortest->stillneed_time = 0;
            // cpu时间 ->已运行时间
            shortest->cpu_time = shortest->serve_time;
            // 进程状态更改为完成 
            shortest->state = process_type_finish;
            
            // 计算进程调度总运行时间 
            total_run_time += shortest->serve_time;
            // 记录算法的进程总周转时间 
            total_cometofinish_time += total_run_time - shortest->arrive_time; 
            
            // 将运行完的服务时间最短的进程调整到队尾
            if(running_list->next == shortest){
                // 如果运行完的进程为队列的第一个进程 
                // 定位最后一个进程 
                PCB *move = running_list->next;
                while(move->next != NULL){
                    move = move->next;
                }
                move->next = shortest;
                running_list->next = shortest->next;
                shortest->next = NULL;
            }else{
                // 定位服务时间最短的进程的前一个进程
                PCB *move1 = index; 
                // 定位最后一个进程 
                PCB *move2 = index;
                while(move1->next != shortest){
                    move1 = move1->next;
                }
                while(move2->next != NULL){
                    move2 = move2->next;
                }
                move1->next = shortest->next;
                move2->next = shortest;
                shortest->next = NULL;      
            }
            
            // 展示当前队列状况
            printf("进程完成\n");
            printf("当前进程调度执行时间：%d\n",total_run_time);
            printf("当前进程调度算法的总周转时间：%d\n",total_cometofinish_time); 
            printf("---------------------------------------------------------------------\n");
            show(running_list);
            printf("---------------------------------------------------------------------\n");
        } 
        shortest = running_list->next;
        index = shortest;
        p = shortest;
    }
    return total_cometofinish_time;
} 

// 时间片轮转调度算法 
int round_robin(PCB *running_list){
    /*
    每次运行完进程后：
        如果进程运行完毕，会将该进程从队首调到队尾 
        如果进程运行完毕且下一进程到达时间大于进程运行总时间，即当前进程全部执行完，等待下一进程到达 
        如果进程运行未完毕，将该进程调整到未完成队列的末尾
     */
    // 记录第一个结点的位置
    PCB *index = running_list->next;
    
    // 判断进程是否到达，进程调度已过多长时间  
    int total_run_time = 0;
    // 记录算法的进程总周转时间 
    int total_cometofinish_time = 0; 
    PCB *s = NULL; 
    while (index != NULL && index->state != process_type_finish) {
        // 进程是否到达 
        if(index -> state == process_type_run){
            // 按时间片运行该进程，即修改进程的cpu时间和仍需要时间、计数器
            s = index;
            s->time_slice = TIME_SLICE;
            
            // 记录最初cpu时间 
            int old_cpu_time = s->cpu_time; 
            // cpu时间（已运行时间) = 总需时间 -（当前cpu时间+时间片)
            // 若已完成则直接显示总需时间
            s->cpu_time = (s->cpu_time + TIME_SLICE)<s->serve_time ? s->cpu_time + TIME_SLICE : s->serve_time;
            // 若当前仍需时间减时间片小于等于零，则说明进程已完成
            s->stillneed_time = (s->stillneed_time - TIME_SLICE)>0 ? s->stillneed_time - TIME_SLICE : 0;
            // 计数器+1
            s->time_count += 1;
            
            // 计算进程调度总运行时间 
            if (s->cpu_time == s->serve_time){
                total_run_time += (s->serve_time - old_cpu_time);
            }else{
                total_run_time += TIME_SLICE;
            }
            
            // 判断该进程是否结束
            if (s->stillneed_time == 0) {
                // 修改进程状态
                s->state = process_type_finish;
                printf("进程完成\n");
                printf("当前进程调度总运行时间：%d\n",total_run_time); 
                total_cometofinish_time += (total_run_time - s->arrive_time);
                printf("当前进程调度算法的总周转时间：%d\n",total_cometofinish_time); 
            }else{
                s->state = process_type_wait;
            }
            
            // 当前进程已执行完
            if (s->state == process_type_finish){
                // 将已完成进程调整到已完成队列末尾 
                // 1.头指针指向首元结点的下一个结点
                running_list->next = s->next;
                
                // 2.遍历整个链表，将其插入到已完成队列的最尾端
                PCB *p = running_list;
                while (p->next != NULL) {
                    p = p->next;
                }
                p->next = s;
                s->next = NULL;
                    
                // 进程调度总运行时间小于下一进程到达时间
                if (total_run_time < running_list->next->arrive_time){
                    // 当前进程全部执行完，等待下一进程到达
                    printf("当前进程调度执行时间：%d\n",total_run_time);
                    printf("当前没有新进程到达，等待新进程到达...\n");
                    printf("等待时间：%d\n",running_list->next->arrive_time-total_run_time);
                    // 更新进程调度总运行时间
                    total_run_time += running_list->next->arrive_time-total_run_time; 
                    printf("当前进程调度执行时间：%d\n",total_run_time);
                } 
            } else {
                // 当前进程队列有其他进程排队 
                // 1.头指针指向首元结点的下一个结点
                running_list->next = s->next;
                
                // 2.遍历整个链表，将其插入到未完成队列的最尾端(其后是已完成的队列)
                PCB *p = running_list;
                // 2.1寻找插入位置
                while (p->next != NULL && p->next->arrive_time <= total_run_time && p->next->state != process_type_finish) {
                    p = p->next;
                }
                // 2.2判断插入位置
                if (p->next == NULL) {
                    // 最后一个
                    p->next = s;
                    p->next->next = NULL;
                }else{
                    // 不是最后一个，其后仍有结点
                    s->next = p->next;
                    p->next = s;
                }
            }
        }
        index = running_list->next;
        
        if(total_run_time >= index->arrive_time){
            set_run(running_list);
        }
        
        // 展示当前队列状况
        printf("---------------------------------------------------------------------\n");
        show(running_list);
        printf("---------------------------------------------------------------------\n");
    }
    return total_cometofinish_time;
}

// 高响应比优先调度算法
int highest_priority(PCB *running_list){
    /*
        在已到达进程中找当前响应比最高的进程，将进程状态更改为R并执行，
        如果所有已到达进程已执行完毕，则等待新进程到达，等到进程到达，
        如果进程完成则将该进程状态更改为F并调整到队列尾部 
     */
    // 记录第一个结点的位置
    PCB *highest = running_list->next;
    
    // 判断进程是否到达，进程调度已过多长时间  
    int total_run_time = 0;
    // 记录算法的进程总周转时间 
    int total_cometofinish_time = 0; 
    
    // index定位队列的第一个进程 
    PCB *index = highest;
    PCB *p = highest;
    while(index != NULL && index->state != process_type_finish){
        // 更新进程的响应比
        PCB *update = index;
        while(update != NULL && update->arrive_time <= total_run_time && update->state != process_type_finish){
            update->priority = ((float)total_run_time - (float)update->arrive_time + (float)update->serve_time)/update->serve_time; 
            update = update->next;
        }
        
        // 记录第一个进程的当前响应比 
        float highest_priority = index->priority;
         
        // 在已到达进程中找当前响应比最高的进程
        while(p != NULL){ 
            if (p->arrive_time <= total_run_time && highest_priority < p->priority && p->state != process_type_finish) {
                highest = p;
            }
            p = p->next;
        }
    
        // 如果上面已找出当前响应比最高的进程，判断该进程是否到达
        // 排除没有进程到达的情况
        if (highest->arrive_time > total_run_time) {
            // 当前无新进程到达 
            // 当前进程全部执行完，等待下一进程到达
            printf("当前进程调度执行时间：%d\n",total_run_time);
            printf("当前没有新进程到达，等待新进程到达...\n");
            printf("等待时间：%d\n",running_list->next->arrive_time - total_run_time);
            // 更新进程调度总运行时间
            total_run_time += running_list->next->arrive_time - total_run_time; 
            printf("当前进程调度执行时间：%d\n",total_run_time);
        } else {
            // 运行当前就绪队列中当前响应比最高的进程
            highest->state = process_type_run;
            
            printf("---------------------------------------------------------------------\n");
            show(running_list);
            printf("---------------------------------------------------------------------\n");
            
            // 仍需运行时间
            highest->stillneed_time = 0;
            // cpu时间 ->已运行时间
            highest->cpu_time = highest->serve_time;
            // 进程状态更改为完成 
            highest->state = process_type_finish;
            
            // 计算进程调度总运行时间 
            total_run_time += highest->serve_time;
            // 记录算法的进程总周转时间 
            total_cometofinish_time += total_run_time - highest->arrive_time; 
            
            // 将运行完的当前响应比最高的进程调整到队尾
            if(running_list->next == highest){
                // 如果运行完的进程为队列的第一个进程 
                // 定位最后一个进程 
                PCB *move = running_list->next;
                while(move->next != NULL){
                    move = move->next;
                }
                move->next = highest;
                running_list->next = highest->next;
                highest->next = NULL;
            }else{
                // 定位当前响应比最高的进程的前一个进程
                PCB *move1 = index; 
                // 定位最后一个进程 
                PCB *move2 = index;
                while(move1->next != highest){
                    move1 = move1->next;
                }
                while(move2->next != NULL){
                    move2 = move2->next;
                }
                move1->next = highest->next;
                move2->next = highest;
                highest->next = NULL;       
            }
            
            // 展示当前队列状况
            printf("进程完成\n");
            printf("当前进程调度执行时间：%d\n",total_run_time);
            printf("当前进程调度算法的总周转时间：%d\n",total_cometofinish_time); 
            printf("---------------------------------------------------------------------\n");
            show(running_list);
            printf("---------------------------------------------------------------------\n");
        } 
        highest = running_list->next;
        index = highest;
        p = highest;
    }
    return total_cometofinish_time; 
} 


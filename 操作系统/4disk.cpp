#include <stdio.h>
#include <stdlib.h>

#define START 100

// 磁盘范围 [0,end]
int end;
// 存储随机生成10个磁道请求
int request[15];
// 存储各算法的磁头移动顺序
int order[15];
// 标记 -- 被访问过：1，未被访问：0，此时全为0
int sign[15];
// 磁道距离
int dis[15];
// 函数声明
// 菜单
void menu();
// 随机生成10个磁道请求
void create();
// 先来先服务算法
void FCFS();
// 最短寻道时间优先算法
void SSTF();
// 扫描算法
void SCAN();
// 循环扫描算法
void CSCAN();
// 确定磁头移动的方向
int direction();
// 展示算法寻道顺序和平均移动磁道数
void show();
int main() {
    printf(
        "\n ---------------------当前磁头所在磁道：%d---------------------\n",
        START);

    // 随机生成10个磁道请求
    create();

    // 菜单
    menu();
}
// 随机生成10个磁道请求
void create() {
    // 产生磁盘寻道范围
    printf("\n\t磁盘寻道范围从0开始，最大不超过256");
    printf("\n\t由于当前磁头位于100，磁盘寻道范围应大于或等于100");
    printf("\n\t请输入符号条件的磁盘寻道范围：");
    // int end;
    scanf("%d", &end);
    while (end < 100 || end > 256) {
        printf("\n\t你输入的磁盘寻道范围不符合条件");
        printf("\n\t请重新输入符号条件的磁盘寻道范围：");
        scanf("%d", &end);
    }
    printf("\n\t磁盘寻道范围确定成功！\n");

    // 随机生成10个磁道请求
    for (int i = 1; i <= 10; ++i) {
        request[i] = rand() % (end + 1);
        // 排除生成重复的磁道请求
        for (int j = i - 1; j >= 1; --j) {
            if (request[i] == request[j]) {
                --i;
                break;
            }
        }
    }
    printf("\n\t已随机产生10个磁道请求！\n\n");
    printf("\t磁道请求：");
    for (int i = 1; i <= 10; ++i) {
        printf("%d ", request[i]);
    }
    printf(
        "\n\n----------------------------------------------------------------"
        "\n");
}
// 菜单
void menu() {
    while (true) {
        printf("\n\t请选择使用哪种磁盘调度算法：\n\n");
        printf("\t\t1.先来先服务算法(FCFS)\n");
        printf("\t\t2.短寻道时间优先算法(SSTF)\n");
        printf("\t\t3.扫描算法(SCAN)\n");
        printf("\t\t4.循环扫描算法(CSCAN)\n");
        printf("\t\t5.退出\n\n");
        printf("\t请输入有效操作编号：");

        int option;
        scanf("%d", &option);
        printf("\n");

        // 根据输入操作编号进行相应操作
        switch (option) {
            case 1:
                // 先来先服务算法
                FCFS();
                show();
                break;
            case 2:
                // 最短寻道时间优先算法
                SSTF();
                show();
                break;
            case 3:
                // 扫描算法
                SCAN();
                show();
                break;
            case 4:
                // 循环扫描算法
                CSCAN();
                show();
                break;
            case 5:
                printf(
                    "---------------------------退出成功！---------------------"
                    "------\n");
                return;
                break;
            default:
                printf(
                    "-------------输入操作编号错误，请输入有效操作编号！-------"
                    "------\n");
                break;
        }
    }
}
// 先来先服务算法
void FCFS() {
    // 磁道的请求顺序就是磁头的移动顺序
    for (int i = 1; i <= 10; ++i) {
        order[i] = request[i];
        if (i == 1) {
            dis[i] = abs(START - request[i]);
        } else {
            dis[i] = abs(request[i - 1] - request[i]);
        }
    }
}
// 最短寻道时间优先算法
void SSTF() {
    // 定位当前磁道所属下标
    int nowIndex = 0;
    // 定位下一磁道所属下标
    int nextIndex = 1;
    // 存储最短距离
    int s;
    // 找出当前磁头所在磁道距离下一磁道的最短距离对应的磁道
    for (int i = 1; i <= 10; ++i) {
        s = 200;
        if (i == 1) {
            for (int j = 1; j <= 10; ++j) {
                // 开始的磁头到该磁道的距离小于最小距离时
                if (abs(START - request[j]) < s) {
                    s = abs(START - request[j]);
                    nextIndex = j;
                }
            }
        } else {
            for (int j = 1; j <= 10; ++j) {
                // 当该磁道未被访问且当前磁头到该磁道的距离小于最小距离时
                if (sign[j] == 0 && abs(request[nowIndex] - request[j]) < s) {
                    s = abs(request[nowIndex] - request[j]);
                    nextIndex = j;
                }
            }
        }
        // 更新磁道访问顺序数组
        order[i] = request[nextIndex];
        // 更新距离数组
        dis[i] = s;
        // 将该磁道设置为已访问
        sign[nextIndex] = 1;
        // 移动磁头
        nowIndex = nextIndex;
    }
}
// 扫描算法
void SCAN() {
    // 磁头在磁盘来回移动，先从START开始从外向内
    // 磁头开始的磁道
    int start;
    // 确定更改顺序数组的位置
    int index = 1;
    // 磁头移动方向从外向内，开始时为80到100
    // 磁道内大外小，最外为磁道0
    for (start = START; start <= end; ++start) {
        // 遍历请求数组
        // 每次比较请求数组的所有磁道
        // 如果相等则将其加入order数组并更改标记
        for (int i = 1; i <= 10 && index <= 10; i++) {
            if (request[i] == start && sign[i] == 0) {
                order[index] = request[i];
                sign[i] = 1;
                // 更新距离数组
                if (index == 1) {
                    dis[index] = abs(START - order[index]);
                } else {
                    dis[index] = abs(order[index - 1] - order[index]);
                }
                if (start != end) {
                    index++;
                }
            }
        }
    }
    bool change = true;
    // 磁头移动方向变为从内向外
    for (start = end; start >= 0; --start) {
        // 遍历请求数组
        // 每次比较请求数组的所有磁道
        // 如果相等则将其加入order数组并更改标记
        for (int i = 1; i <= 10 && index <= 10; i++) {
            if (request[i] == start && sign[i] == 0) {
                order[index] = request[i];
                sign[i] = 1;
                // 更新距离数组
                if (change) {
                    dis[index] = 2 * end - order[index - 1] - order[index];
                    change = false;
                } else {
                    dis[index] = abs(order[index - 1] - order[index]);
                }
                index++;
            }
        }
    }
}
// 循环扫描算法
void CSCAN() {
    // 磁头在磁盘来回移动，先从START开始从外向内
    // 磁头开始的磁道
    int start;
    // 确定更改顺序数组的位置
    int index = 1;
    // 磁头移动方向从外向内，开始时为80到100
    // 磁道内大外小，最外为磁道0
    for (start = START; start <= end; ++start) {
        // 遍历请求数组
        // 每次比较请求数组的所有磁道
        // 如果相等则将其加入order数组并更改标记
        for (int i = 1; i <= 10 && index <= 10; i++) {
            if (request[i] == start && sign[i] == 0) {
                order[index] = request[i];
                sign[i] = 1;
                // 更新距离数组
                if (index == 1) {
                    dis[index] = abs(START - order[index]);
                } else {
                    dis[index] = abs(order[index - 1] - order[index]);
                }
                if (start != end) {
                    index++;
                }
            }
        }
    }
    bool change = true;
    // 磁头移动方向仍然从外向内，从0磁道重新开始扫描
    for (start = 0; start <= START - 1; ++start) {
        // 遍历请求数组
        // 每次比较请求数组的所有磁道
        // 如果相等则将其加入order数组并更改标记
        for (int i = 1; i <= 10 && index <= 10; i++) {
            if (request[i] == start && sign[i] == 0) {
                order[index] = request[i];
                sign[i] = 1;
                // 更新距离数组
                if (change) {
                    dis[index] = order[index];
                    change = false;
                } else {
                    dis[index] = abs(order[index - 1] - order[index]);
                }
                index++;
            }
        }
    }
}
// 展示算法寻道顺序和平均移动磁道数
void show() {
    // 总移动磁道数
    int sum_dis = 0;

    printf("\t开始的磁道：%d\n", START);
    printf("\t    被访问的下一磁道\t磁头需移动距离\n");
    for (int i = 1; i <= 10; ++i) {
        printf("\t\t%4d\t\t", order[i]);
        printf("%8d\n", dis[i]);
        sum_dis += dis[i];
    }
    printf("\n\t该磁盘调度算法下磁头的平均移动磁道数：%0.2f\n\n",
           (float)sum_dis / 10);
    printf(
        "----------------------------------------------------------------\n");
    // 将标记数组全部置0
    for (int i = 1; i <= 10; ++i) {
        sign[i] = 0;
    }
}

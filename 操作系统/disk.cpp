#include <stdio.h>
#include <stdlib.h>

#define START 100

int end = 256;
int request[100];
int order[4][100];
int sign[4][100];
int dis[4][100];
int NUM = 0;
int dissum[4] = {0};

void create(int number) {
    for (int i = 1; i <= number; ++i) {
        request[i] = rand() % (end + 1);
        for (int j = i - 1; j >= 1; --j) {
            if (request[i] == request[j]) {
                --i;
                break;
            }
        }
    }
}

void SSTF(int number) {
    int nowIndex = 0;
    int nextIndex = 1;
    int s;

    for (int i = 1; i <= number; ++i) {
        s = 200;
        if (i == 1) {
            for (int j = 1; j <= number; ++j) {
                if (abs(START - request[j]) < s) {
                    s = abs(START - request[j]);
                    nextIndex = j;
                }
            }
        } else {
            for (int j = 1; j <= number; ++j) {
                if (sign[0][j] == 0 &&
                    abs(request[nowIndex] - request[j]) < s) {
                    s = abs(request[nowIndex] - request[j]);
                    nextIndex = j;
                }
            }
        }
        order[0][i] = request[nextIndex];
        dis[0][i] = s;
        sign[0][nextIndex] = 1;
        nowIndex = nextIndex;
    }
}

void SCAN(int number) {
    int start;
    int index = 1;

    for (start = START; start <= end; ++start) {
        for (int i = 1; i <= number && index <= 10; i++) {
            if (request[i] == start && sign[1][i] == 0) {
                order[1][index] = request[i];
                sign[1][i] = 1;
                if (index == 1) {
                    dis[1][index] = abs(START - order[1][index]);
                } else {
                    dis[1][index] = abs(order[1][index - 1] - order[1][index]);
                }
                if (start != end) {
                    index++;
                }
            }
        }
    }

    bool change = true;

    for (start = end; start >= 0; --start) {
        for (int i = 1; i <= number && index <= number; i++) {
            if (request[i] == start && sign[1][i] == 0) {
                order[1][index] = request[i];
                sign[1][i] = 1;
                if (change) {
                    dis[1][index] =
                        2 * end - order[1][index - 1] - order[1][index];
                    change = false;
                } else {
                    dis[1][index] = abs(order[1][index - 1] - order[1][index]);
                }
                index++;
            }
        }
    }
}

void CSCAN(int number) {
    int start;
    int index = 1;

    for (start = START; start <= end; ++start) {
        for (int i = 1; i <= number && index <= 10; i++) {
            if (request[i] == start && sign[2][i] == 0) {
                order[2][index] = request[i];
                sign[2][i] = 1;
                if (index == 1) {
                    dis[2][index] = abs(START - order[2][index]);
                } else {
                    dis[2][index] = abs(order[2][index - 1] - order[2][index]);
                }
                if (start != end) {
                    index++;
                }
            }
        }
    }

    order[2][index] = end;
    dis[2][index] = abs(order[2][index - 1] - order[2][index]);

    for (start = 0; start <= end; ++start) {
        for (int i = 1; i <= number && index <= number; i++) {
            if (request[i] == start && sign[2][i] == 0) {
                order[2][index] = request[i];
                sign[2][i] = 1;
                dis[2][index] = abs(order[2][index - 1] - order[2][index]);
                index++;
            }
        }
    }
}

void FCFS(int number) {
    for (int i = 1; i <= number; ++i) {
        order[3][i] = request[i];
        sign[3][i] = 1;
        if (i == 1) {
            dis[3][i] = abs(START - order[3][i]);
        } else {
            dis[3][i] = abs(order[3][i] - order[3][i - 1]);
        }
    }
}

void show(int number) {
    int dissum = 0;

    printf("要访问的磁道号：");
    for (int i = 1; i <= number; i++)
        printf("%d,", request[i]);
    printf("\n");
    printf("开始的磁道：%d\n\n", START);

    for (int j = 0; j < 4; j++) {
        printf("被访问的下一磁道：");
        for (int i = 1; i <= number; ++i) {
            printf("%d,", order[j][i]);
        }
        printf("\n");
        printf("磁头需移动距离：");
        for (int i = 1; i <= number; ++i) {
            printf("%d,", dis[j][i]);
            dissum += dis[j][i];
        }
        printf("\n");
        if (j == 0)
            printf("SSTF下磁头的平均移动磁道数：%0.2f\n\n",
                   (float)dissum / number);
        if (j == 1)
            printf("SCAN下磁头的平均移动磁道数：%0.2f\n\n",
                   (float)dissum / number);
        if (j == 2)
            printf("CSCAN下磁头的平均移动磁道数：%0.2f\n\n",
                   (float)dissum / number);
        if (j == 3)
            printf("FCFS下磁头的平均移动磁道数：%0.2f\n\n",
                   (float)dissum / number);
        dissum = 0;
    }
    for (int j = 0; j < 4; j++) {
        for (int i = 1; i <= 10; ++i) {
            sign[j][i] = 0;
        }
    }
}

void show2(int number) {
    for (int z = 1; z <= number; z++) {
        NUM++;
        system("cls");

        printf("要访问的磁道号：");
        for (int i = 1; i <= number; i++)
            printf("%d,", request[i]);
        printf("\n");
        printf("开始的磁道：%d\n\n", START);

        for (int j = 0; j < 4; j++) {
            printf("被访问的下一磁道：");
            printf("%d,", order[j][z]);
            printf("\n");
            printf("磁头需移动距离：");
            printf("%d,", dis[j][z]);
            dissum[j] += dis[j][z];
            printf("\n");
            if (j == 0)
                printf("SSTF下磁头的平均移动磁道数：%0.2f\n\n",
                       (float)dissum[j] / NUM);
            if (j == 1)
                printf("SCAN下磁头的平均移动磁道数：%0.2f\n\n",
                       (float)dissum[j] / NUM);
            if (j == 2)
                printf("CSCAN下磁头的平均移动磁道数：%0.2f\n\n",
                       (float)dissum[j] / NUM);
            if (j == 3)
                printf("FCFS下磁头的平均移动磁道数：%0.2f\n\n",
                       (float)dissum[j] / NUM);
        }

        system("pause");
    }

    for (int j = 0; j < 4; j++) {
        for (int i = 1; i <= number; ++i) {
            sign[j][i] = 0;
            dis[j][i];
            order[j][i];
        }
    }
}

int main() {
    int number = 10;

    for (int i = 0; i < 10; i++) {
        create(number);
        SSTF(number);
        SCAN(number);
        CSCAN(number);
        FCFS(number);
        show2(number);
    }

    return 0;
}

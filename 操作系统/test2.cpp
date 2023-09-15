#include <stdio.h>
#include <time.h>
 
struct sjf
{
    char name[10]; //进程号
    float arr; //提交时间
    float ing; //运行时间
    float ks; //开始
    float jieshu; //完成
    float zhouz; //周转时间
    float Dzhouz; //带权周转时间
};
 
void input(struct sjf *p,int N)//输入进程相关信息
{
    printf("请输入进程号、提交时间、运行时间：\n");
    printf("\n");
    for(int i=0; i<N; i++)
    {
        scanf("%s",p[i].name);
        scanf("%f",&p[i].arr);
        scanf("%f",&p[i].ing);
    }
}
 
void sort_arr(struct sjf *p,int N) //对所有进程按照提交时间进行排序
{
    int i,j;
    struct sjf temp;
    for(i=0; i<N-1; i++)
    {
        for(j=0; j<N-i-1; j++)
        {
            if(p[j].arr>p[j+1].arr)
            {
                temp=p[j];
                p[j]=p[j+1];
                p[j+1]=temp;
            }
        }
    }
}
 
void sort_ing(struct sjf *p,int N)//对等待队列中的进程按照运行时间排序
{
    struct sjf temp;
    int k=0;
    float end=0.0;//保存CPU已经运行的时间
    for(int i=0; i<N; i++)
    {
        k=i;//保存当前i的下标
        while(p[i].arr<=end&&i<N)//对从第i个之后的所有进程进行搜索
            i++;//找到最后一个等待进程的位置
        for (int t=k; t<i-1; t++)//对当前正在等待执行的进程按照运行时间排序
        {
            for (int j = t+1; j<i; j++)
            {
                if(p[t].ing < p[j].ing)
                    continue;
                else
                {
                    temp = p[t];
                    p[t] = p[j];
                    p[j] = temp;
                }
            }
        }
        i=k; //将i的下标还原
        end+=p[i].ing;
    }
}
 
void run(struct sjf *p,int N)
{
    float sta = p[0].arr;//下一个进程的开始时间
    for (int i=0; i<N; i++)
    {
        p[i].ks = sta;
        p[i].jieshu = p[i].ks + p[i].ing;
        p[i].zhouz = p[i].jieshu - p[i].arr;
        p[i].Dzhouz = p[i].zhouz / p[i].ing;
        if (i == N-1)//运行到最后一个进程时，已不需要计算它下一个进程的开始时间
            break;
        sta = p[i].jieshu>p[i+1].arr?p[i].jieshu:p[i+1].arr;
        //当进程1的结束时间大于进程2的到达时间，进程2的开始时间为进程1的结束时间，否则为进程2的到达时间
    }
}
 
void show(struct sjf *p,int N)
{
    int i; 
    printf("进程号\t提交时间\t运行时间\t开始\t\t结束\t\t周转时间\t带权周转时间\n");
    for(i=0; i<N; i++)
    {
        printf("%s\t",p[i].name);
        printf("%.2f\t\t",p[i].arr);
        printf("%.2f\t\t",p[i].ing);
        printf("%.2f\t\t",p[i].ks);
        printf("%.2f\t\t",p[i].jieshu);
        printf("%.2f\t\t",p[i].zhouz);
        printf("%.2f\t\t",p[i].Dzhouz);
        printf("\n");
    }
}
 
int main()
{
    int N; //进程数量
 
    struct sjf a[10]; //创建结构体数组
    input(a,N);
    sort_arr(a,N);
    sort_ing(a,N);
    run(a,N);
    show(a,N);
    printf("\n");
 
    int i;
    float average_zhouz=0;
    float average_Dzhouz=0;
    for(i=0; i<N; i++)
    {
        average_zhouz+=a[i].zhouz;
        average_Dzhouz+=a[i].Dzhouz;
    }
    average_zhouz/=N;
    average_Dzhouz/=N;
    printf("采用短进程优先算法算得平均周转时间 = %.2f \n",average_zhouz);
    printf("采用短进程优先算法算得平均带权周转时间 = %.2f \n",average_Dzhouz);
    return 0;
}
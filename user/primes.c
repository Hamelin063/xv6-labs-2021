#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

//0r,1w

int newproc(int p[2]){
    int flag;
    int tmp;
    int prime;
    close(p[1]);
    if(read(p[0],&prime,4)!=4){
        fprintf(2,"failed to read in child\n");
        exit(1);
    }
    printf("prime %d\n",prime);// 最近的这个肯定是当前的prime
    flag=read(p[0],&tmp,4); //读取失败就停止
    if(flag){
        int np[2];
        pipe(np);
        if(fork()==0){
            newproc(np);
        }
        else{
            close(np[0]);
            // 先处理当前多读的这一个数字
            if(tmp%prime){
                if(write(np[1],&tmp,4)!=4){
                    fprintf(2,"failed to write in np\n");
                    exit(1);
                }
            }
            // 再把剩下的数字全读出来 用当前的prime筛一遍
            while(read(p[0],&tmp,4)){
                if(tmp%prime){
                    if(write(np[1],&tmp,4)!=4){
                        fprintf(2,"failed to write in np\n");
                        exit(1);
                    }
                }
            }
            close(p[0]);
            close(np[1]);
            wait(0);
        }
    }
    exit(0);
}
int main(){
    int p[2];
    pipe(p);
    if(fork()==0){
        newproc(p);
    }
    //关于用&i的问题：write函数要求传入一个指向数据的指针
    //&i本身获取的就是i的内存地址，也就是一个指针
    //前文char buf[2]可以直接传buf的原因是buf本身也就是一个指针
    else{
        close(p[0]);//父进程不需要读取，关闭管道的读端
        for(int i=2;i<=35;i++){
            if(write(p[1],&i,4)!=4){
                fprintf(2,"first process failed to write %d to pipe\n",i);
                exit(1);
            }
        }
        close(p[1]);// 将2-35的数字全写入管道中
        wait(0);
        exit(0);
    }
    return 0;
}
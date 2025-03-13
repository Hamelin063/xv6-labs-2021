#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
// 2指代的就是stderr，可以对此进行定义
// 此处用法和linux课程小实验是一样的
int
main(int argc,char *argv[]){
    if(argc!=2){
        fprintf(2,"Usage:sleep seconds\n");
        exit(1);
    }
    int time_to_sleep=atoi(argv[1]);
    sleep(time_to_sleep);
    exit(0);
}
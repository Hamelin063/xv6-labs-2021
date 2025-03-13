#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

char buf[1024];

int lineread(char *nargv[MAXARG],int cargc){
    int idx=0;//已经读取的字节数
    //0代表标准输入，即从标准输入读取一个字节
    while(read(0,buf+idx,1)){
        if(idx==1023){
            fprintf(2,"Too long argument\n");
            exit(1);
        }
        if(buf[idx]=='\n'){
            break;//读到换行符为止
        }
        idx++;
    }
    buf[idx]=0;
    if(idx==0) return 0;//没有读取到任何字符
    int offset=0;
    while(offset<idx){
        nargv[cargc++]=buf+offset;
        while(buf[offset]!=' '&&offset<idx){
            offset++;
        }
        //在 Linux 终端中，参数之间可以相隔一个或多个空格，它们没有影响。
        //命令行解析器会自动忽略额外的空格，并将多个连续的空格视为单个分隔符。
        while(buf[offset]==' '&&offset<idx){
            buf[offset++]=0;
        }
    }
    return cargc;
}

int main(int argc,char *argv[]){
    if(argc<=1){
        fprintf(2,"Usage: xargs command (arg...)\n");
        exit(1);
    }
    char *command=malloc(strlen(argv[1])+1);
    //strlen(argv[1])：计算 argv[1]（即命令行参数中的第二个字符串）的长度，不包括字符串的终止空字符 \0
    //+1 是为了留出空间存储终止字符（\0），因为字符串需要以 \0 结束。
    char *nargv[MAXARG];
    strcpy(command,argv[1]);//strcpy 会将 argv[1] 中的内容（包括终止符 \0）逐个字符复制到 command 所指向的内存区域。
    for(int i=1;i<argc;i++){
        nargv[i-1]=malloc(strlen(argv[i])+1);
        strcpy(nargv[i-1],argv[i]);
    }
    int cargc;
    while((cargc=lineread(nargv,argc-1))!=0){//标准输入有可能有很多行
        nargv[cargc]=0;
        if(fork()==0){
            exec(command,nargv);
            fprintf(2,"exec failed\n");
            exit(1);
        }
        wait(0);
    }
    exit(0);
}
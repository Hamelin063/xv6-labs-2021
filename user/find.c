#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void finder(char *path,char *target)
{
  char buf[512], *p;
  int fd;//文件描述符
  struct dirent de;//读取目录条目
  struct stat st;//读取文件状态

  if((fd = open(path, 0)) < 0){ //只读打开路径
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){ //检查打开的fd对应的文件状态信息
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    fprintf(2,"Usage: find dir file\n");
    exit(1);

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");//检查路径长度，防止缓冲区溢出
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);//buf指向的是开头，+strlen(buf)就指向了buf的末尾
    *p++ = '/';//若path是"a",则buf变为"a/"
    //如果路径长度没问题，就将path复制到buf中，然后p指向buf末尾。接着添加一个'/'，此时p指向这个斜杠后的位置。
    //这样，当读取目录中的每个条目时，会把条目名称追加到路径后面，形成完整的路径名
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0||strcmp(de.name,".")==0||strcmp(de.name,"..")==0)//目录项未使用/过滤当前目录和上级目录
        continue;
      memmove(p, de.name, DIRSIZ);//
      p[DIRSIZ] = 0;//若 de.name 是 "b"，则 buf 变为 "a/b"，并添加终止符 \0
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      if(st.type==T_DIR){
        finder(buf,target);//递归进下一个目录继续寻找
      }
      else if(st.type==T_FILE){
        if(strcmp(de.name,target)==0){
            printf("%s\n",buf);
        }
      }
    }
    break;
  }
  close(fd);
}
int main(int argc,char *argv[]){
    if(argc!=3){
        fprintf(2,"usage: find dir file\n");
        exit(1);
    }
    char *path = argv[1];
    char *target = argv[2];
    finder(path,target);
    exit(0);
}
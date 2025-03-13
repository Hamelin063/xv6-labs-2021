// sysproc.c 主要处理与进程相关的系统调用
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "sysinfo.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// 内核态的sys_trace
// 示例指令：trace 32 grep hello README
// argint的作用是把32这个数字提取出
// int argint(int n, int *ip);
// int n: 要提取的参数序号（从 0 开始）。
// int *ip: 用于存储提取的整数值的指针。

// |=：按位或运算
uint64
sys_trace(void)
{
  int mask_to_trace;
  if(argint(0,&mask_to_trace)<0){
    return -1;
  }
  myproc()->tracemark |= mask_to_trace;
  return 0;
}

// 将内核数据传递到用户空间，需要使用copyout函数
// 这个函数通常在内核需要将数据传递到用户空间时使用,数据从内核空间（src）复制到用户空间的虚拟地址（dstva）
// 例如在系统调用中返回数据给用户程序。
// int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len);
// pagetable:用户进程的页表（pagetable_t 类型），用于将用户虚拟地址（dstva）转换为物理地址。
// dstva:用户空间的虚拟地址，表示数据复制的目标地址。
// src:内核空间的指针，表示数据复制的源地址。
// len:要复制的数据的长度（以字节为单位）。

// copyout() 是通过逐字节复制数据的方式将内核空间的数据复制到用户空间的。
// 无论数据的类型是什么（如 int、struct stat 等），都可以通过 char * 指针逐字节访问和复制。

// argaddr()用于从用户程序的系统调用参数中获取一个地址。
// 第一个参数 0 表示获取系统调用的第一个参数（参数序号从 0 开始）。
// 第二个参数 &p 是一个指针，用于存储获取到的地址。
uint64
sys_sysinfo(void){
  struct proc *mproc=myproc();
  uint64 p;
  if(argaddr(0,&p)<0){
    return -1;
  }
  struct sysinfo s;
  s.freemem=kfreememosize();
  s.nproc=nunusedpnum();
  if(copyout(mproc->pagetable,p,(char*)&s,sizeof(s))<0){
    return -1;
  }
  return 0;
}

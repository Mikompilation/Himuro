#ifndef EE_EEKERNEL_H
#define EE_EEKERNEL_H

#define WRITEBACK_DCACHE        0
#define INVALIDATE_DCACHE       1
#define INVALIDATE_ICACHE       2
#define INVALIDATE_CACHE        3

#define DMAC_VIF0               0
#define DMAC_VIF1               1
#define DMAC_GIF                2
#define DMAC_FROM_IPU           3
#define DMAC_TO_IPU             4
#define DMAC_FROM_SPR           8
#define DMAC_TO_SPR             9

typedef struct {
	int status;
	void *entry;
	void *stack;
	int stackSize;
	void *gpReg;
	int initPriority;
	int currentPriority;
	u_int attr;
	u_int option;
	int waitType;
	int waitId;
	int wakeupCount;
} ThreadParam;

typedef struct {
	int currentCount;
	int maxCount;
	int initCount;
	int numWaitThreads;
	u_int attr;
	u_int option;
} SemaParam;

extern void *_gp;

#define ExitHandler() \
    asm volatile("\n\
        sync.l    \n\
        ei        \n\
    ");

int CreateThread(ThreadParam *);
int StartThread(int, void *arg);
int GetThreadId(void);
int ChangeThreadPriority(int, int);
int RotateThreadReadyQueue(int);

int CreateSema(SemaParam *param);
int SignalSema(int);
int WaitSema(int);
int DeleteSema(int);

#endif // EE_EEKERNEL_H

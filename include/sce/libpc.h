#ifndef SCE_LIBPC_H
#define SCE_LIBPC_H

static inline int scePcGetCounter0(void) {
    register int ctr0;
    __asm__ volatile ("mfpc %0, 0": "=r" (ctr0));
    return ctr0;
}

#define SCE_PC0_RESERVED            (0  <<  5)
#define SCE_PC0_CPU_CYCLE           (1  <<  5)
#define SCE_PC0_SINGLE_ISSUE        (2  <<  5)
#define SCE_PC0_BRANCH_ISSUED       (3  <<  5)
#define SCE_PC0_BTAC_MISS           (4  <<  5)
#define SCE_PC0_ITLB_MISS           (5  <<  5)
#define SCE_PC0_ICACHE_MISS         (6  <<  5)
#define SCE_PC0_DTLB_ACCESSED       (7  <<  5)
#define SCE_PC0_NONBLOCK_LOAD       (8  <<  5)
#define SCE_PC0_WBB_SINGLE_REQ      (9  <<  5)
#define SCE_PC0_WBB_BURST_REQ       (10 <<  5)
#define SCE_PC0_ADDR_BUS_BUSY       (11 <<  5)
#define SCE_PC0_INST_COMP           (12 <<  5)
#define SCE_PC0_NON_BDS_COMP        (13 <<  5)
#define SCE_PC0_COP2_COMP           (14 <<  5)
#define SCE_PC0_LOAD_COMP           (15 <<  5)
#define SCE_PC0_NO_EVENT            (16 <<  5)

#define SCE_PC1_LOW_BRANCH_ISSUED   (0  << 15)
#define SCE_PC1_CPU_CYCLE           (1  << 15)
#define SCE_PC1_DUAL_ISSUE          (2  << 15)
#define SCE_PC1_BRANCH_MISS_PREDICT (3  << 15)
#define SCE_PC1_TLB_MISS            (4  << 15)
#define SCE_PC1_DTLB_MISS           (5  << 15)
#define SCE_PC1_DCACHE_MISS         (6  << 15)
#define SCE_PC1_WBB_SINGLE_UNAVAIL  (7  << 15)
#define SCE_PC1_WBB_BURST_UNAVAIL   (8  << 15)
#define SCE_PC1_WBB_BURST_ALMOST    (9  << 15)
#define SCE_PC1_WBB_BURST_FULL      (10 << 15)
#define SCE_PC1_DATA_BUS_BUSY       (11 << 15)
#define SCE_PC1_INST_COMP           (12 << 15)
#define SCE_PC1_NON_BDS_COMP        (13 << 15)
#define SCE_PC1_COP1_COMP           (14 << 15)
#define SCE_PC1_STORE_COMP          (15 << 15)
#define SCE_PC1_NO_EVENT            (16 << 15)

#define SCE_PC_EXL0                 (1  <<  1)
#define SCE_PC_K0                   (1  <<  2)
#define SCE_PC_S0                   (1  <<  3)
#define SCE_PC_U0                   (1  <<  4)
#define SCE_PC_EVENT0               (31 <<  5)
#define SCE_PC_EXL1                 (1  << 11)
#define SCE_PC_K1                   (1  << 12)
#define SCE_PC_S1                   (1  << 13)
#define SCE_PC_U1                   (1  << 14)
#define SCE_PC_EVENT1               (31 << 15)
#define SCE_PC_CTE                  (1  << 31)

void scePcStart(int control, int counter0, int counter1);
void scePcStop();

#endif // SCE_LIBPC_H

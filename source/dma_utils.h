#ifndef _DMA_UTILS_
#define _DMA_UTILS_

#include <sys/mman.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

/* ltoh: little to host */
/* htol: little to host */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#  define ltohl(x)       (x)
#  define ltohs(x)       (x)
#  define htoll(x)       (x)
#  define htols(x)       (x)
#elif __BYTE_ORDER == __BIG_ENDIAN
#  define ltohl(x)     __bswap_32(x)
#  define ltohs(x)     __bswap_16(x)
#  define htoll(x)     __bswap_32(x)
#  define htols(x)     __bswap_16(x)
#endif

#define BYPASS_MAP_SIZE  (256*1024UL)
#define CONTROL_MAP_SIZE (256*1024UL)

#define GPIO0_DAT 0x00000
#define GPIO1_DAT 0x00000
#define GPIO2_DAT 0x20000

#define BP3_REGFILE     0x30000
#define RX_STATE        0x00
#define TX_STATE        0x04
#define CPU2FIFO_CNT    0x08
#define FIFO2SNN_CNT    0x0C
#define SNN2FIFO_CNT    0x10
#define FIFO2CPU_CNT    0x14
#define WDATA_1         0x18
#define WDATA_2         0x1C
#define RDATA_1         0x20
#define RDATA_2         0x24
#define DATA_CNT        0x28
#define TLAST_CNT       0x2C
#define SEND_LEN        0x30
#define CTRL_REG        0x34
#define oFrmNum_REG     0x38

#define CONTROL_DEV  0
#define TRANSMIT_DEV 1
#define H2C_DEV      2
#define C2H_DEV      3

static int open_dev(char *devName,int dev_type);
static void *mmap_dev(int fd,long mapsize);
void write_control(off_t target, uint32_t val);
void write_bypass(off_t target, uint32_t val);
int send_dma(char *buffer,uint64_t size);
int read_dma(char *buffer,uint64_t size);
uint32_t read_reg(void *map_base,off_t target);
uint32_t read_bypass(off_t target);
int wait_irq0(void);
int pcie_init();
void pcie_deinit();

#endif // _DMA_UTILS_
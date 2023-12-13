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

#define BP_LED_BASE     0x10000
#define CT_LED0_BASE    0x00000
#define CT_LED1_BASE    0x10000

#define REGFILE_BASE    0x00000
#define RX_STATE        0  * 4
#define TX_STATE        1  * 4
#define CPU2FIFO_CNT    2  * 4
#define FIFO2SNN_CNT    3  * 4
#define SNN2FIFO_CNT    4  * 4
#define FIFO2CPU_CNT    5  * 4
#define WDATA_1         6  * 4
#define WDATA_2         7  * 4
#define RDATA_1         8  * 4
#define RDATA_2         9  * 4
#define DATA_CNT        10 * 4
#define TLAST_CNT       11 * 4

#define SEND_LEN        20 * 4
#define CTRL_REG        21 * 4
#define oFrmNum_REG     22 * 4
#define DP_RSTN         23 * 4
#define SINGLE_CHANNEL  24 * 4
#define CHANNEL_MASK    25 * 4
#define OEN             26 * 4

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
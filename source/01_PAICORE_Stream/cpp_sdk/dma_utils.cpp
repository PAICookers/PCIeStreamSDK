#include "dma_utils.h"

void *control_base;
int control_fd;

void *bypass_base;
int bypass_fd;

int h2c_fd;
int c2h_fd;
int irq0_fd;

static int open_dev(char *devName,int dev_type)
{
    int fd;
    if(dev_type == CONTROL_DEV)
        fd = open(devName, O_RDWR | O_SYNC);
    else if(dev_type == TRANSMIT_DEV)
        fd = open(devName, O_RDWR | O_NONBLOCK);
    else if(dev_type == H2C_DEV)
        fd = open(devName, O_RDWR);
    else if(dev_type == C2H_DEV)
        fd = open(devName, O_RDWR | O_TRUNC);
    else
    {
        printf("choose dev_type control or transmit.\n");
        return -1;
    }
    if(fd == -1)
    {
        printf("open dev error\n");
        return -1;
    }
    return fd;
}

static void *mmap_dev(int fd,long mapsize)
{
    void *map_base;
    map_base = mmap(0, mapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return map_base;
}

void write_reg(void *map_base, off_t target, uint32_t writeval)
{
    void *virt_addr;
    virt_addr = (uint8_t*)map_base + target; // (uint8_t*) move pointer using btyes.

    writeval = htoll(writeval);
    *((uint32_t *) virt_addr) = writeval;
}

uint32_t read_reg(void *map_base,off_t target)
{
    void *virt_addr;
    virt_addr = (uint8_t*)map_base + target; // (uint8_t*) move pointer using btyes.
    
    uint32_t readval = *((uint32_t *) virt_addr);
    readval = ltohl(readval);
    return readval;
}


void write_control(off_t target, uint32_t val)
{
   write_reg(control_base,target,val);
}

void write_bypass(off_t target, uint32_t val)
{
   write_reg(bypass_base,target,val);
}

uint32_t read_bypass(off_t target)
{
    uint32_t val;
    val = read_reg(bypass_base,target);
    return val;
}

int send_dma(char *buffer,uint64_t size)
{
    int rc; 
    rc = write(h2c_fd, buffer, size);
    return rc;
}

int read_dma(char *buffer,uint64_t size)
{
    int rc; 
    rc = read(c2h_fd, buffer, size);
    return rc;
}



int wait_irq0(void)
{
    int val;
    read(irq0_fd,&val,4);
    return val;
}

int pcie_init()
{
    control_fd = open_dev((char *)"/dev/xdma0_user", CONTROL_DEV);
    if(control_fd < 0)
        return -5;
    control_base = mmap_dev(control_fd,CONTROL_MAP_SIZE);

    bypass_fd = open_dev((char *)"/dev/xdma0_bypass", CONTROL_DEV);
    if(bypass_fd < 0)
        return -5;
    bypass_base = mmap_dev(bypass_fd,BYPASS_MAP_SIZE);

    irq0_fd = open_dev((char *)"/dev/xdma0_events_0", CONTROL_DEV);
    if(irq0_fd < 0)
        return -5;

    h2c_fd = open_dev((char *)"/dev/xdma0_h2c_0", H2C_DEV);
    if(h2c_fd < 0)
        return -5;

    c2h_fd = open_dev((char *)"/dev/xdma0_c2h_0", H2C_DEV);
    if(c2h_fd < 0)
        return -5;

    return 1;
}

void pcie_deinit()
{
    if(control_fd >= 0) close(control_fd);
    if(bypass_fd >= 0)  close(bypass_fd);
    if(irq0_fd >= 0)    close(irq0_fd);
    if(h2c_fd >= 0)     close(h2c_fd);
    if(c2h_fd >= 0)     close(c2h_fd);
}
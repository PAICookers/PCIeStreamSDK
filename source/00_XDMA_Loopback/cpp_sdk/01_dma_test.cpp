#include "dma_utils.h"

void show_reg_status(void)
{
    uint32_t val,val2;
    printf("\n\r");
    val = read_bypass(REGFILE_BASE + RX_STATE);
    printf("RX_STATE      : %d\n",val);
    val = read_bypass(REGFILE_BASE + TX_STATE);
    printf("TX_STATE      : %d\n",val);
    val = read_bypass(REGFILE_BASE + CPU2FIFO_CNT);
    printf("CPU2FIFO_CNT  : %d\n",val);
    val = read_bypass(REGFILE_BASE + FIFO2SNN_CNT);
    printf("FIFO2SNN_CNT  : %d\n",val);
    val = read_bypass(REGFILE_BASE + SNN2FIFO_CNT);
    printf("SNN2FIFO_CNT  : %d\n",val);
    val = read_bypass(REGFILE_BASE + FIFO2CPU_CNT);
    printf("FIFO2CPU_CNT  : %d\n",val);
    val = read_bypass(REGFILE_BASE + SEND_LEN);
    printf("SEND_LEN      : %d\n",val);
    val = read_bypass(REGFILE_BASE + CTRL_REG);
    printf("CTRL_REG      : %d\n",val);
    val = read_bypass(REGFILE_BASE + WDATA_1);
    val2 = read_bypass(REGFILE_BASE + WDATA_2);
    printf("WDATA         : %08x%08x\n",val2,val);
    val = read_bypass(REGFILE_BASE + RDATA_1);
    val2 = read_bypass(REGFILE_BASE + RDATA_2);
    printf("RDATA         : %08x%08x\n",val2,val);
    val = read_bypass(REGFILE_BASE + DATA_CNT);
    printf("DATA_CNT      : %d\n",val);
    val = read_bypass(REGFILE_BASE + TLAST_CNT);
    printf("TLAST_IN_CNT  : %d\n",val & 0x0000FFFF);
    printf("TLAST_OUT_CNT : %d\n",val >> 16);
    printf("\n\r");
}

int main()
{
    
    if(pcie_init() < 0){
        printf("error");
        return -1;
    }

    write_bypass(REGFILE_BASE + CPU2FIFO_CNT, 0);
    write_bypass(REGFILE_BASE + FIFO2SNN_CNT, 0);
    write_bypass(REGFILE_BASE + SNN2FIFO_CNT, 0);
    write_bypass(REGFILE_BASE + FIFO2CPU_CNT, 0);

    write_bypass(REGFILE_BASE + DP_RSTN, 0);
    write_bypass(REGFILE_BASE + DP_RSTN, 1);
    write_bypass(REGFILE_BASE + CTRL_REG, 1);

    show_reg_status();

    char *buffer = NULL;
    posix_memalign((void **)&buffer, 8, 1024);

    int rc;
    uint64_t write_size = 8 * 50;

    for(int i = 0 ; i < write_size ; i++){
        buffer[i] = i;
    }

    // loopback need to start recv
    write_bypass(REGFILE_BASE + oFrmNum_REG, write_size >> 3);
    write_bypass(REGFILE_BASE + RX_STATE,1);

    write_bypass(REGFILE_BASE + SEND_LEN, write_size >> 3);
    rc = send_dma(buffer, write_size);

    uint32_t val = 0;
    while(val == 0)
    {
        val = read_bypass(REGFILE_BASE + TX_STATE);
    }
    write_bypass(REGFILE_BASE + TX_STATE,0);

    printf("send %d bytes.\n",rc);
    show_reg_status();

    char *read_buffer = NULL;
    posix_memalign((void **)&read_buffer, 8, 1024);
    
    uint64_t read_size = write_size;
    rc = read_dma(read_buffer, read_size);
    printf("read %d bytes.\n",rc);

    while(val == 1)
    {
        val = read_bypass(REGFILE_BASE + RX_STATE);
    }
    write_bypass(REGFILE_BASE + RX_STATE,0);

    show_reg_status();

    int err = 0;
    for(int i = 0 ; i < write_size ; i++){
        if(read_buffer[i] != buffer[i])
        {
            err = 1;
            
        }     
    }
    if(!err) printf("DMA LOOP TEST PASS  !\n");
    else     printf("DMA LOOP TEST ERROR !\n");

    pcie_deinit();
    return 0;

}

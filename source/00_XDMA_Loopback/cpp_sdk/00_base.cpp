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

    // // wait_irq0();

    uint32_t led_val;

    for(int i; i < 2; i++){
        led_val = 0x1;
        write_control(CT_LED0_BASE,led_val);
        write_control(CT_LED1_BASE,led_val);
        write_bypass(BP_LED_BASE,led_val);
        usleep(200000);
        led_val = 0x0;
        write_control(CT_LED0_BASE,led_val);
        write_control(CT_LED1_BASE,led_val);
        write_bypass(BP_LED_BASE,led_val);
        usleep(200000);
    }

    show_reg_status();
    pcie_deinit();
    return 0;

}

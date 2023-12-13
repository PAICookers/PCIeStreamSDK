import time

from dma_util import *

if __name__ == "__main__":

    if(pcie_init() < 0):
        print("pcie_init error")

    write_bypass(REGFILE_BASE + CPU2FIFO_CNT, 0)
    write_bypass(REGFILE_BASE + FIFO2SNN_CNT, 0)
    write_bypass(REGFILE_BASE + SNN2FIFO_CNT, 0)
    write_bypass(REGFILE_BASE + FIFO2CPU_CNT, 0)

    write_bypass(REGFILE_BASE + DP_RSTN, 0)
    write_bypass(REGFILE_BASE + DP_RSTN, 1)

    for i in range(2):
        led_val = 0x1
        write_control(CT_LED0_BASE,led_val)
        write_control(CT_LED1_BASE,led_val)
        write_bypass(BP_LED_BASE,led_val)
        time.sleep(0.2)
        led_val = 0x0
        write_control(CT_LED0_BASE,led_val)
        write_control(CT_LED1_BASE,led_val)
        write_bypass(BP_LED_BASE,led_val)
        time.sleep(0.2)
    show_reg_status()


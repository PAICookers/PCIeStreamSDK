import numpy as np
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
    write_bypass(REGFILE_BASE + CTRL_REG, 1)

    show_reg_status()

    write_size = 2 << 3

    # loopback need to start recv
    write_bypass(REGFILE_BASE + oFrmNum_REG, write_size >> 3)
    write_bypass(REGFILE_BASE + RX_STATE,1)

    write_bypass(REGFILE_BASE + SEND_LEN, write_size >> 3)

    send_dma("01234567abcdefgh",write_size)

    val = 0
    while(val == 0):
        val = read_bypass(REGFILE_BASE + TX_STATE)
    write_bypass(REGFILE_BASE + TX_STATE,0)

    show_reg_status()

    
    a = '1'*write_size
    read_dma(a,write_size)

    while(val == 1):
        val = read_bypass(REGFILE_BASE + RX_STATE)
    write_bypass(REGFILE_BASE + RX_STATE,0)
    
    # print(a)
    show_reg_status()

    # 直接用send_dma收发的是ascii码，而且无法用指针来接收输出，需要使用numpy和返回值包装
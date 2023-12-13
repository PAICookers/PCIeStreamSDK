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

    # show_reg_status()

    # send_data = np.array([1 << 60] * 10, dtype=np.uint64)
    send_data = np.random.randint(0,2**64-1, size=50, dtype=np.uint64)
    write_byte_nums = send_data.size << 3 # byte_nums

    # loopback need to start recv
    write_bypass(REGFILE_BASE + oFrmNum_REG, write_byte_nums >> 3)
    write_bypass(REGFILE_BASE + RX_STATE,1)

    write_bypass(REGFILE_BASE + SEND_LEN, write_byte_nums >> 3)

    rc = send_dma_np(send_data,write_byte_nums)

    print("send %d bytes." % rc)

    val = 0
    while(val == 0):
        val = read_bypass(REGFILE_BASE + TX_STATE)
    write_bypass(REGFILE_BASE + TX_STATE,0)

    show_reg_status()

    rc ,read_data = read_dma_np(write_byte_nums)
    print("read %d bytes." % rc)
    while(val == 1):
        val = read_bypass(REGFILE_BASE + RX_STATE)
    write_bypass(REGFILE_BASE + RX_STATE,0)
    
    show_reg_status()

    print(read_data.shape)
    print((send_data == read_data).all())

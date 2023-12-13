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

    data1 = np.array([1 << 60] * 50, dtype=np.uint64)
    write_byte_nums = data1.size << 3 # byte_nums

    # loopback need to start recv
    write_bypass(REGFILE_BASE + oFrmNum_REG, write_byte_nums >> 3)
    write_bypass(REGFILE_BASE + RX_STATE,1)

    write_bypass(REGFILE_BASE + SEND_LEN, write_byte_nums >> 3)

    rc = send_dma_np(data1,write_byte_nums)

    print("send %d bytes." % rc)

    val = 0
    while(val == 0):
        val = read_bypass(REGFILE_BASE + TX_STATE)
    write_bypass(REGFILE_BASE + TX_STATE,0)

    show_reg_status()

    a = '1' * write_byte_nums
    rc = read_dma(a,write_byte_nums)
    print("read %d bytes." % rc)
    while(val == 1):
        val = read_bypass(REGFILE_BASE + RX_STATE)
    write_bypass(REGFILE_BASE + RX_STATE,0)
    
    show_reg_status()

    # data1 = np.array([1, 3, 5, 7, 9], dtype=np.uint64)
    # data2 = np.array([2, 4, 6, 8, 10], dtype=np.uint64)
    # var1 = _add_arrays_1d(data1,data2)
    # print('var1=', var1)
    # print(var1.dtype)
from serial_utils import serialConfig
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

    serialConfig()
    
    framePath = "../../test_data/RAW_FRAME/config.bin"
    send_data = np.fromfile(framePath, dtype='<u8')
    write_byte_nums = send_data.size << 3 # byte_nums

    write_bypass(REGFILE_BASE + SEND_LEN, write_byte_nums >> 3)

    rc = send_dma_np(send_data,write_byte_nums)

    print("send %d bytes." % rc)

    val = 0
    while(val == 0):
        val = read_bypass(REGFILE_BASE + TX_STATE)
    write_bypass(REGFILE_BASE + TX_STATE,0)

    show_reg_status()


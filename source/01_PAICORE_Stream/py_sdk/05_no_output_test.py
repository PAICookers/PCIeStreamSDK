from serial_utils import serialConfig
import numpy as np
import time

from dma_util import *

def SendFrame(send_data):
    write_byte_nums = send_data.size << 3 # byte_nums
    write_bypass(REGFILE_BASE + SEND_LEN, write_byte_nums >> 3)
    rc = send_dma_np(send_data,write_byte_nums)
    # print("send %d bytes." % rc)

    val = 0
    while(val == 0):
        val = read_bypass(REGFILE_BASE + TX_STATE)
    write_bypass(REGFILE_BASE + TX_STATE,0)

def RecvFrame(oFrmNum):
    write_bypass(REGFILE_BASE + RX_STATE,1)
    rc ,outputFrames = read_dma_np(oFrmNum << 3)
    # print("read %d bytes." % rc)

    val = 1
    while(val == 1):
        val = read_bypass(REGFILE_BASE + RX_STATE)
    write_bypass(REGFILE_BASE + RX_STATE,0)
    return outputFrames

if __name__ == "__main__":

    if(pcie_init() < 0):
        print("pcie_init error")

    write_bypass(REGFILE_BASE + CPU2FIFO_CNT, 0)
    write_bypass(REGFILE_BASE + FIFO2SNN_CNT, 0)
    write_bypass(REGFILE_BASE + SNN2FIFO_CNT, 0)
    write_bypass(REGFILE_BASE + FIFO2CPU_CNT, 0)

    write_bypass(REGFILE_BASE + DP_RSTN, 0)
    write_bypass(REGFILE_BASE + DP_RSTN, 1)

    oFrmNum = np.random.randint(1, 100)
    write_bypass(REGFILE_BASE + oFrmNum_REG, oFrmNum)

    serialConfig()
  
    # make sure config and init use one channel
    write_bypass(REGFILE_BASE + OEN, 0b0110)

    write_bypass(REGFILE_BASE + CHANNEL_MASK, 0b0100)
    write_bypass(REGFILE_BASE + SINGLE_CHANNEL, 1)


    write_bypass(REGFILE_BASE + RX_STATE,1)
    rc ,outputFrames = read_dma_np(oFrmNum << 3)
    print("read %d bytes." % rc)

    val = 1
    while(val == 1):
        val    = read_bypass(REGFILE_BASE + RX_STATE)
        rx_num = read_bypass(REGFILE_BASE + FIFO2CPU_CNT  )
        if rx_num == oFrmNum:
            break
    write_bypass(REGFILE_BASE + RX_STATE,0)
    show_reg_status()
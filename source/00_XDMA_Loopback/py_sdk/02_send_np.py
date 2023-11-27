from example import pcie_init,read_bypass,write_bypass,send_dma,read_dma
from example import send_dma_np, _add_arrays_1d
import time

import numpy as np
 

REGFILE_BASE = 0x00000
BP_LED_BASE  = 0x10000

CT_LED0_BASE = 0x00000
CT_LED1_BASE = 0x10000

RX_STATE     = 0x00
TX_STATE     = 0x04
CPU2FIFO_CNT = 0x08
FIFO2SNN_CNT = 0x0C
SNN2FIFO_CNT = 0x10
FIFO2CPU_CNT = 0x14
WDATA_1      = 0x18
WDATA_2      = 0x1C
RDATA_1      = 0x20
RDATA_2      = 0x24
DATA_CNT     = 0x28
TLAST_CNT    = 0x2C
SEND_LEN     = 0x30
CTRL_REG     = 0x34
oFrmNum_REG  = 0x38
DP_RSTN      = 0x3C


def show_reg_status():
    print()
    print("RX_STATE      : %d" % read_bypass(REGFILE_BASE + RX_STATE      ))
    print("TX_STATE      : %d" % read_bypass(REGFILE_BASE + TX_STATE      ))
    print("CPU2FIFO_CNT  : %d" % read_bypass(REGFILE_BASE + CPU2FIFO_CNT  ))
    print("FIFO2SNN_CNT  : %d" % read_bypass(REGFILE_BASE + FIFO2SNN_CNT  ))
    print("SNN2FIFO_CNT  : %d" % read_bypass(REGFILE_BASE + SNN2FIFO_CNT  ))
    print("FIFO2CPU_CNT  : %d" % read_bypass(REGFILE_BASE + FIFO2CPU_CNT  ))
    print("SEND_LEN      : %d" % read_bypass(REGFILE_BASE + SEND_LEN      ))
    print("CTRL_REG      : %d" % read_bypass(REGFILE_BASE + CTRL_REG      ))
    val1 = read_bypass(REGFILE_BASE + WDATA_1)
    val2 = read_bypass(REGFILE_BASE + WDATA_2)
    print("WDATA         : 0x%016x" % (val2 << 32 | val1))
    val1 = read_bypass(REGFILE_BASE + RDATA_1)
    val2 = read_bypass(REGFILE_BASE + RDATA_2)
    print("RDATA         : 0x%016x" % (val2 << 32 | val1))
    print("DATA_CNT      : %d" % read_bypass(REGFILE_BASE + DATA_CNT      ))
    val = read_bypass(REGFILE_BASE + TLAST_CNT)
    print("TLAST_IN_CNT  : %d" % (val & 0x0000FFFF))
    print("TLAST_OUT_CNT : %d" % (val >> 16))
    print()

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
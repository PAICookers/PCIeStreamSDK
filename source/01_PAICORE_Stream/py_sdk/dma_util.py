from example import _add_arrays_1d
from example import pcie_init,read_bypass,write_bypass,write_control
from example import send_dma,read_dma
from example import send_dma_np, read_dma_np


REGFILE_BASE = 0x00000
BP_LED_BASE  = 0x10000

CT_LED0_BASE = 0x00000
CT_LED1_BASE = 0x10000

RX_STATE        = 0  * 4
TX_STATE        = 1  * 4
CPU2FIFO_CNT    = 2  * 4
FIFO2SNN_CNT    = 3  * 4
SNN2FIFO_CNT    = 4  * 4
FIFO2CPU_CNT    = 5  * 4
WDATA_1         = 6  * 4
WDATA_2         = 7  * 4
RDATA_1         = 8  * 4
RDATA_2         = 9  * 4
DATA_CNT        = 10 * 4
TLAST_CNT       = 11 * 4

SEND_LEN        = 20 * 4
CTRL_REG        = 21 * 4
oFrmNum_REG     = 22 * 4
DP_RSTN         = 23 * 4
SINGLE_CHANNEL  = 24 * 4
CHANNEL_MASK    = 25 * 4
OEN             = 26 * 4

def show_reg_status():
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

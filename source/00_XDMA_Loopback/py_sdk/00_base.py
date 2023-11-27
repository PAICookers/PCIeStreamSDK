from example import pcie_init,read_bypass,write_bypass,write_control
import time

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


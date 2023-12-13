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

    oFrmNum = 20
    write_bypass(REGFILE_BASE + oFrmNum_REG, oFrmNum)

    serialConfig()
    
    configPath = "../../test_data/RAW_FRAME/config.bin"
    initPath   = "../../test_data/RAW_FRAME/init.bin"
    workPath   = "../../test_data/RAW_FRAME/chipin.bin"
    outputrPath= "../../test_data/RAW_FRAME/chipout_ref.bin"

    configFrames = np.fromfile(configPath, dtype='<u8')
    initFrames   = np.fromfile(initPath, dtype='<u8')
    workFrames   = np.fromfile(workPath, dtype='<u8')

    # make sure config and init use one channel
    write_bypass(REGFILE_BASE + OEN, 0b111)

    write_bypass(REGFILE_BASE + CHANNEL_MASK, 0b010)
    write_bypass(REGFILE_BASE + SINGLE_CHANNEL, 1)
    
    SendFrame(configFrames) # config

    write_bypass(REGFILE_BASE + CTRL_REG, 4)
    SendFrame(initFrames) # init
    write_bypass(REGFILE_BASE + CTRL_REG, 0)

    write_bypass(REGFILE_BASE + SINGLE_CHANNEL, 0)

    t0 = time.time()
    SendFrame(workFrames) # work
    t1 = time.time()
    outputFrames = RecvFrame(oFrmNum)

    t2 = time.time()
    # show_reg_status()

    outputFrames_ref = np.fromfile(outputrPath, dtype='<u8')

    outputFrames = np.delete(outputFrames,np.where(outputFrames == 0))
    outputFrames = np.delete(outputFrames,np.where(outputFrames == 18446744073709551615))

    outputFrames_ref = np.delete(outputFrames_ref,np.where(outputFrames_ref == 0))
    outputFrames_ref = np.delete(outputFrames_ref,np.where(outputFrames_ref == 18446744073709551615))

    print(outputFrames.shape)
    print(outputFrames_ref.shape)

    print((outputFrames == outputFrames_ref).all())

    val = read_bypass(REGFILE_BASE + TLAST_CNT)
    print("CORE INFERENCE TIME : %d us" % val)
    print('SEND FRMAE TIME     :',round((t1-t0)*1000*1000),'us')
    print('RECV FRMAE TIME     :',round((t2-t1)*1000*1000),'us')

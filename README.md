# PCIeStreamSDK

## 简介

基于PAICORE实现的PCIe硬件数据通路，使用Xilinx的XDMA IP核，数据传输为AXI Stream协议，本工程为数据通路中的PC上位机程序，用户可以使用此工程的应用程序与PAICORE进行数据交互，完成收帧和发帧操作。

## Usage
    Compile : sudo make compile

    Your should use frame in bin file.
    
    Config  :
        set mode = 0,cFrmNum,cpath. 

    Run     :
        set mode = 1,iFrmNum,wFrmNum,oFrmNum,ipath,wpath,opath

    Use it in this project : https://github.com/PAICookers/PAIBoard_PCIe

# Table of Terms

|     Name      |       Defnition        | 
| :-----------: | :--------------------: |
|  FrmNum       |        Frame numbers   |  
|  path         |        bin file path   |   
|  c            |        config          | 
|  i            |        init            |
|  w            |        work            |    
|  o            |        output          | 

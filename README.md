# PCIeStreamSDK

## Intro
    dma_utils.cpp 中提供了PCIe AXIstream、AXI Lite和中断的API
    "/dev/xdma0_h2c_0"、"/dev/xdma0_c2h_0" 实现AXI Stream数据输入和输出通道
    "/dev/xdma0_user"、"/dev/xdma0_bypass" 实现AXI Lite设备的读写
    "/dev/xdma0_events_0"                  实现中断的逻辑(目前没用上)

### AXI Stream

    AXI Stream通路作为本硬件实现的主要数据通路，在CPP层面，只需要调用如下函数，即可实现AXI Stream通路数据的读写。

    写数据要求把数据先放入buffer中，然后指定要传输的数据数目；
    rc = send_dma(buffer, size);

    读数据要求准备好特定大小的buffer，然后指定读数据数目，读完成后数据会放入buffer中。
    rc = read_dma(buffer, size);

    与PAICORE APP交互方面，由于本数据传输采用的是CPP，而应用实现采用python语言，目前的数据交互方式是通过bin文件实现，readBin和writeBin函数实现了这方面的功能，外部调用时给出路径和帧数目即可。

### AXI Lite

    为了保证通路的正常运行和通路调试的便捷性，使用AXI Lite接口实现寄存器模块，用于读写一些寄存器，配置硬件通路和读取通路信息。

    RX_STATE        接收状态寄存器
    TX_STATE        发送状态寄存器
    CPU2FIFO_CNT    PCIe输出到FIFO的数据计数器
    FIFO2SNN_CNT    输入到PAICORE的数据计数器
    SNN2FIFO_CNT    PAICORE输出的数据计数器
    FIFO2CPU_CNT    PCIe从FIFO中收到的数据计数器
    WDATA_1         PCIe端每次握手时写数据低32位
    WDATA_2         PCIe端每次握手时写数据高32位
    RDATA_1         PCIe端每次握手时读数据低32位
    RDATA_2         PCIe端每次握手时读数据高32位
    DATA_CNT        PCIe输出数据帧数的计数器
    TLAST_CNT       PCIe输出Tlast次数的计数器
    SEND_LEN        设定发送数据帧数
    CTRL_REG        控制寄存器，Bit0位计数器清除控制位，Bit1为PAICORE的E_INIT信号
    oFrmNum_REG     设定接收数据帧数

    RX_STATE、TX_STATE起到了中断的作用，更好的处理方式是利用XDMA自带的中断，目前还没实现
    计数器和数据记录是为了Debug用，可以观察到数据卡在了哪个位置
    TLAST_CNT、SEND_LEN在使用XDMA时发现，从PCIe输出的AXI Stream数据虽然是按照一整个buffer一次性发送的，但给出的信号不一定是只有1个tlast，可能会有两个或者多个，因此使用SEND_LEN信号人为指定每次发送的帧数，达到足够数目后才有tlast信号

    E_INIT信号需要在每次进行新的推理时给出，先拉高E_INIT信号，然后发初始化帧，然后拉低E_INIT信号，接着才能发送脉冲帧进行推理

    oFrmNum_REG 指定接收帧的数目，如果PAICORE输出帧少于此数目，可以用全0或全1补齐
    收帧指定的数目需要大于将要接收的帧，按照PAICORE的运行机制，对于同一个应用来说，输出的帧数目大致是相同的，只要设定输出帧数比较大就可以，如10000。
    在硬件中的上行通路中加入了fullOutputFrame模块，可以实现输出帧的补齐，补齐到指定的数目，这使得输出帧的数目是固定的，便于上位机接收。
    代码中寄存器配置体现为 write_bypass(BP3_REGFILE + oFrmNum_REG, outputFrmNum);

## Usage
    Compile : sudo make compile

    Config  :
        set mode = 0,cFrmNum,cpath. Your should use frame in bin file.

    Run     :
        set mode = 1,iFrmNum,wFrmNum,oFrmNum,ipath,wpath,opath

    In this project, when sending or receiving frame, you should know the actual number of frames.


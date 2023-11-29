# PCIeStreamSDK

    1、driver不用装，开机自动装
        sudo apt install build-essential
        
        sudo cp xdma.ko /lib/modules/5.15.0-72-generic/kernel/drivers/pci/
        sudo cp xdma.ko /lib/modules/$(uname -r)/kernel/drivers/pci/
        sudo depmod -a
        sudo gedit /etc/modules-load.d/modules.conf

        xdma
        
        lsmod

    2、pybind11需要将cpp编译成.so
        conda install -c conda-forge pybind11
        c++ -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) dma_utils.cpp -o dma_utils$(python3-config --extension-suffix)

    3、不同的测试依赖不同的bitstream和硬件
        github链接……
        00_XDMA_Loopback  环回测试，无需硬件连接
        01_PAICORE_Stream 需要连接PAICORE U3 E,串口
            sudo chmod 777 /dev/ttyUSB0
        02_PAICORE_REG_RECV 使用寄存器读取来读少量的输出数据，只有在极少数据时能够实现一定的加速效果，大部分情况下反而是减速，因为AXIL并不快
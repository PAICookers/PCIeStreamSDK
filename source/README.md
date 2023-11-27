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

    3、不同的测试是不同的bitstream和硬件
        00_XDMA_Loopback  环回测试，无需硬件连接
        01_PAICORE_Stream 需要连接PAICORE U3 E,串口
            sudo chmod 777 /dev/ttyUSB0
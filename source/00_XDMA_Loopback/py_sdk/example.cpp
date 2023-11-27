#include <pybind11/pybind11.h>

#include<iostream>
#include<cstdlib>
#include<ctime>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#undef max//放在<pybind11/numpy.h>之前（#define NOMINMAX有时不起作用）
#include<pybind11/numpy.h>
 
namespace py = pybind11;
using namespace py::literals; 

/* ltoh: little to host */
/* htol: little to host */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#  define ltohl(x)       (x)
#  define ltohs(x)       (x)
#  define htoll(x)       (x)
#  define htols(x)       (x)
#elif __BYTE_ORDER == __BIG_ENDIAN
#  define ltohl(x)     __bswap_32(x)
#  define ltohs(x)     __bswap_16(x)
#  define htoll(x)     __bswap_32(x)
#  define htols(x)     __bswap_16(x)
#endif

using namespace std;

#define BYPASS_MAP_SIZE  (256*1024UL)
#define CONTROL_MAP_SIZE (256*1024UL)

#define CONTROL_DEV  0
#define TRANSMIT_DEV 1
#define H2C_DEV      2
#define C2H_DEV      3

void *control_base;
int control_fd;

void *bypass_base;
int bypass_fd;

int h2c_fd;
int c2h_fd;
int irq0_fd;


int open_dev(char *devName,int dev_type)
{
    int fd;
    if(dev_type == CONTROL_DEV)
        fd = open(devName, O_RDWR | O_SYNC);
    else if(dev_type == TRANSMIT_DEV)
        fd = open(devName, O_RDWR | O_NONBLOCK);
    else if(dev_type == H2C_DEV)
        fd = open(devName, O_RDWR);
    else if(dev_type == C2H_DEV)
        fd = open(devName, O_RDWR | O_TRUNC);
    else
    {
        printf("choose dev_type control or transmit.\n");
        return -1;
    }
    if(fd == -1)
    {
        printf("open dev error\n");
        return -1;
    }
    return fd;
}

void *mmap_dev(int fd,long mapsize)
{
    void *map_base;
    map_base = mmap(0, mapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return map_base;
}


uint32_t read_reg(void *map_base,off_t target)
{
    void *virt_addr;
    virt_addr = (uint8_t*)map_base + target; // (uint8_t*) move pointer using btyes.
    
    uint32_t readval = *((uint32_t *) virt_addr);
    readval = ltohl(readval);
    return readval;
}


uint32_t read_bypass(off_t target)
{
    uint32_t val;
    val = read_reg(bypass_base,target);
    return val;
}

void write_reg(void *map_base, off_t target, uint32_t writeval)
{
    void *virt_addr;
    virt_addr = (uint8_t*)map_base + target; // (uint8_t*) move pointer using btyes.

    writeval = htoll(writeval);
    *((uint32_t *) virt_addr) = writeval;
}

void write_control(off_t target, uint32_t val)
{
   write_reg(control_base,target,val);
}

void write_bypass(off_t target, uint32_t val)
{
   write_reg(bypass_base,target,val);
}

int send_dma(char *buffer,uint64_t size)
{
    int rc; 
    rc = write(h2c_fd, buffer, size);
    return rc;
}

int read_dma(char *buffer,uint64_t size)
{
    int rc; 
    rc = read(c2h_fd, buffer, size);
    return rc;
}

int pcie_init()
{
    control_fd = open_dev((char *)"/dev/xdma0_user", CONTROL_DEV);
    if(control_fd < 0)
        return -5;
    control_base = mmap_dev(control_fd,CONTROL_MAP_SIZE);

    bypass_fd = open_dev((char *)"/dev/xdma0_bypass", CONTROL_DEV);
    if(bypass_fd < 0)
        return -5;
    bypass_base = mmap_dev(bypass_fd,BYPASS_MAP_SIZE);

    irq0_fd = open_dev((char *)"/dev/xdma0_events_0", CONTROL_DEV);
    if(irq0_fd < 0)
        return -5;

    h2c_fd = open_dev((char *)"/dev/xdma0_h2c_0", H2C_DEV);
    if(h2c_fd < 0)
        return -5;

    c2h_fd = open_dev((char *)"/dev/xdma0_c2h_0", H2C_DEV);
    if(c2h_fd < 0)
        return -5;

    return 1;
}

py::array_t<uint64_t> _add_arrays_1d(py::array_t<uint64_t>& arr1, py::array_t<uint64_t>& arr2) {
    py::buffer_info buf1 = arr1.request();// 获取arr1, arr2的信息
    py::buffer_info buf2 = arr2.request();
 
    if (buf1.ndim != 1 || buf2.ndim != 1)
        throw std::runtime_error("Number of dimensions must be one");
    if (buf1.size != buf2.size)
        throw std::runtime_error("Input shape must match");
 
    auto result = py::array_t<uint64_t>(buf1.size);//申请空间
    py::buffer_info buf3 = result.request();
 
    uint64_t* ptr1 = (uint64_t*)buf1.ptr;       //获取numpy.ndarray 数据指针
    uint64_t* ptr2 = (uint64_t*)buf2.ptr;
    uint64_t* ptr3 = (uint64_t*)buf3.ptr;
 
    for (int i = 0; i < buf1.shape[0]; i++){//指针访问numpy.ndarray
        ptr3[i] = ptr1[i] + ptr2[i];}
 
    return result;
}


int send_dma_np(py::array_t<uint64_t>& send_data, uint64_t bytes_num)
{
    py::buffer_info send_data_buf = send_data.request();
    uint8_t* send_ptr = (uint8_t*)send_data_buf.ptr;

    int rc; 
    rc = write(h2c_fd, send_ptr, bytes_num);
    return rc;
}

tuple<int, py::array_t<uint64_t>> read_dma_np(uint64_t bytes_num)
{
    auto read_data_np = py::array_t<uint64_t>(bytes_num >> 3);//申请空间
    py::buffer_info read_data_buf = read_data_np.request();
    uint8_t* read_ptr = (uint8_t*)read_data_buf.ptr;
    
    int rc;
    rc = read(c2h_fd, read_ptr, bytes_num);
    return {rc, read_data_np};
}

PYBIND11_MODULE(example, m) {
    m.doc() = "pybind11 example plugin";

    m.def("pcie_init", &pcie_init, "A function that adds two numbers");
    m.def("write_control", &write_control, "A function that adds two numbers");
    m.def("write_bypass", &write_bypass, "A function that adds two numbers");
    m.def("read_bypass", &read_bypass, "A function that adds two numbers");
    m.def("send_dma", &send_dma, "A function that adds two numbers");
    m.def("read_dma", &read_dma, "A function that adds two numbers");
    m.def("_add_arrays_1d", &_add_arrays_1d);
    m.def("send_dma_np", &send_dma_np);
    m.def("read_dma_np", &read_dma_np);
}
#include "dma_utils.h"
#include <thread>

#include <chrono>   
using namespace std;
using namespace chrono;

void show_reg_status(void)
{
    uint32_t val,val2;
    printf("\n\r");
    val = read_bypass(REGFILE_BASE + RX_STATE);
    printf("RX_STATE      : %d\n",val);
    val = read_bypass(REGFILE_BASE + TX_STATE);
    printf("TX_STATE      : %d\n",val);
    val = read_bypass(REGFILE_BASE + CPU2FIFO_CNT);
    printf("CPU2FIFO_CNT  : %d\n",val);
    val = read_bypass(REGFILE_BASE + FIFO2SNN_CNT);
    printf("FIFO2SNN_CNT  : %d\n",val);
    val = read_bypass(REGFILE_BASE + SNN2FIFO_CNT);
    printf("SNN2FIFO_CNT  : %d\n",val);
    val = read_bypass(REGFILE_BASE + FIFO2CPU_CNT);
    printf("FIFO2CPU_CNT  : %d\n",val);
    val = read_bypass(REGFILE_BASE + SEND_LEN);
    printf("SEND_LEN      : %d\n",val);
    val = read_bypass(REGFILE_BASE + CTRL_REG);
    printf("CTRL_REG      : %d\n",val);
    val = read_bypass(REGFILE_BASE + WDATA_1);
    val2 = read_bypass(REGFILE_BASE + WDATA_2);
    printf("WDATA         : %08x%08x\n",val2,val);
    val = read_bypass(REGFILE_BASE + RDATA_1);
    val2 = read_bypass(REGFILE_BASE + RDATA_2);
    printf("RDATA         : %08x%08x\n",val2,val);
    val = read_bypass(REGFILE_BASE + DATA_CNT);
    printf("DATA_CNT      : %d\n",val);
    val = read_bypass(REGFILE_BASE + TLAST_CNT);
    printf("TLAST_IN_CNT  : %d\n",val & 0x0000FFFF);
    printf("TLAST_OUT_CNT : %d\n",val >> 16);
    printf("\n\r");
}

void read_bin_to_buffer(char *filename, char *buffer, uint64_t size)
{
	FILE* fp;
	fp = std::fopen(filename, "r");
    if(!fp) {
        std::perror("File opening failed");
        exit(-1);
    }
	fread(buffer,4,size,fp);
	fclose(fp);
}

void writeBin(char *filename, char *buffer, uint64_t size)
{
    FILE *fp;
    if ((fp = fopen(filename, "wb")) == NULL)
    {
        std::perror("File opening failed");
        exit(-1);
    }
    fwrite(buffer, sizeof(char), size, fp);
    fclose(fp);
}

void SendFrame(char *filename, uint64_t size)
{
    int rc;
    char *buffer = NULL;
    uint32_t val = 0;

    write_bypass(REGFILE_BASE + SEND_LEN, size >> 3);

    posix_memalign((void **)&buffer, 8, size);
    read_bin_to_buffer(filename, buffer, size);

    rc = send_dma(buffer, size);
    
    while(val == 0)
    {
        val = read_bypass(REGFILE_BASE + TX_STATE);
    }
    write_bypass(REGFILE_BASE + TX_STATE,0);
    printf("send %d bytes.\n",rc);
    // show_reg_status();
}

void RecvFrame(char *filename, uint64_t size)
{
    int rc;
    char *read_buffer = NULL;
    uint32_t val = 1;

    write_bypass(REGFILE_BASE + RX_STATE,1);

    posix_memalign((void **)&read_buffer, 8, size);
    rc = read_dma(read_buffer, size);
    
    while(val == 1)
    {
        val = read_bypass(REGFILE_BASE + RX_STATE);
    }
    write_bypass(REGFILE_BASE + RX_STATE,0);

    writeBin(filename, read_buffer, size);

    printf("read %d bytes.\n",rc);
    show_reg_status();
}

int main()
{

    if(pcie_init() < 0){
        printf("error");
        return -1;
    }

    write_bypass(REGFILE_BASE + CPU2FIFO_CNT, 0);
    write_bypass(REGFILE_BASE + FIFO2SNN_CNT, 0);
    write_bypass(REGFILE_BASE + SNN2FIFO_CNT, 0);
    write_bypass(REGFILE_BASE + FIFO2CPU_CNT, 0);

    write_bypass(REGFILE_BASE + DP_RSTN, 0);
    write_bypass(REGFILE_BASE + DP_RSTN, 1);

    uint64_t oFrmNum;
    oFrmNum = 10000;
    write_bypass(REGFILE_BASE + oFrmNum_REG, oFrmNum);


    char fileName[] ="../../test_data/RAW_FRAME/config.bin";
    char fileName1[]="../../test_data/RAW_FRAME/init.bin";
    char fileName2[]="../../test_data/RAW_FRAME/chipin.bin";
    char fileName3[]="../../test_data/RAW_FRAME/chipout.bin";

    uint64_t configSize = 8 * 320208; //bytes
    uint64_t initSize   = 8 * 150; //bytes
    uint64_t inputSize  = 8 * 6401; //bytes
    uint64_t outputSize = 8 * oFrmNum;

    show_reg_status();

    SendFrame(fileName, configSize);

    auto start = system_clock::now();
    
    write_bypass(REGFILE_BASE + CTRL_REG, 4);
    SendFrame(fileName1, initSize);
    write_bypass(REGFILE_BASE + CTRL_REG, 0);
    SendFrame(fileName2, inputSize);
    RecvFrame(fileName3, outputSize);

    auto end   = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout <<  "send & recv frame :" 
        << double(duration.count()) * microseconds::period::num / microseconds::period::den * 1000
        << " ms" << endl;
        

    pcie_deinit();
    return 0;

}

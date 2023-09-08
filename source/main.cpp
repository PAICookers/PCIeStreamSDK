#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include "dma_utils.h"
#include <thread>

void show_reg_status(void)
{
    uint32_t val,val2;
    printf("\n\r");
    val = read_bypass(BP3_REGFILE + RX_STATE);
    printf("RX_STATE      : %d\n",val);
    val = read_bypass(BP3_REGFILE + TX_STATE);
    printf("TX_STATE      : %d\n",val);
    val = read_bypass(BP3_REGFILE + CPU2FIFO_CNT);
    printf("CPU2FIFO_CNT  : %d\n",val);
    val = read_bypass(BP3_REGFILE + FIFO2SNN_CNT);
    printf("FIFO2SNN_CNT  : %d\n",val);
    val = read_bypass(BP3_REGFILE + SNN2FIFO_CNT);
    printf("SNN2FIFO_CNT  : %d\n",val);
    val = read_bypass(BP3_REGFILE + FIFO2CPU_CNT);
    printf("FIFO2CPU_CNT  : %d\n",val);
    val = read_bypass(BP3_REGFILE + SEND_LEN);
    printf("SEND_LEN      : %d\n",val);
    val = read_bypass(BP3_REGFILE + CTRL_REG);
    printf("CTRL_REG      : %d\n",val);
    val = read_bypass(BP3_REGFILE + WDATA_1);
    val2 = read_bypass(BP3_REGFILE + WDATA_2);
    printf("WDATA         : %08x%08x\n",val2,val);
    val = read_bypass(BP3_REGFILE + RDATA_1);
    val2 = read_bypass(BP3_REGFILE + RDATA_2);
    printf("RDATA         : %08x%08x\n",val2,val);
    val = read_bypass(BP3_REGFILE + DATA_CNT);
    printf("DATA_CNT      : %d\n",val);
    val = read_bypass(BP3_REGFILE + TLAST_CNT);
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

int SendFrame(char *filename, uint64_t size)
{
    int rc;
    char *buffer = NULL;
    uint32_t val = 0;

    write_bypass(BP3_REGFILE + SEND_LEN, size >> 3);

    posix_memalign((void **)&buffer, 8, size);
    read_bin_to_buffer(filename, buffer, size);

    rc = send_dma(buffer, size);
    
    while(val == 0)
    {
        val = read_bypass(BP3_REGFILE + TX_STATE);
    }
    write_bypass(BP3_REGFILE + TX_STATE,0);
    // printf("send %d bytes.\n",rc);
    // show_reg_status();
    return rc;
}

void RecvFrame(char *filename, uint64_t size)
{
    int rc;
    char *read_buffer = NULL;
    uint32_t val = 1;

    write_bypass(BP3_REGFILE + RX_STATE,1);

    posix_memalign((void **)&read_buffer, 8, size);
    rc = read_dma(read_buffer, size);
    
    while(val == 1)
    {
        val = read_bypass(BP3_REGFILE + RX_STATE);
    }
    write_bypass(BP3_REGFILE + RX_STATE,0);

    writeBin(filename, read_buffer, size);

    // printf("read %d bytes.\n",rc);
    // show_reg_status();
}

char* configFramePath;    // config frame path.
char* initFramePath;      //   init frame path.
char* workFramePath  ;    //   work frame path.
char* outputFramePath;    // output frame path.
int mode;                 // mode0: config mode, mode1: work mode.

uint64_t configFrmNum;
uint64_t initFrmNum  ;
uint64_t workFrmNum  ;
uint64_t outputFrmNum;

int parse_args(int argc, char *argv[]);

int main(int argc, char *argv[]){
    int rc;
    parse_args(argc,argv);

    if(pcie_init() < 0){
        printf("error");
        return -1;
    }

    write_bypass(BP3_REGFILE + CPU2FIFO_CNT, 0);
    write_bypass(BP3_REGFILE + FIFO2SNN_CNT, 0);
    write_bypass(BP3_REGFILE + SNN2FIFO_CNT, 0);
    write_bypass(BP3_REGFILE + FIFO2CPU_CNT, 0);
    write_bypass(BP3_REGFILE + CTRL_REG, 1);
    write_bypass(BP3_REGFILE + CTRL_REG, 0);

    char *sendFramePath;
    uint64_t sendFrmNum;
    if(mode == 0) {
        rc = SendFrame(configFramePath, configFrmNum << 3);
        // printf("\nsend %d bytes.\n",rc);
        if(rc == configFrmNum << 3)
            printf("\nConfig Done.\n\n");
        else
            printf("\nConfig Fail.\n\n");
    }
    else {
        sendFramePath = workFramePath;
        sendFrmNum = workFrmNum;

        write_bypass(BP3_REGFILE + oFrmNum_REG, outputFrmNum);
        write_bypass(BP3_REGFILE + CTRL_REG, 2);
        rc = SendFrame(initFramePath, initFrmNum << 3);
        write_bypass(BP3_REGFILE + CTRL_REG, 0);
        rc = SendFrame(workFramePath, workFrmNum << 3);
        RecvFrame(outputFramePath, outputFrmNum << 3);
    }

    pcie_deinit();
    return 0;
}

int parse_args(int argc, char *argv[]) {
    const struct option table[] = {
        {"mode"     , required_argument, NULL, 'm'},
        {"cFrmNum"  , required_argument, NULL, 'x'},
        {"iFrmNum"  , required_argument, NULL, 'u'},
        {"wFrmNum"  , required_argument, NULL, 'e'},
        {"oFrmNum"  , required_argument, NULL, 'p'},
        {"cpath"    , required_argument, NULL, 'c'},
        {"ipath"    , required_argument, NULL, 'i'},
        {"wpath"    , required_argument, NULL, 'w'},
        {"opath"    , required_argument, NULL, 'o'},
        {0          , 0                , NULL,  0 },
    };

    int o;
    while ( (o = getopt_long(argc, argv, "-d:i:", table, NULL)) != -1) {
        switch (o) {
            case 'm': 
                mode            = atol(optarg)  ; 
                // printf("Now is working in mode %d\n" , mode);            
                break;
            case 'x': 
                configFrmNum    = atol(optarg)  ; 
                // printf("set configFrmNum    == %ld\n" , configFrmNum);            
                break;
            case 'u': 
                initFrmNum    = atol(optarg)  ; 
                // printf("set initFrmNum      == %ld\n" , initFrmNum);            
                break;
            case 'e': 
                workFrmNum      = atol(optarg)  ; 
                // printf("set workFrmNum      == %ld\n" , workFrmNum);            
                break;
            case 'p': 
                outputFrmNum    = atol(optarg)  ; 
                // printf("set outputFrmNum    == %ld\n" , outputFrmNum);            
                break;
            case 'c': 
                configFramePath = optarg   ; 
                // printf("set configFramePath == %s\n" , configFramePath); 
                break;
            case 'i': 
                initFramePath = optarg   ; 
                // printf("set initFramePath   == %s\n" , initFramePath); 
                break;
            case 'w': 
                workFramePath   = optarg   ; 
                // printf("set workFramePath   == %s\n" , workFramePath );  
                break;
            case 'o': 
                outputFramePath = optarg   ; 
                // printf("set outputFramePath == %s\n" , outputFramePath); 
                break;
        }
    }
    return 0;
}
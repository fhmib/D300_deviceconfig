#ifndef _DC_MONITOR_H
#define _DC_MONITOR_H

#define CONFIG_FILE             "./init.sh"
#define DEVINFO_FILE            "./devinfo"

#define DEVNAME                 "/dev/mem"
#define FPGA_ADDR_BASE          0x42000000
#define BB_BASE_ADDR            0x8000
#define AD9361_BASE_ADDR        0xF000

#define ADDR_9361_SPI_BUSY      (AD9361_BASE_ADDR+(0x3FF<<2))

#define _FPGA_IO_(ZZ)           (*((int*)(g_FPGA_pntr+ZZ)))

int dc_cfg_nodeid(void*, int);
int dc_cfg_nodename(void*, int);
int dc_cfg_freq(void*, int);
int dc_cfg_tx1(void*, int);
int dc_cfg_tx2(void*, int);
int dc_cfg_ipaddr(void*, int);
int dc_cfg_ipmask(void*, int);
int dc_cfg_ipgate(void*, int);
int dc_cfg_rtc(void*, int);
int dc_cfg_btyvol(void*, int);
int dc_cfg_btytype(void*, int);

int ad9361_read_bb(int);
int ad9361_write_bb(int, int);
int ad9361_read(int);
int ad9361_write(int, int);
int drvFPGA_Init(int*);
int drvFPGA_Close(int*);
int drvFPGA_Read(int);
int drvFPGA_Write(int, int);

#endif

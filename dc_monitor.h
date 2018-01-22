#ifndef _DC_MONITOR_H
#define _DC_MONITOR_H

#define CONFIG_FILE             "./init.sh"
#define DEVINFO_FILE            "./devinfo"
#define FPGA_FILE               "./fpgadown.sh"

#define DEVNAME                 "/dev/mem"
#define FPGA_ADDR_BASE          0x42000000
#define BB_BASE_ADDR            0x8000
#define AD9361_BASE_ADDR        0xF000

#define ADDR_9361_SPI_BUSY      (AD9361_BASE_ADDR+(0x3FF<<2))

#define _FPGA_IO_(ZZ)           (*((int*)(g_FPGA_pntr+ZZ)))

//config functions
int dc_cfg_hmver(void*, int);
int dc_cfg_nlver(void*, int);
int dc_cfg_rtver(void*, int);
int dc_cfg_ipver(void*, int);
int dc_cfg_dcver(void*, int);
int dc_cfg_fpgaver(void*, int);
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

//common functions
int mod_infile(const char*, const char*, const char*, const char*, const char*);
int chk_num(char*);
int chk_str(char*);
int chk_diff_from_dmsg(const char*, const char*);
int update_data_msg(const char*, const char*);

//bottom functions
int ad9361_read_bb(int);
int ad9361_write_bb(int, int);
int ad9361_read(int);
int ad9361_write(int, int);
int drvFPGA_Init(int*);
int drvFPGA_Close(int*);
int drvFPGA_Read(int);
int drvFPGA_Write(int, int);

#endif

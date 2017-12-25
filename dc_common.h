#ifndef _DC_COMMON_H
#define _DC_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/sem.h>

#define EPT                 fprintf
#define ASSERT              assert

#define U8                  unsigned char
#define U16                 unsigned short
#define U32                 unsigned int

#define MADR                U8
#define PATH_CREAT_KEY      "/etc/profile"          /*path name for creatint all queue, can be modified*/
#define PNAME_DEVCFG        "devcfg"
#define PNAME_BOA           "boa"
#define SN_DEVCFG           20
#define SN_DC_THREAD        21
#define SN_BOA              25
#define QUEUE_R             0400
#define QUEUE_W             0200
#define QUEUE_MODE          (QUEUE_R |QUEUE_W |QUEUE_W>>3 |QUEUE_W>>6)
#define MAX_NODE_CNT        32
#define MAX_MSG_BUF         8192
#define SND_MSG_LEN         MAX_MSG_BUF-sizeof(long)

#define DNAME_NDID          "NodeId"
#define DNAME_NDNAME        "NodeName"
#define DNAME_FREQ          "CentreFreq"
#define DNAME_BW            "ChannelBandwidth"
#define DNAME_TX1           "TX1Power"
#define DNAME_TX2           "TX2Power"
#define DNAME_RSAN0         "Rssi_Ant0"
#define DNAME_RSAN1         "Rssi_Ant1"
#define DNAME_IPADDR        "IpAddress"
#define DNAME_IPMASK        "IpMask"
#define DNAME_IPGATE        "IpGateway"
#define DNAME_RTC           "HaveRTC"
#define DNAME_BTYVOL        "BatteryVoltage"
#define DNAME_BTYTYPE       "BatteryType"

typedef enum _GB_MSG_TYPE
{
    MMSG_DC_BOAREAD = 5000,
    MMSG_DC_BOAWRITE,
    MMSG_DC_SNDBOA,
    MMSG_DC_REQ
}GB_MSG;

typedef enum _DC_MSG_TYPE
{
    DC_SND_BOA_DATA = 1,
    DC_SND_BOA_REQ
}DC_MSG;

typedef struct _qinfo_t
{
    char    pname[64];
    int     sub;
    key_t   key_q;
    int     qid;
}qinfo_t;

typedef struct _trans_data
{
    char    name[64];
    char    enable;
    char    isstr;
    char*   pvalue;
    int     (*opera)(void*);
}trans_data;

//struct for the data sharing among thread
typedef struct _dc_tshare_t
{
    pthread_mutex_t      mutex;
    pthread_cond_t       cond;
    int                  qr_run;
    int                  cfg_run;
}dc_tshare_t;

typedef struct _mmsg_t
{
    long    mtype;
    MADR    node;
    char    data[MAX_MSG_BUF];
}mmsg_t;
#define MMSG_LEN        sizeof(mmsg_t)

typedef struct _dcmsg_t
{
    long    type;
    char    data[SND_MSG_LEN];
}dcmsg_t;

//a table stores all device parameters
//no use
typedef struct _device_info_t
{
    int     node_id;                //self node id 
    char    node_name[64];          //self node name
 
    float   freq;                   //centre frequency
    float   bw;                     //bandwidth
    int     tx1_power;
    int     tx2_power;
    int     rssi_ant0[32];
    int     rssi_ant1[32];
 
    char    ip_addr[16];
    char    ip_mask[16];
    char    ip_gate[16];
 
    int     have_rtc;
    int     battery_vol;
    int     battery_type;
 
/*
    //GPS
    int     valid;
    int     time;
    int     date;
    char    fix_type[16];
    int     num_sats;
    float   lat_pos;
    char    lat_hem[16];
    float   long_pos;
    char    long_hem[16];
    float   speed;
    float   course;
    float   height;
    float   accuracy;
    float   vaccuracy;
    float   geoid_height;
*/
}device_info_t;

int     dc_init(void);
void*   dc_qrv_thread(void*);
void*   dc_cfg_thread(void*);
int     dc_rmsg_proc(int, void*);
//void    dc_info_init(void);
void    dc_msg_malloc(void);
void    dc_mem_free(void);
int     dc_msg_to_boa(mmsg_t*, U16);

void    dc_snd2boa(void);
int     add_data(char*, int);
void    dc_config(int, void*);
//for test
void    write_data_for_test(void);

int     dc_get_qids(void* arg);
int     dc_queues_delete(void);
int     dc_msg_send(int, void*, int);
//int dc_strstr(char* find, char* dest);

#endif

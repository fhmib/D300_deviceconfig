#include "dc_common.h"
#include "dc_monitor.h"

extern int qs;
extern int re_qin, dc_qid, boa_qid;
extern qinfo_t qinfs[];
extern int cnt_p;
extern char pname[64];
extern dc_tshare_t dc_share;

int read_first;             //the variable must be set 1 before config.
trans_data data_msg[] =
{
    {DNAME_FPGAVER, 0, 1, NULL, dc_cfg_fpgaver},
    {DNAME_HMVER, 0, 1, NULL, dc_cfg_hmver},
    {DNAME_NLVER, 0, 1, NULL, dc_cfg_nlver},
    {DNAME_RTVER, 0, 1, NULL, dc_cfg_rtver},
    {DNAME_IPVER, 0, 1, NULL, dc_cfg_ipver},
    {DNAME_DCVER, 0, 1, NULL, dc_cfg_dcver},
    {DNAME_NDID, 0, 0, NULL, dc_cfg_nodeid},
    {DNAME_NDNAME, 0, 1, NULL, dc_cfg_nodename},
    {DNAME_FREQ, 0, 0, NULL, dc_cfg_freq},
    {DNAME_TX1, 0, 0, NULL, dc_cfg_tx1},
    {DNAME_TX2, 0, 0, NULL, dc_cfg_tx2},
    //{DNAME_RSAN0, 0, 0, NULL, NULL},
    //{DNAME_RSAN1, 0, 0, NULL, NULL},
    {DNAME_IPADDR, 0, 1, NULL, dc_cfg_ipaddr},
    {DNAME_IPMASK, 0, 1, NULL, dc_cfg_ipmask},
    {DNAME_IPGATE, 0, 1, NULL, dc_cfg_ipgate},
    {DNAME_RTC, 0, 1, NULL, dc_cfg_rtc},
    {DNAME_BTYVOL, 0, 0, NULL, dc_cfg_btyvol},
    {DNAME_BTYTYPE, 0, 1, NULL, dc_cfg_btytype},
    {DNAME_SNDRATE, 0, 1, NULL, dc_cfg_sndrate},
    {DNAME_RCVRATE, 0, 1, NULL, dc_cfg_rcvrate},

    {DNAME_NTXIPPKT, 0, 1, NULL, dc_cfg_nowtxippkt},
    {DNAME_NRXIPPKT, 0, 1, NULL, dc_cfg_nowrxippkt},
    {DNAME_NTXIPERR, 0, 1, NULL, dc_cfg_nowtxiperr},
    {DNAME_NRXIPERR, 0, 1, NULL, dc_cfg_nowrxiperr},
    {DNAME_NTXIPBYTE, 0, 1, NULL, dc_cfg_nowtxipbyte},
    {DNAME_NRXIPBYTE, 0, 1, NULL, dc_cfg_nowrxipbyte},

    {DNAME_PTXIPPKT, 0, 1, NULL, dc_cfg_pretxippkt},
    {DNAME_PRXIPPKT, 0, 1, NULL, dc_cfg_prerxippkt},
    {DNAME_PTXIPERR, 0, 1, NULL, dc_cfg_pretxiperr},
    {DNAME_PRXIPERR, 0, 1, NULL, dc_cfg_prerxiperr},
    {DNAME_PTXIPBYTE, 0, 1, NULL, dc_cfg_pretxipbyte},
    {DNAME_PRXIPBYTE, 0, 1, NULL, dc_cfg_prerxipbyte},

    {DNAME_SETUART1, 0, 1, NULL, NULL},

    {DNAME_UART1SPEED, 0, 0, NULL, dc_read_Uart1Speed},
    {DNAME_UART1FLOW, 0, 1, NULL, dc_read_Uart1Flow},
    {DNAME_UART1DATA, 0, 0, NULL, dc_read_Uart1Data},
    {DNAME_UART1STOP, 0, 1, NULL, dc_read_Uart1Stop},
    {DNAME_UART1PARITY, 0, 1, NULL, dc_read_Uart1Parity}
};
const int data_msg_cnt = sizeof(data_msg)/sizeof(data_msg[0]);
#define DATA_CNT        sizeof(data_msg)/sizeof(data_msg[0])
const int data_cfg_len = sizeof(dc_cfg)*DATA_CNT;
dc_cfg data_cfg[DATA_CNT];  //use for config thread
int data_cfg_cnt;           //indecate how many configurations to be configured
int send_seq;
int cfg_flag;               //thread signal

double rcv_rate, snd_rate;
double pre_snd, pre_rcv;
double now_snd, now_rcv;
char *pre_rcv_ippkt, *pre_rcv_iperr, *pre_rcv_ipbyte;
char *now_rcv_ippkt, *now_rcv_iperr, *now_rcv_ipbyte;
char *pre_snd_ippkt, *pre_snd_iperr, *pre_snd_ipbyte;
char *now_snd_ippkt, *now_snd_iperr, *now_snd_ipbyte;

static pthread_t mrx_tid;
static pthread_t net_tid;
static pthread_t cfg_tid;

MADR sa;                //self address
//int dt_qid;             //deviceconfig internal msg queue id

int main(int argc, char* argv[])
{
    int rval = 0;
    int stop;

    memset(pname, 0, sizeof(pname));
    strcpy(pname, argv[0]);

    if(argc != 2){
        rval = 1;
        goto process_return;
    }
    sa = atoi(argv[1]);

    rval = dc_init();
    if(rval) goto process_return;

    rval = dc_get_qids(argv[0]);
    if(rval != 0){
        EPT(stderr, "process %s:create get queue function fails\n", argv[0]);
        rval = 2;
        goto process_return;
    }
    sleep(1);
    while(qs == 0){
        sleep(1);
    }

    rval = pthread_create(&mrx_tid, NULL, dc_qrv_thread, &qinfs[re_qin].qid);
    if(rval != 0){
        EPT(stderr, "%s:can not open create msg receiving thread\n", argv[0]);
        rval = 3;
        goto process_return;
    }

    rval = pthread_create(&net_tid, NULL, dc_net_thread,NULL);
    if(rval != 0){
        EPT(stderr, "%s:can not open create net thread\n", argv[0]);
        rval = 3;
        goto process_return;
    }
    rval = pthread_create(&cfg_tid, NULL, dc_cfg_thread, &boa_qid);
    if(rval != 0){
        EPT(stderr, "%s:can not open config thread\n", argv[0]);
        rval = 3;
        goto process_return;
    }

    stop = 0;
    pthread_mutex_lock(&dc_share.mutex);
    while(stop == 0)
    {
        EPT(stderr, "%s:waiting for the exit of sub threads\n", argv[0]);
        pthread_cond_wait(&dc_share.cond, &dc_share.mutex);
        EPT(stderr, "%s:share.qr_run = %d, share.cfg_run = %d\n", argv[0], dc_share.qr_run, dc_share.cfg_run);
        if(dc_share.qr_run == 0 || dc_share.cfg_run == 0 || dc_share.net_run == 0){
            if(dc_share.qr_run == 0)
                EPT(stderr, "msg receiving thread quit\n");
            if(dc_share.cfg_run == 0)
                EPT(stderr, "config thread quit\n");
            if(dc_share.net_run == 0)
                EPT(stderr, "net thread quit\n");
            stop = 1;
            continue;
        }
    }
    pthread_mutex_unlock(&dc_share.mutex);

process_return:
    sleep(1);
    dc_queues_delete();
    dc_mem_free();
    exit(rval);
}

void* dc_qrv_thread(void* arg)
{
    int qid, rcnt;
    mmsg_t rx_msg;
    int rval = 0, stop;

    pthread_detach(pthread_self());

    qid = *(int*)arg;
    ASSERT(qid == qinfs[re_qin].qid);
    if(qid < 0){
        EPT(stderr, "%s:wrong receiving queue id:%d\n", qinfs[re_qin].pname, qid);
        rval = 1;
        goto thread_return;
    }

    rval = 0;
    stop = 0;
    while(0 == stop)
    {
        memset(&rx_msg, 0, sizeof(rx_msg));
        rcnt = msgrcv(qid, &rx_msg, MAX_MSG_BUF, 0, 0);
        if(rcnt < 0){
            if(EIDRM != errno){
                EPT(stderr, "%s:error in receiving msg, no:%d, meaning[%s]\n", qinfs[re_qin].pname, errno, strerror(errno));
                continue;
            }
            else{
                EPT(stderr, "%s:quit msg receiving thread\n", qinfs[re_qin].pname);
                rval = 2;
                break;
            }
        }
        rval = dc_rmsg_proc(rcnt, &rx_msg);
        if(rval != 0){
            stop = 1;
        }
    }

thread_return:
    pthread_mutex_lock(&dc_share.mutex);
    dc_share.qr_run = 0;
    pthread_cond_signal(&dc_share.cond);
    pthread_mutex_unlock(&dc_share.mutex);

    sleep(1);
    pthread_exit((void*)&rval);
}

/*
 * function:
 *      according to type of msg to choose opposite function
 * patameters:
 *      len:                msg length
 *      data:               msg body
 * return:
 *      0:                  success
 *      other:              failure
 */
int dc_rmsg_proc(int len, void* data)
{
    int rval = 0;
    mmsg_t* rmsg;

    rmsg = (mmsg_t*)data;

    if(len <= (int)sizeof(MADR)){
        EPT(stderr, "%s:reveive a message with wrong length:%d\n", __func__, len);
        rval = 1;
        goto func_exit;
    }
    if(rmsg->seq < 0 ||  rmsg->seq > 99){
        EPT(stderr, "%s:receive a message with wrong seq num:%d\n", __func__, rmsg->seq);
        rval = 2;
        goto func_exit;
    }

    switch(rmsg->mtype)
    {
        case MMSG_DC_BOAREAD:
            dc_read_2boa(rmsg->data, rmsg->seq, len-sizeof(MADR));
            break;
        case MMSG_DC_BOAWRITE:
            dc_write_cfg(rmsg->data, rmsg->seq, len-sizeof(MADR));
            break;
        default:
            EPT(stderr, "%s:receive unknown msg type, no = %ld\n", __func__, rmsg->mtype);
            break;
    }

func_exit:
    return rval;
}

/*
 * function:
 *      create deviceconfig interal queue, initialize some parameters and so on
 * return:
 *      0:              success
 *      1:              failure
 */
int dc_init()
{
    //key_t key;
    int rval = 0;

    read_first = 0;
    cfg_flag = 0;
    data_cfg_cnt = 0;
    rcv_rate = 0;
    snd_rate = 0;

    now_rcv_ippkt = (char*)malloc(16);
    memset(now_rcv_ippkt, 0, 16);
    now_rcv_iperr = (char*)malloc(16);
    memset(now_rcv_iperr, 0, 16);
    now_rcv_ipbyte = (char*)malloc(16);
    memset(now_rcv_ipbyte, 0, 16);
    now_snd_ippkt = (char*)malloc(16);
    memset(now_snd_ippkt, 0, 16);
    now_snd_iperr = (char*)malloc(16);
    memset(now_snd_iperr, 0, 16);
    now_snd_ipbyte = (char*)malloc(16);
    memset(now_snd_ipbyte, 0, 16);

    pre_rcv_ippkt = (char*)malloc(16);
    memset(pre_rcv_ippkt, 0, 16);
    pre_rcv_iperr = (char*)malloc(16);
    memset(pre_rcv_iperr, 0, 16);
    pre_rcv_ipbyte = (char*)malloc(16);
    memset(pre_rcv_ipbyte, 0, 16);
    pre_snd_ippkt = (char*)malloc(16);
    memset(pre_snd_ippkt, 0, 16);
    pre_snd_iperr = (char*)malloc(16);
    memset(pre_snd_iperr, 0, 16);
    pre_snd_ipbyte = (char*)malloc(16);
    memset(pre_snd_ipbyte, 0, 16);

    dc_msg_malloc();

#ifndef LINUX_TEST
    if(uart_init(UART1_PATH)){
        rval = 1;
        goto func_exit;
    }
#endif

    //for test
    //write_data_for_test();
/*
    key = ftok(PATH_CREAT_KEY, SN_DC_THREAD);
    dt_qid = msgget(key, IPC_CREAT | QUEUE_MODE);
    EPT(stderr, "node[%d] create dt_qid = %d, key = %x", sa, dt_qid, key);
    if(-1 == dt_qid){
        EPT(stderr, "%s:can not create devcfg internal queue\n", qinfs[re_qin].pname);
        rval = 1;
        goto func_exit;
    }
*/
func_exit:
    return rval;
}

void dc_msg_malloc()
{
    int i, len;

    len = 0;
    for(i = 0; i < data_msg_cnt; i++)
    {
        if(0 == strcmp(DNAME_HMVER, data_msg[i].name)){
            len = 64;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_NLVER, data_msg[i].name)){
            len = 64;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_RTVER, data_msg[i].name)){
            len = 64;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_IPVER, data_msg[i].name)){
            len = 64;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_DCVER, data_msg[i].name)){
            len = 64;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_FPGAVER, data_msg[i].name)){
            len = 64;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_NDID, data_msg[i].name)){
            len = 2;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_NDNAME, data_msg[i].name)){
            len = 64;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_FREQ, data_msg[i].name)){
            len = 8;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        /*
        else if(0 == strcmp(DNAME_BW, data_msg[i].name)){
            len = 4;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_TFCI, data_msg[i].name)){
            len = 4;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_MMODE, data_msg[i].name)){
            len = 6;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        */
        else if(0 == strcmp(DNAME_TX1, data_msg[i].name)){
            len = 4;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_TX2, data_msg[i].name)){
            len = 4;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_RSAN0, data_msg[i].name)){
            len = 256;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_RSAN1, data_msg[i].name)){
            len = 256;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_IPADDR, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_IPMASK, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_IPGATE, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_RTC, data_msg[i].name)){
            len = 1;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_BTYVOL, data_msg[i].name)){
            len = 8;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_BTYTYPE, data_msg[i].name)){
            len = 8;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_SNDRATE, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_RCVRATE, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }

        else if(0 == strcmp(DNAME_NTXIPPKT, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_NRXIPPKT, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_NTXIPERR, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_NRXIPERR, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_NTXIPBYTE, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_NRXIPBYTE, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }

        else if(0 == strcmp(DNAME_PTXIPPKT, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_PRXIPPKT, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_PTXIPERR, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_PRXIPERR, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_PTXIPBYTE, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_PRXIPBYTE, data_msg[i].name)){
            len = 16;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_SETUART1, data_msg[i].name)){
            len = 32;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_UART1SPEED, data_msg[i].name)){
            len = 8;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_UART1FLOW, data_msg[i].name)){
            len = 1;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_UART1DATA, data_msg[i].name)){
            len = 1;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_UART1STOP, data_msg[i].name)){
            len = 1;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
        else if(0 == strcmp(DNAME_UART1PARITY, data_msg[i].name)){
            len = 1;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }

        else{
            //something new that be forgeted to added in this func will use default len = 64
            len = 64;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
    }
}

/*
 * function:
 *      free the memory of data_msg
 */
void dc_mem_free()
{
    int i;
    
    free(now_rcv_ippkt);
    free(now_rcv_iperr);
    free(now_rcv_ipbyte);
    free(now_snd_ippkt);
    free(now_snd_iperr);
    free(now_snd_ipbyte);

    free(pre_rcv_ippkt);
    free(pre_rcv_iperr);
    free(pre_rcv_ipbyte);
    free(pre_snd_ippkt);
    free(pre_snd_iperr);
    free(pre_snd_ipbyte);

    for(i = 0; i < data_msg_cnt; i++)
    {
        free(data_msg[i].pvalue);
    }
    return;
}
/*
 * function:
 *      according the message from boa to config the machine
 * parameters:
 *      arg:                boa queue id
 */
void* dc_cfg_thread(void* arg)
{
    int qid;
    int rval = 0;

    pthread_detach(pthread_self());

    qid = *(int*)arg;
    if(qid < 0){
        rval = 1;
        EPT(stderr, "%s:boa_qid = %d\n", __func__, qid);
        goto thread_return;
    }

    while(1)
    {
        if(cfg_flag > 0){
            rval = dc_cfg_func(qid);
        }
        else{
            sleep(1);
        }
    }

thread_return:
    pthread_mutex_lock(&dc_share.mutex);
    dc_share.cfg_run = 0;
    pthread_cond_signal(&dc_share.cond);
    pthread_mutex_unlock(&dc_share.mutex);

    sleep(1);
    pthread_exit((void*)&rval);
}

void *dc_net_thread(void *arg)
{
    FILE *fp = NULL;
    int rval = 0;
    int len;
    int i = 0;
    int read_ip_t = 0;
    char *pos, *p;

    pthread_detach(pthread_self());

    arg = NULL;
    now_rcv = 0;
    now_snd = 0;
    while(1){

        pre_rcv = now_rcv;
        pre_snd = now_snd;
        //fprintf(stderr, "pre_rcv:%lf\t pre_snd:%lf\n", pre_rcv, pre_snd);
        pthread_mutex_lock(&dc_share.net_mutex);
        fp = fopen(NET_PATH, "r");
        if(fp == NULL){
            fprintf(stderr, "can not open "  NET_PATH "\n");
            rval = 1;
            goto thread_return;
        }

        char buf[1024];
        memset(buf, 0, 1024);
        len = fread(buf, 1, 1024, fp);
        
        pos = strstr(buf, NET_DEV_NAME);
        if(pos == NULL){
            fprintf(stderr, "%s:can not find dev:" NET_DEV_NAME "\n", __func__);
            fclose(fp);
            fp = NULL;
            sleep(1);
            continue;
        }

        i = 0;
        for(p = strtok(pos, " \n\t\r"); p; p = strtok(NULL, " \n\t\r")){
            i++;
            switch(i){
                case 2:
                    if(read_ip_t == UPDATE_INTER){
                        strcpy(pre_rcv_ipbyte, now_rcv_ipbyte);
                        strcpy(now_rcv_ipbyte, p);
                        //EPT(stderr, "20180209:%s\n", p);
                    }
                    now_rcv = atod(p);
                    break;

                case 3:
                    if(read_ip_t == UPDATE_INTER){
                        strcpy(pre_rcv_ippkt, now_rcv_ippkt);
                        strcpy(now_rcv_ippkt, p);
                    }
                    break;

                case 4:
                    if(read_ip_t == UPDATE_INTER){
                        strcpy(pre_rcv_iperr, now_rcv_iperr);
                        strcpy(now_rcv_iperr, p);
                    }
                    break;

                case 10:
                    if(read_ip_t == UPDATE_INTER){
                        strcpy(pre_snd_ipbyte, now_snd_ipbyte);
                        strcpy(now_snd_ipbyte, p);
                    }
                    now_snd = atod(p);
                    break;

                case 11:
                    if(read_ip_t == UPDATE_INTER){
                        strcpy(pre_snd_ippkt, now_snd_ippkt);
                        strcpy(now_snd_ippkt, p);
                    }
                    break;

                case 12:
                    if(read_ip_t == UPDATE_INTER){
                        strcpy(pre_snd_iperr, now_snd_iperr);
                        strcpy(now_snd_iperr, p);
                    }
                    break;


            }
            /*
            if(i == 2)
                now_rcv = atod(p);
            if(i == 10){
                now_snd = atod(p);
                break;
            }
            */
        }

        now_rcv = now_rcv/(1024/8);
        now_snd = now_snd/(1024/8);
        //fprintf(stderr, "now_rcv:%lf\t now_snd:%lf\n", now_rcv, now_snd);

        snd_rate = now_snd - pre_snd;
        rcv_rate = now_rcv - pre_rcv;
        pthread_mutex_unlock(&dc_share.net_mutex);
        fclose(fp);
        fp = NULL;
        read_ip_t++;
        if(read_ip_t > UPDATE_INTER) read_ip_t = 1;
        sleep(1);
    }

thread_return:
    pthread_mutex_lock(&dc_share.net_mutex);
    dc_share.net_run = 0;
    pthread_cond_signal(&dc_share.cond);
    pthread_mutex_unlock(&dc_share.net_mutex);

    sleep(1);
    pthread_exit((void*)&rval);
}

int dc_msg_to_boa(mmsg_t* snd_msg, U16 length)
{
    if(dc_msg_send(boa_qid, snd_msg, length) > 0){
        EPT(stderr, "%s:send to boa wrong\n", __func__);
        return 1;
    }
    return 0;
}


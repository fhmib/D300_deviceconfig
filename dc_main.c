#include "dc_common.h"

extern int qs;
extern int re_qin, dc_qid, boa_qid;
extern qinfo_t qinfs[];
extern int cnt_p;
extern char pname[64];
extern dc_tshare_t dc_share;

extern trans_data data_msg[];
extern int data_cnt;
extern int read_first;

static pthread_t mrx_tid;
static pthread_t cfg_tid;

MADR sa;                //self address
int dt_qid;             //deviceconfig internal msg queue id

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

    dc_init();

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

    rval = pthread_create(&cfg_tid, NULL, dc_cfg_thread, &boa_qid);
    if(rval != 0){
        EPT(stderr, "%s:can not open config thread\n", argv[0]);
        rval = 4;
        goto process_return;
    }

    stop = 0;
    pthread_mutex_lock(&dc_share.mutex);
    while(stop == 0)
    {
        EPT(stderr, "%s:waiting for the exit of sub threads\n", argv[0]);
        pthread_cond_wait(&dc_share.cond, &dc_share.mutex);
        EPT(stderr, "%s:share.qr_run = %d, share.cfg_run = %d\n", argv[0], dc_share.qr_run, dc_share.cfg_run);
        if(dc_share.qr_run == 0 || dc_share.cfg_run == 0){
            if(dc_share.qr_run == 0)
                EPT(stderr, "msg receiving thread quit\n");
            if(dc_share.cfg_run == 0)
                EPT(stderr, "config thread quit\n");
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
    pthread_mutex_unlock(&dc_share.mutex);
    sleep(1);
    pthread_exit((void*)&rval);
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
    key_t key;
    int rval = 0;

    read_first = 0;

    dc_msg_malloc();

    key = ftok(PATH_CREAT_KEY, SN_DC_THREAD);
    dt_qid = msgget(key, IPC_CREAT | QUEUE_MODE);
    EPT(stderr, "node[%d] create dt_qid = %d, key = %x", sa, dt_qid, key);
    if(-1 == dt_qid){
        EPT(stderr, "%s:can not create devcfg internal queue\n", qinfs[re_qin].pname);
        rval = 1;
        goto func_exit;
    }

func_exit:
    return rval;
}

void dc_msg_malloc()
{
    int i, len;

    len = 0;
    for(i = 0; i < data_cnt; i++)
    {
        if(0 == strcmp(DNAME_NDID, data_msg[i].name)){
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
        else if(0 == strcmp(DNAME_BW, data_msg[i].name)){
            len = 4;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
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
        else{
            //something new you forget to added in this func, use default len = 64
            len = 64;
            data_msg[i].pvalue = (char*)malloc(sizeof(char)*len);
            memset(data_msg[i].pvalue, 0, len);
        }
    }
}

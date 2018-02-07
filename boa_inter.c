#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
//#include <sys/ipc.h>
#include <sys/msg.h>

#define EPT                 fprintf
#define MAX_MSG_BUF         8192
#define PATH_CREAT_KEY      "/etc/profile"          /*path name for creatint all queue, can be modified*/
#define SN_DEVCFG           20
#define SN_DC_THREAD        21
#define SN_BOA              25
#define QUEUE_R             0400
#define QUEUE_W             0200
#define QUEUE_MODE          (QUEUE_R |QUEUE_W |QUEUE_W>>3 |QUEUE_W>>6)

typedef struct _mmsg_t
{
    long    mtype;
    int     seq;
    char    data[MAX_MSG_BUF];
}mmsg_t;

typedef enum _GB_MSG_TYPE
{
    MMSG_DC_BOAREAD = 5000,
    MMSG_DC_BOAWRITE,
    MMSG_DC_SNDBOA,                 //occupy 5002-5101
    MMSG_DC_RET = 5102,
}GB_MSG;

int read_from_dc(char*);
int send_to_dc(char*);

int main()
{
    char buf[1024];
    int rval;

    strcpy(buf, "HighMacVersion FPGAVersion NetLayerVersion RoutingVersion If2TcpIpVersion DeviceConfigVersion NodeName NodeId SendRate ReceiveRate\0");
    rval = read_from_dc(buf);
    if(!rval){
        EPT(stderr, "%s:read failed\n", __FILE__);
    }
#if 0
    strcpy(buf, "{\n\t\"NodeId\": 1,\n\t\"NodeName\": \" rzxt_mech[1]\",\n\t\"HighMacVersion\": \"highmac_0124\",\n\t\"NetLayerVersion\": \"netlayer_0124\",\n\t\"RoutingVersion\": \"routingp_0124\",\n\t\"If2TcpIpVersion\": \"if2tcpip_0124\",\n\t\"DeviceConfigVersion\": \"devcfg_0124\",\n\t\"FPGAVersion\": \"FPGAVER_0124\"\n}\0");
    rval = send_to_dc(buf);
    if(rval == -1){
        EPT(stderr, "%s:write failed\n", __FILE__);
    }
    else if(rval == 0){
        EPT(stderr, "%s:write success\n", __FILE__);
    }
    else if(rval > 0){
        EPT(stderr, "%s:write incomplete success, these configurations can not write[%s]\n", __FILE__, buf);
    }
#endif

    return 0;
}

/*
 * function:
 *      communicate with devcfg process to obtain values according to parameter.
 * parameters:
 *      buf:                string of names that web desire to obtain, such as 
 *                          "NodeId NodeName TX1Power", and it stores names and
 *                          values packing with JSON after the funcion returned,
 *                          such as "
 *                          {
 *                              "NodeId": 2,
 *                              "NodeName": "rzxt_mech[2]",
 *                              "TX1Power": 10
 *                          }"
 * return:
 *      0:                  read failed
 *      other:              length of buf.
 */
int read_from_dc(char *buf)
{
    int boa_qid = -1;
    int dc_qid = -1;
    key_t boa_key_q, dc_key_q;
    mmsg_t msg_data, rcv_data;
    int len = 0;
    int i;

    memset(&msg_data, 0, sizeof(msg_data));
    memset(&rcv_data, 0, sizeof(rcv_data));
    srand((unsigned)time(NULL));

    boa_key_q = ftok(PATH_CREAT_KEY, SN_BOA);
    boa_qid = msgget(boa_key_q, QUEUE_MODE);
    dc_key_q = ftok(PATH_CREAT_KEY, SN_DEVCFG);
    dc_qid = msgget(dc_key_q, QUEUE_MODE);

    if(-1 == boa_qid || -1 == dc_qid){
        EPT(stderr, "%s:open message queue failed\n", __func__);
        return 0;
    }

    msg_data.mtype = MMSG_DC_BOAREAD;
    msg_data.seq = rand()%100;
    len += sizeof(int);
    strcpy(msg_data.data, buf);
    len += strlen(msg_data.data);
    msgsnd(dc_qid, &msg_data, len, 0);
    i = 0;
    while(1){
        if(-1 == msgrcv(boa_qid, &rcv_data, MAX_MSG_BUF, MMSG_DC_SNDBOA+msg_data.seq, IPC_NOWAIT)){
            //EPT(stderr, "%s:no msg from the queue\n", __func__);
            i++;
            if(i > 5) return 0;
            sleep(1);
            continue;
        }
        else break;
    }
    strcpy(buf, rcv_data.data);
    len = strlen(rcv_data.data);
    EPT(stderr, "%s:mtype:%ld seq:%d data:\n%s\n", __FILE__, rcv_data.mtype, rcv_data.seq, rcv_data.data);

    return len;
}

/*
 * function:
 *      sends names and values that web desire to config to devcfg process.
 * parameters:
 *      buf:                string of names and values that web wanna config,
 *                          such as a JSON fromat, and store names of configurations
 *                          which config failed, such as "NodeId NodeName"
 * return:
 *      -1:                 config failed
 *      0:                  config all configurations success
 *      1:                  config part of configurations success, names of configurations which config failed are saved in parameter 'buf'
 */
int send_to_dc(char *buf)
{
    int boa_qid = -1;
    int dc_qid = -1;
    key_t boa_key_q, dc_key_q;
    mmsg_t msg_data, rcv_data;
    int len, i, wrong_num;

    memset(&msg_data, 0, sizeof(msg_data));
    memset(&rcv_data, 0, sizeof(rcv_data));

    boa_key_q = ftok(PATH_CREAT_KEY, SN_BOA);
    boa_qid = msgget(boa_key_q, QUEUE_MODE);
    dc_key_q = ftok(PATH_CREAT_KEY, SN_DEVCFG);
    dc_qid = msgget(dc_key_q, QUEUE_MODE);

    if(-1 == boa_qid || -1 == dc_qid){
        EPT(stderr, "%s:open message queue failed\n", __func__);
        return 0;
    }

    len = 0;
    msg_data.mtype = MMSG_DC_BOAWRITE;
    msg_data.seq = 1;
    len += sizeof(int);
    strcpy(msg_data.data, buf);
    len += strlen(msg_data.data);
    msgsnd(dc_qid, &msg_data, len, 0);
    i = 0;
    while(1){
        if(-1 == msgrcv(boa_qid, &rcv_data, MAX_MSG_BUF, MMSG_DC_RET, IPC_NOWAIT)){
            //EPT(stderr, "%s:no msg from the queue\n", __func__);
            i++;
            if(i > 5) return -1;
            sleep(1);
            continue;
        }
        else break;
    }
    if(rcv_data.data[0] == 0){
        EPT(stderr, "%s:write to device failed\n", __func__);
        return -1;
    }
    else if(rcv_data.data[0] == 1){
        return 0;
    }
    else{
        wrong_num = *(int*)(rcv_data.data+1);
        len = strlen(buf);
        assert(len >= strlen(rcv_data.data+1+sizeof(int)));
        memset(buf, 0, len);
        strcpy(buf, rcv_data.data+1+sizeof(int));
        //EPT(stderr, "%s:wrong_num:%d buf:%s\n", __FILE__, wrong_num, buf);
        return wrong_num;
    }
}

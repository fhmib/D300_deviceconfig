#include "dc_common.h"
#include "dc_monitor.h"

extern trans_data data_msg[];
extern int data_msg_cnt;
extern int data_len;
extern dc_cfg data_cfg[];
extern int data_cfg_cnt;
extern int send_seq;
extern int cfg_flag;
extern int read_first;
extern dc_tshare_t dc_share;
extern MADR sa;

//ensure uart config once
int cfg_uart;

int dc_cfg_func(int arg)
{
    int qid, i, count;
    int rval = 0;
    int len = 0;
    int send_num = 0;;                          //ensure sending package's num according to cfg_flag.
    int wrong_cnt = 0;
    char wrong_buf[1024];                       //store names of options which configured failed.
#ifndef LINUX_TEST
    int fd;
#endif

    mmsg_t snd_msg;
    cfg_node* node_list_h = NULL;              //head of node list
    cfg_node* node_list_p = NULL;              //current position of node list
    memset(wrong_buf, 0, sizeof(wrong_buf));

    qid = arg;
    if(qid < 0){
        EPT(stderr, "%s:boa_qid is invalid\n", __func__);
        rval = 1;
        goto func_exit;
    }
    ASSERT(cfg_flag > 0);

    pthread_mutex_lock(&dc_share.mutex);

    send_num = cfg_flag;
    if(read_first == 0){
        cfg_flag = 0;
        data_cfg_cnt = 0;
        EPT(stderr, "%s:must read first!\n", __func__);
        rval = 3;
        pthread_mutex_unlock(&dc_share.mutex);
        goto func_exit;
    }
    if(data_cfg_cnt <= 0){
        cfg_flag = 0;
        EPT(stderr, "%s:no msg!\n", __func__);
        rval = 2;
        pthread_mutex_unlock(&dc_share.mutex);
        goto func_exit;
    }
    cfg_flag = 0;
    cfg_uart = 0;
    count = data_cfg_cnt;
    //EPT(stderr, "%s:I'm ready for compared with data_msg!\n", __func__);
    for(i = 0; i < count; i++)
    {
        rval = cmpwith_data_msg(data_cfg[i].name, data_cfg[i].value);
        //EPT(stderr, "***********%d**********\n", rval);
        if(rval == 1){
            //EPT(stderr, "%s:%s's new value is same to old\n", __func__, data_cfg[i].name);
            if(wrong_cnt) strcat(wrong_buf, " ");
            strcat(wrong_buf, data_cfg[i].name);
            wrong_cnt++;
            continue;
        }
        if(rval == 2){
            rval = 3;
            EPT(stderr, "%s:wrong name[%s],function can not continue because it may causes errors\n", __func__, data_msg[i].name);
            pthread_mutex_unlock(&dc_share.mutex);
            goto func_exit;
        }

        if(NULL == node_list_h){
            node_list_h = (cfg_node*)malloc(NODE_LENTH);
            memset(node_list_h, 0, NODE_LENTH);
            node_list_p = node_list_h;
        }
        else{
            node_list_p->next = (cfg_node*)malloc(NODE_LENTH);
            memset(node_list_p->next, 0, NODE_LENTH);
            node_list_p = node_list_p->next;
        }
        strcpy(node_list_p->name, data_cfg[i].name);
        strcpy(node_list_p->value, data_cfg[i].value);
    }

    rval = 0;

#ifndef LINUX_TEST
    rval = drvFPGA_Init(&fd);
    if(rval){
        EPT(stderr, "%s:initialize drvFPGA failed\n", __func__);
        pthread_mutex_unlock(&dc_share.mutex);
        goto func_exit;
    }
#endif
    node_list_p = node_list_h;
    while(1){
        if(NULL == node_list_p)
            break;
        pthread_mutex_lock(&dc_share.net_mutex);
        for(i = 0; i < data_msg_cnt; i++){
            if(0 == strcmp(node_list_p->name, data_msg[i].name)){
                if(data_msg[i].opera){
                    rval = data_msg[i].opera(node_list_p->value, 1);
                    if(rval){
                        EPT(stderr, "%s:config %s failed\n", __func__, data_msg[i].name);
                        if(wrong_cnt) strcat(wrong_buf, " ");
                        strcat(wrong_buf, data_msg[i].name);
                        wrong_cnt++;
                    }
                    break;
                }
                else{
                    EPT(stderr, "%s:no matching operation for %s\n", __func__, node_list_p->name);
                    break;
                }
            }
        }
        pthread_mutex_unlock(&dc_share.net_mutex);
        node_list_p = node_list_p->next;
    }
    rval = 0;
#ifndef LINUX_TEST
    drvFPGA_Close(&fd);
#endif
    pthread_mutex_unlock(&dc_share.mutex);

    memset(&snd_msg, 0, sizeof(snd_msg));
    snd_msg.mtype = MMSG_DC_RET;
    snd_msg.seq = send_seq;
    len += sizeof(int);
    //EPT(stderr, "rval:%d, wrong_cnt:%d\n", rval, wrong_cnt);
    snd_msg.data[0] = wrong_cnt;
    len += 1;
    *(int*)(snd_msg.data+1) = wrong_cnt;
    len += sizeof(int);
    strcpy(snd_msg.data+1+sizeof(int), wrong_buf);
    len += strlen(wrong_buf);
    //EPT(stderr, "snd_msg.data[0]:%d\n", (int)snd_msg.data[0]);
    for(; send_num > 0; send_num--){
        dc_msg_to_boa(&snd_msg, len);
    }

    //EPT(stderr, "%s: cfg_uart = %d\n", __func__, cfg_uart);
    ASSERT(cfg_uart == 0);
/*
    node_list_p = node_list_h;
    while(NULL != node_list_p)
    {
        EPT(stderr, "name:%s value:%s\n", node_list_p->name, node_list_p->value);
        node_list_p = node_list_p->next;
    }
*/
func_exit:
    //free mem
    while(1)
    {
        if(NULL == node_list_h)
            break;
        node_list_p = node_list_h->next;
        free(node_list_h);
        if(NULL == node_list_p){
            node_list_h = NULL;
            break;
        }
        else{
            node_list_h = node_list_p;
        }
    }

    if(rval){
        EPT(stderr, "%s:config failed. rval = %d\n", __func__, rval);
        memset(&snd_msg, 0, sizeof(snd_msg));
        snd_msg.mtype = MMSG_DC_RET;
        snd_msg.seq = send_seq;
        len += sizeof(int);
        snd_msg.data[0] = 111;
        len += strlen(snd_msg.data);
        for(; send_num > 0; send_num--){
            dc_msg_to_boa(&snd_msg, len);
        }
    }

    return rval;
}

/*
 * function:
 *      judge if the value from boa is the same with data_msg
 * parameters:
 *      index:              index of data_cfg
 * return:
 *      0:                  diff
 *      1:                  same
 *      2:                  not find the name of data_msg
 */
int cmpwith_data_msg(char* name, char* value)
{
    int i;
    int rval = 0;

    for(i = 0; i < data_msg_cnt; i++)
    {
        if(0 == strcmp(name, data_msg[i].name)){
            if(0 == strcmp(DNAME_RCVRATE, name)) goto func_exit;
            if(0 == strcmp(DNAME_SNDRATE, name)) goto func_exit;
            if(0 == strcmp(value, data_msg[i].pvalue)){
                //EPT(stderr, "[%s] [%s]\n", value, data_msg[i].pvalue);
                rval = 1;
                goto func_exit;
            }
            else{
                if((0 == strcmp(DNAME_UART1SPEED, name)) || (0 == strcmp(DNAME_UART1FLOW, name)) || (0 == strcmp(DNAME_UART1DATA, name)) || (0 == strcmp(DNAME_UART1STOP, name)) || (0 == strcmp(DNAME_UART1PARITY, name))){
                    cfg_uart++;
                    //EPT(stderr, "%s: cfg_uart = %d\n", __func__, cfg_uart);
                }
                goto func_exit;
            }
        }
    }

    EPT(stderr, "%s:can not find the name from data_msg:%s\n", __func__, name);
    rval = 2;

func_exit:
    return rval;
}

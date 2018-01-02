#include "dc_common.h"

extern trans_data data_msg[];
extern int data_msg_cnt;
extern int data_len;
extern dc_cfg data_cfg[];
extern int data_cfg_cnt;
extern int cfg_flag;
extern int read_first;
extern dc_tshare_t dc_share;
extern MADR sa;

int dc_cfg_func(int arg)
{
    int qid, i, count;
    int rval = 0;
    int len = 0;
    mmsg_t snd_msg;
    cfg_node* node_list_h = NULL;              //head of node list
    cfg_node* node_list_p = NULL;              //current position of node list

    qid = arg;
    if(qid < 0){
        EPT(stderr, "%s:boa_qid is invalid\n", __func__);
        rval = 1;
        goto func_exit;
    }
    ASSERT(cfg_flag > 0);

    pthread_mutex_lock(&dc_share.mutex);

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
    count = data_cfg_cnt;
    //EPT(stderr, "%s:I'm ready for update data_msg!\n", __func__);
    for(i = 0; i < count; i++)
    {
        rval = update_data_msg(data_cfg[i].name, data_cfg[i].value);
        if(rval)
            continue;

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

    pthread_mutex_unlock(&dc_share.mutex);

    node_list_p = node_list_h;
    while(1){
        if(NULL == node_list_p)
            break;
        for(i = 0; i < data_msg_cnt; i++){
            if(0 == strcmp(node_list_p->name, data_msg[i].name)){
                rval = data_msg[i].opera(node_list_p->value, 1);
                break;
            }
        }
        node_list_p = node_list_p->next;
    }

    memset(&snd_msg, 0, sizeof(snd_msg));
    snd_msg.mtype = MMSG_DC_RET;
    snd_msg.node = sa;
    len += sizeof(MADR);
    dc_msg_to_boa(&snd_msg, len);
/*
    node_list_p = node_list_h;
    while(NULL != node_list_p)
    {
        EPT(stderr, "name:%s value:%s\n", node_list_p->name, node_list_p->value);
        node_list_p = node_list_p->next;
    }
*/
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

func_exit:
    return rval;
}

/*
 * function:
 *      judge if the value from boa is the same with data_msg and update the different data
 * parameters:
 *      index:              index of data_cfg
 * return:
 *      0:                  diff
 *      1:                  same
 *      2:                  not find the name of data_msg
 */
int update_data_msg(char* name, char* value)
{
    int i;
    int rval;

    for(i = 0; i < data_msg_cnt; i++)
    {
        if(0 == strcmp(name, data_msg[i].name)){
            if(0 == strcmp(value, data_msg[i].pvalue)){
                rval = 1;
                goto func_exit;
            }
            else{
                strcpy(data_msg[i].pvalue, value);
                rval = 0;
                goto func_exit;
            }
        }
    }

    EPT(stderr, "%s:can not find the name from data_msg:%s\n", __func__, name);
    rval = 2;

func_exit:
    return rval;
}

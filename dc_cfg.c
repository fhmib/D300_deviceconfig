#include "dc_common.h"

extern trans_data data_msg[];
extern int data_msg_cnt;
extern int data_len;
extern dc_cfg data_cfg[];
extern int data_cfg_cnt;
extern int cfg_flag;
extern dc_tshare_t dc_share;

int dc_cfg_func(int arg)
{
    int qid, i, count;
    int rval = 0;
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

    if(data_cfg_cnt <= 0){
        pthread_mutex_unlock(&dc_share.mutex);
        EPT(stderr, "%s:no msg!\n", __func__);
        rval = 2;
        goto func_exit;
    }
    cfg_flag = 0;
    count = data_cfg_cnt;
    for(i = 0; i < count; i++)
    {
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
    while(NULL != node_list_p)
    {
        EPT(stderr, "name:%s value:%s\n", node_list_p->name, node_list_p->value);
        node_list_p = node_list_p->next;
    }

    //free mem
    while(1)
    {
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

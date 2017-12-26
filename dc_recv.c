#include "dc_common.h"

//extern device_info_t *dc_info;
extern MADR sa;
extern dc_tshare_t dc_share;

int read_first;             //the variable must be set 1 before config.
trans_data data_msg[] =
{
    {DNAME_NDID, 0, 0, NULL, NULL},
    {DNAME_NDNAME, 0, 1, NULL, NULL},
    {DNAME_FREQ, 0, 0, NULL, NULL},
    {DNAME_BW, 0, 0, NULL, NULL},
    {DNAME_TX1, 0, 0, NULL, NULL},
    {DNAME_TX2, 0, 0, NULL, NULL},
    {DNAME_RSAN0, 0, 0, NULL, NULL},
    {DNAME_RSAN1, 0, 0, NULL, NULL},
    {DNAME_IPADDR, 0, 1, NULL, NULL},
    {DNAME_IPMASK, 0, 1, NULL, NULL},
    {DNAME_IPGATE, 0, 1, NULL, NULL},
    {DNAME_RTC, 0, 0, NULL, NULL},
    {DNAME_BTYVOL, 0, 0, NULL, NULL},
    {DNAME_BTYTYPE, 0, 0, NULL, NULL}
};
const int data_cnt = sizeof(data_msg)/sizeof(data_msg[0]);

int dc_read_2boa(void* arg, int lenth)
{
    int len = 0;
    mmsg_t snd_msg;
    int rval = 0;
    char name[64];
    char* buf;
    char ch;
    int i, pos1, pos2;

    if(lenth <= 0){
        EPT(stderr, "%s:receive msg with wrong format\n",__func__);
        rval = 1;
        goto func_exit;
    }
    buf = (char*)arg;

    snd_msg.mtype = MMSG_DC_SNDBOA;
    snd_msg.node = sa;
    len += sizeof(MADR);
    len += sizeof(long);

    pthread_mutex_lock(&dc_share.mutex);

    for(pos1 = 0; pos1 < lenth; pos1++)
    {
        memset(name, 0, sizeof(name));
        ch = *(buf+pos1);
        if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')){
            for(pos2 = 1;;pos2++)
            {
                ch = *(buf+pos1+pos2);
                if(ch == '-' || ch == '_' || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')){
                    continue;
                }
                else
                    break;
            }
            strncpy(name, buf+pos1, pos2);
            EPT(stderr, "%s:name:%s\n", __func__, name);
            for(i = 0; i < data_cnt; i++)
            {
                if(0 == strcmp(data_msg[i].name, name)){
                    data_msg[i].enable = 1;
                }
            }
            pos1 += (pos2 - 1);
        }
    }

    //update device_info;
    //rval = update_data_msg();
    
    //for test
    write_data_for_test();

    //put data to snd_msg.data
    rval = add_data(snd_msg.data, MAX_MSG_BUF);
    EPT(stderr, "\nI'm in %s,%d, data content:", __func__, __LINE__);
    EPT(stderr, "\n%s\n", snd_msg.data);
    len += strlen(snd_msg.data);

    pthread_mutex_unlock(&dc_share.mutex);

    //send message to boa
    dc_msg_to_boa(&snd_msg, len);
    
    read_first = 1;

func_exit:
    return rval;
}

/*
 * function:
 *      put data_msg to arg
 * parameters:
 *      arg:        data buffer
 *      length:        max size of arg
 * return:
 *      0:          success
 *      1:          buf filled
 *      2:          buf length out of para-length
 */
int add_data(char* arg, int length)
{
    int i, len, fstval;
    char* buf;
    int rval = 0;

    len = 0;
    fstval = 0;
    buf = (char*)malloc(8192*sizeof(char));
    memset(buf, 0, sizeof(char)*8192);
    len += sprintf(buf+len, "{\n\t");
    for(i = 0; i < data_cnt; i++)
    {
        if(data_msg[i].enable == 0){
            continue;
        }

        if(fstval == 0){
            fstval = 1;
        }
        else{
            len += sprintf(buf+len, ",\n\t");
        }

        if(data_msg[i].isstr != 0){
            len += sprintf(buf+len, "\"%s\": \"%s\"", data_msg[i].name, data_msg[i].pvalue);
        }
        else{
            len += sprintf(buf+len, "\"%s\": %s", data_msg[i].name, data_msg[i].pvalue);
        }
        data_msg[i].enable = 0;

        if(len > 8192){
        EPT(stderr, "%s:buf filled! len = %d\n", __func__, len);
            rval = 1;
            goto func_exit;
        }
    }
    len += sprintf(buf+len, "\n}");
    if(len > length){
        EPT(stderr, "%s:data length out of range. len = %d, range = %d\n", __func__, len, length);
        rval = 2;
        goto func_exit;
    }
    strcpy(arg, buf);
func_exit:
    return rval;
}

void write_data_for_test()
{
    /*
    int i;
    for(i = 0; i < data_cnt; i++)
    {
        data_msg[i].enable = 1;
    }
    */
    strcpy(data_msg[0].pvalue, "3");
    strcpy(data_msg[1].pvalue, "RZXT_NODE[3]");
    strcpy(data_msg[2].pvalue, "2155.5");
    strcpy(data_msg[3].pvalue, "2.5");
    strcpy(data_msg[4].pvalue, "ab12");
    strcpy(data_msg[5].pvalue, "1122");
    strcpy(data_msg[6].pvalue, "[12, 41, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ]");
    strcpy(data_msg[7].pvalue, "[12, 41, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ]");
    strcpy(data_msg[8].pvalue, "192.168.3.240");
    strcpy(data_msg[9].pvalue, "255.255.255.0");
    strcpy(data_msg[10].pvalue, "192.168.3.1");
    strcpy(data_msg[11].pvalue, "1");
    strcpy(data_msg[12].pvalue, "123.4");
    strcpy(data_msg[13].pvalue, "215");
    return;
}

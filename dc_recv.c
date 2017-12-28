#include "dc_common.h"

//extern device_info_t *dc_info;
extern MADR sa;
extern int dt_qid;
extern int cfg_flag;
extern int data_cfg_cnt;
extern dc_tshare_t dc_share;
extern trans_data data_msg[];
extern int data_msg_cnt;
extern int data_cfg_len;
extern int read_first;
extern dc_cfg data_cfg[];

/*
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
*/
/*
 * function:
 *      according to msg from boa, the function reads the information that boa wants and sends them to boa
 * parameters:
 *      arg:                receive data from boa
 *      length:             data length
 * return:
 *      0:                  success
 *      other:              failure
 */
int dc_read_2boa(void* arg, int length)
{
    int len = 0;
    mmsg_t snd_msg;
    int rval = 0;
    char name[64];
    char* buf;
    char ch;
    int i, pos1, pos2;

    if(length <= 0){
        EPT(stderr, "%s:receive msg with wrong format\n",__func__);
        rval = 1;
        goto func_exit;
    }
    buf = (char*)arg;

    memset(&snd_msg, 0, sizeof(snd_msg));
    snd_msg.mtype = MMSG_DC_SNDBOA;
    snd_msg.node = sa;
    len += sizeof(MADR);

    pthread_mutex_lock(&dc_share.mutex);

    for(pos1 = 0; pos1 < length; pos1++)
    {
        ch = *(buf+pos1);
        if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')){
            memset(name, 0, sizeof(name));
            for(pos2 = 1;;pos2++)
            {
                ch = *(buf+pos1+pos2);
                if(ch == '.' || ch == '-' || ch == '_' || ch == '[' || ch == ']' || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')){
                    continue;
                }
                else
                    break;
            }
            strncpy(name, buf+pos1, pos2);
            //EPT(stderr, "%s:name:%s\n", __func__, name);
            for(i = 0; i < data_msg_cnt; i++)
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
    //write_data_for_test();

    //put data to snd_msg.data
    rval = add_data(snd_msg.data, MAX_MSG_BUF);
    //EPT(stderr, "\nI'm in %s,%d,sending data content:", __func__, __LINE__);
    //EPT(stderr, "\n%s\n", snd_msg.data);
    len += strlen(snd_msg.data);

    //send message to boa
    dc_msg_to_boa(&snd_msg, len);

    read_first = 1;

    pthread_mutex_unlock(&dc_share.mutex);

func_exit:
    return rval;
}

/*
 * function:
 *      analyzes the msg from boa and sends them to deviceconfig internal queue
 * parameters:
 *      arg:                receive data from boa
 *      length:             data length
 * return:
 *      0:                  success
 *      other:              failure
 */
int dc_write_cfg(void* arg, int length)
{
    int rval = 0;
    char* buf;
    char ch, temp[8];
    int pos1, pos2, pos_t;
    //int judge;
    //char name_tmp[64];
    char string[256];
    
    if(length <= 0){
        EPT(stderr, "%s:receive msg with wrong format\n",__func__);
        rval = 1;
        goto func_exit;
    }
    buf = (char*)arg;

    pthread_mutex_lock(&dc_share.mutex);

    if(cfg_flag > 2){
        rval = 2;
        EPT(stderr, "%s:data_cfg thread waited so long or write frequently. config operation stopped\n",__func__);
        pthread_mutex_unlock(&dc_share.mutex);
        goto func_exit;
    }
    memset((char*)data_cfg, 0, data_cfg_len);
    data_cfg_cnt = 0;

    for(pos1 = 0; pos1 < length; pos1++)
    {
        ch = *(buf+pos1);
        if(ch == '"'){
            data_cfg_cnt++;
            memset(string, 0, sizeof(string));
            for(pos2 = 1;; pos2++)
            {
                ch = *(buf+pos1+pos2);
                if(ch != '"'){
                    continue;
                }
                else
                    break;
            }
            if(pos2 < 2){
                EPT(stderr, "%s,%d:wrong format\n", __func__, __LINE__);
                rval = 3;
                goto func_exit;
            }
            strncpy(string, buf+pos1+1, pos2-1);
            pos1 += pos2;
        }
        else if(ch == '['){
            data_cfg_cnt++;
            pos_t = 0;
            memset(string, 0, sizeof(string));
            memset(temp, 0, sizeof(temp));
            string[0] = '[';
            for(pos2 = 1;; pos2++)
            {
                ch = *(buf+pos1+pos2);
                if(ch == '.' || ch == '-' || ch == '_' || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')){
                    temp[pos_t++] = ch;
                    continue;
                }
                else if(ch == ','){
                    if(pos_t == 0){
                        continue;
                    }
                    else{
                        strcat(string, temp);
                        strcat(string, ", ");
                        pos_t = 0;
                        memset(temp, 0, sizeof(temp));
                        continue;
                    }
                }
                else if(ch == ']'){
                    if(pos_t == 0){
                        break;
                    }
                    else{
                        strcat(string, temp);
                        strcat(string, "]");
                        pos_t = 0;
                        memset(temp, 0, sizeof(temp));
                        break;
                    }
                }

                if(pos2 >= 256){
                    EPT(stderr, "%s,%d:wrong format\n", __func__, __LINE__);
                    rval = 3;
                    goto func_exit;
                }
            }
            pos1 += pos2;
        }
        else if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')){
            data_cfg_cnt++;
            memset(string, 0, sizeof(string));
            for(pos2 = 1;; pos2++)
            {
                ch = *(buf+pos1+pos2);
                if(ch == '.' || ch == '-' || ch == '_' || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')){
                    continue;
                }
                else
                    break;
            }
            strncpy(string, buf+pos1, pos2);
            pos1 += (pos2 - 1);
        }
        else{
            continue;
        }

        if(1 == (data_cfg_cnt%2)){
            if(64 < strlen(string)){
                EPT(stderr, "%s:wrong name str\n", __func__);
                data_cfg_cnt = 0;
                break;
            }
            //memset(name_tmp, 0, sizeof(name_tmp));
            //strcpy(name_tmp, string);
            //EPT(stderr, "name_tmp:%s\n", name_tmp);
            strcpy(data_cfg[(data_cfg_cnt-1)/2].name, string);
        }
        else{
            //judge the same between data from boa and old data
            //judge = data_cfg_judge(name_tmp, string);
            //if(judge){
            //    data_cfg_cnt -= 2;
            //    continue;
            //}
            //strcpy(data_cfg[(data_cfg_cnt-1)/2].name, name_tmp);
            strcpy(data_cfg[(data_cfg_cnt-1)/2].value, string);
        }
    }

    data_cfg_cnt = data_cfg_cnt/2;
    if(data_cfg_cnt > 0){
        cfg_flag++;
    }
    /*
    for(pos1 = 0; pos1 < data_cfg_cnt; pos1++)
    {
        EPT(stderr, "%s %s\n", data_cfg[pos1].name, data_cfg[pos1].value);
    }
    */
    pthread_mutex_unlock(&dc_share.mutex);

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
    for(i = 0; i < data_msg_cnt; i++)
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
/*
int data_cfg_judge(char* name, char* value)
{
    int i;

    for(i = 0; i < data_msg_cnt; i++)
    {
        if(0 == strcmp(name, data_msg[i].name)){
            if(0 == strcmp(value, data_msg[i].pvalue))
                return 1;
            else
                return 0;
        }
    }
    EPT(stderr, "%s:can not find the name from data_msg:%s\n", __func__, name);
    return 2;
}
*/

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
    strcpy(data_msg[6].pvalue, "[12, 41, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]");
    strcpy(data_msg[7].pvalue, "[12, 41, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]");
    strcpy(data_msg[8].pvalue, "192.168.3.240");
    strcpy(data_msg[9].pvalue, "255.255.255.0");
    strcpy(data_msg[10].pvalue, "192.168.3.1");
    strcpy(data_msg[11].pvalue, "1");
    strcpy(data_msg[12].pvalue, "123.4");
    strcpy(data_msg[13].pvalue, "215");
    return;
}

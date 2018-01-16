#include "dc_common.h"
#include "dc_monitor.h"

extern trans_data data_msg[];
extern int data_msg_cnt;

/*
 * function:
 *      read/write node id
 * parameters:
 *
 *      mode:   0:      put the value from device into data_msg
 *
 *      mode:   1:      config parameter
 *      arg:            the value caller want to config
 *
 * return:
 *      0:              success
 *      1:              failure
 */
int dc_cfg_nodeid(void* arg, int mode) 
{
    int rval = 0;
    FILE* fp = NULL;
    char line[512];
    int i;
    int len;

    if(0 == mode){
        char value[4];

        fp = fopen(CONFIG_FILE, "r");
        if(NULL == fp){
            rval = 1;
            EPT(stderr, "%s:can not open config file\n", __func__);
            goto func_exit;
        }
        while(!feof(fp)){
            memset(line, 0, sizeof(line));
            memset(value, 0, sizeof(value));
            if(NULL == fgets(line, sizeof(line), fp))
                continue;
            if(NULL == strstr(line, "node_id="))
                continue;

            sscanf(line, "%*[^0-9]%[0-9]", value);
            //EPT(stderr, "%s:value[%s]\n", __func__, value);
            len = strlen(value);
            if(0 == len || 2 < len){
                EPT(stderr, "%s:get nodeid fail\n", __func__);
                rval = 2;
                fclose(fp);
                fp = NULL;
                goto func_exit;
            }
            if(2 == len && value[0] == '0'){
                value[0] = value[1];
                value[1] = 0;
            }
            for(i = 0; i < data_msg_cnt; i++){
                if(0 == strcmp(data_msg[i].name, DNAME_NDID)){
                    memset(data_msg[i].pvalue, 0, 2);
                    strcpy(data_msg[i].pvalue, value);
                    fclose(fp);
                    fp = NULL;
                    goto func_exit;
                }
                else
                    continue;
            }
        }
    }
    else{
        int size;
        char* p_value = (char*)arg;
        char config_path[] = CONFIG_FILE;
        char* buf;
        char* pos;

        len = strlen(p_value);
        if(2 == len && *p_value == '0'){
            *p_value = *(p_value + 1);
            *(p_value + 1) = 0;
        }
        size = file_size(config_path);
        //EPT(stderr, "%s:file length = %d\n", __func__, size);
        buf = (char*)malloc(sizeof(char)*(size+2));
        memset(buf, 0, sizeof(char)*(size+2));
        fp = fopen(CONFIG_FILE, "r");
        while(!feof(fp)){
            memset(line, 0, sizeof(line));
            if(NULL == fgets(line, sizeof(line), fp)){
                continue;
            }
            if(line[0] == '#'){
                strcat(buf, line);
                continue;
            }
            pos = strstr(line, "node_id=");
            if(NULL == pos){
                strcat(buf, line);
                continue;
            }
            pos += strlen("node_id=");
            strcpy(pos, p_value);
            pos += strlen(p_value);
            *pos++ = '\n';
            *pos = 0;
            strcat(buf, line);
        }
        //EPT(stderr, "*****************buf****************\n");
        //EPT(stderr, "%s\n", buf);
        //EPT(stderr, "*****************buf****************\n");
        fclose(fp);
        fp = fopen(CONFIG_FILE, "w");
        fprintf(fp, "%s", buf);

        free(buf);
        fclose(fp);
        fp = NULL;
    }

func_exit:
    return rval;
}

int dc_cfg_nodename(void* arg, int mode)
{
    int rval = 0;
    FILE *fp = NULL;
    char line[512];
    char value[64];
    int len, i;

    if(0 == mode){
        char *pos;
        //EPT(stderr, "%s:I'm in\n", __func__);
        fp = fopen(DEVINFO_FILE, "r");
        //EPT(stderr, "%s:I'm in\n", __func__);
        if(fp == NULL){
            EPT(stderr, "%s:can not open devinfo file\n", __func__);
            rval = 1;
            goto func_exit;
        }
        while(!feof(fp)){
            memset(line, 0, sizeof(line));
            memset(value, 0, sizeof(value));
            if(NULL == fgets(line, sizeof(line), fp)){
                continue;
            }
            if(NULL == strstr(line, "NodeName=")){
                continue;
            }
            pos = line;
            while(1){
                if('=' == *pos){
                    pos++;
                    break;
                }
                else{
                    pos++;
                    continue;
                }
            }
            strncpy(value, pos, 64);
            len = strlen(value);
            if(len >= 64 || len <= 0){
                EPT(stderr, "%s:wrong name\n", __func__);
                rval = 2;
                fclose(fp);
                fp = NULL;
                goto func_exit;
            }
            while(' ' == value[0]){
                strcpy(value, value+1);
                len--;
            }
            while((' ' == value[len-1]) || ('\n' == value[len-1])){
                value[len-1] = 0;
                len--;
            }
            for(i = 0; i < data_msg_cnt; i++){
                if(0 == strcmp(data_msg[i].name, DNAME_NDNAME)){
                    memset(data_msg[i].pvalue, 0, 2);
                    strcpy(data_msg[i].pvalue, value);
                    fclose(fp);
                    fp = NULL;
                    goto func_exit;
                }
                else
                    continue;
            }
        }
    }

func_exit:
    return rval;
}

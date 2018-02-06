#include "dc_common.h"
#include "dc_monitor.h"

extern trans_data data_msg[];
extern int data_msg_cnt;
extern MADR sa;
extern double rcv_rate;
extern double snd_rate;

void *g_FPGA_pntr;

int dc_cfg_hmver(void *arg, int mode)
{
    int rval = 0;
    FILE *fp;
    char value[64];
    char line[512];
    int len, i;
    char *pos;

    if(mode == 0){
        fp = fopen(CONFIG_FILE, "r");
        if(NULL == fp){
            rval = 1;
            EPT(stderr, "%s:can not open config file\n", __func__);
            goto func_exit;
        }
        memset(value, 0, sizeof(value));
        while(!feof(fp)){
            memset(line, 0, sizeof(line));
            if(NULL == fgets(line, sizeof(line), fp))
                continue;
            if(NULL == strstr(line, "highmac"))
                continue;

            //jump after ' '
            len = strlen(line);
            for(i = 0; i < len; i++){
                if(line[i] == ' ')
                    continue;
                else
                    break;
            }
            if(line[i] == '#')
                continue;

            pos = strstr(line, "./");
            pos += strlen("./");
            i = 0;
            while(1){
                if(' ' == *pos)
                    break;
                if(i >= 64){
                    EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
                    rval = 2;
                    fclose(fp);
                    fp = NULL;
                    goto func_exit;
                }
                value[i++] = *pos++;
            }
            for(i = 0; i < data_msg_cnt; i++){
                if(0 == strcmp(data_msg[i].name, DNAME_HMVER)){
                    memset(data_msg[i].pvalue, 0, 64);
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
        len = strlen((char*)arg);
        if(len < 1 || len > 64){
            EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
            rval = 2;
            goto func_exit;
        }
        memset(value, 0, sizeof(value));
        strcpy(value, (char*)arg);

        rval = chk_str(value);
        if(rval){
            rval = 2;
            goto func_exit;
        }
        rval = chk_diff_from_dmsg(DNAME_HMVER, value);
        if(rval){
            rval = 3;
            goto func_exit;
        }

        rval = mod_infile(CONFIG_FILE, "highmac", "./", " ", value);

        if(rval){
            rval = 4;
            EPT(stderr, "%s:modify file failed, rval = %d\n", __func__, rval);
            goto func_exit;
        }

        rval = update_data_msg(DNAME_HMVER, value);
    }

func_exit:
    return rval;
}

int dc_cfg_nlver(void *arg, int mode)
{
    int rval = 0;
    FILE *fp;
    char value[64];
    char line[512];
    int len, i;
    char *pos;

    if(mode == 0){
        fp = fopen(CONFIG_FILE, "r");
        if(NULL == fp){
            rval = 1;
            EPT(stderr, "%s:can not open config file\n", __func__);
            goto func_exit;
        }
        memset(value, 0, sizeof(value));
        while(!feof(fp)){
            memset(line, 0, sizeof(line));
            if(NULL == fgets(line, sizeof(line), fp))
                continue;
            if(NULL == strstr(line, "netlayer"))
                continue;

            //jump after ' '
            len = strlen(line);
            for(i = 0; i < len; i++){
                if(line[i] == ' ')
                    continue;
                else
                    break;
            }
            if(line[i] == '#')
                continue;

            pos = strstr(line, "./");
            pos += strlen("./");
            i = 0;
            while(1){
                if(' ' == *pos)
                    break;
                if(i >= 64){
                    EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
                    rval = 2;
                    fclose(fp);
                    fp = NULL;
                    goto func_exit;
                }
                value[i++] = *pos++;
            }
            for(i = 0; i < data_msg_cnt; i++){
                if(0 == strcmp(data_msg[i].name, DNAME_NLVER)){
                    memset(data_msg[i].pvalue, 0, 64);
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
        len = strlen((char*)arg);
        if(len < 1 || len > 64){
            EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
            rval = 2;
            goto func_exit;
        }
        memset(value, 0, sizeof(value));
        strcpy(value, (char*)arg);

        rval = chk_str(value);
        if(rval){
            rval = 2;
            goto func_exit;
        }
        rval = chk_diff_from_dmsg(DNAME_NLVER, value);
        if(rval){
            rval = 3;
            goto func_exit;
        }

        rval = mod_infile(CONFIG_FILE, "netlayer", "./", " ", value);

        if(rval){
            rval = 4;
            EPT(stderr, "%s:modify file failed, rval = %d\n", __func__, rval);
            goto func_exit;
        }

        rval = update_data_msg(DNAME_NLVER, value);
    }

func_exit:
    return rval;
}

int dc_cfg_rtver(void *arg, int mode)
{
    int rval = 0;
    FILE *fp;
    char value[64];
    char line[512];
    int len, i;
    char *pos;

    if(mode == 0){
        fp = fopen(CONFIG_FILE, "r");
        if(NULL == fp){
            rval = 1;
            EPT(stderr, "%s:can not open config file\n", __func__);
            goto func_exit;
        }
        memset(value, 0, sizeof(value));
        while(!feof(fp)){
            memset(line, 0, sizeof(line));
            if(NULL == fgets(line, sizeof(line), fp))
                continue;
            if(NULL == strstr(line, "routingp"))
                continue;

            //jump after ' '
            len = strlen(line);
            for(i = 0; i < len; i++){
                if(line[i] == ' ')
                    continue;
                else
                    break;
            }
            if(line[i] == '#')
                continue;

            pos = strstr(line, "./");
            pos += strlen("./");
            i = 0;
            while(1){
                if(' ' == *pos)
                    break;
                if(i >= 64){
                    EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
                    rval = 2;
                    fclose(fp);
                    fp = NULL;
                    goto func_exit;
                }
                value[i++] = *pos++;
            }
            for(i = 0; i < data_msg_cnt; i++){
                if(0 == strcmp(data_msg[i].name, DNAME_RTVER)){
                    memset(data_msg[i].pvalue, 0, 64);
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
        len = strlen((char*)arg);
        if(len < 1 || len > 64){
            EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
            rval = 2;
            goto func_exit;
        }
        memset(value, 0, sizeof(value));
        strcpy(value, (char*)arg);

        rval = chk_str(value);
        if(rval){
            rval = 2;
            goto func_exit;
        }
        rval = chk_diff_from_dmsg(DNAME_RTVER, value);
        if(rval){
            rval = 3;
            goto func_exit;
        }

        rval = mod_infile(CONFIG_FILE, "routingp", "./", " ", value);

        if(rval){
            rval = 4;
            EPT(stderr, "%s:modify file failed, rval = %d\n", __func__, rval);
            goto func_exit;
        }

        rval = update_data_msg(DNAME_RTVER, value);
    }

func_exit:
    return rval;
}

int dc_cfg_ipver(void *arg, int mode)
{
    int rval = 0;
    FILE *fp;
    char value[64];
    char line[512];
    int len, i;
    char *pos;

    if(mode == 0){
        fp = fopen(CONFIG_FILE, "r");
        if(NULL == fp){
            rval = 1;
            EPT(stderr, "%s:can not open config file\n", __func__);
            goto func_exit;
        }
        memset(value, 0, sizeof(value));
        while(!feof(fp)){
            memset(line, 0, sizeof(line));
            if(NULL == fgets(line, sizeof(line), fp))
                continue;
            if(NULL == strstr(line, "if2tcpip"))
                continue;

            //jump after ' '
            len = strlen(line);
            for(i = 0; i < len; i++){
                if(line[i] == ' ')
                    continue;
                else
                    break;
            }
            if(line[i] == '#')
                continue;

            pos = strstr(line, "./");
            pos += strlen("./");
            i = 0;
            while(1){
                if(' ' == *pos)
                    break;
                if(i >= 64){
                    EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
                    rval = 2;
                    fclose(fp);
                    fp = NULL;
                    goto func_exit;
                }
                value[i++] = *pos++;
            }
            for(i = 0; i < data_msg_cnt; i++){
                if(0 == strcmp(data_msg[i].name, DNAME_IPVER)){
                    memset(data_msg[i].pvalue, 0, 64);
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
        len = strlen((char*)arg);
        if(len < 1 || len > 64){
            EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
            rval = 2;
            goto func_exit;
        }
        memset(value, 0, sizeof(value));
        strcpy(value, (char*)arg);

        rval = chk_str(value);
        if(rval){
            rval = 2;
            goto func_exit;
        }
        rval = chk_diff_from_dmsg(DNAME_IPVER, value);
        if(rval){
            rval = 3;
            goto func_exit;
        }

        rval = mod_infile(CONFIG_FILE, "if2tcpip", "./", " ", value);

        if(rval){
            rval = 4;
            EPT(stderr, "%s:modify file failed, rval = %d\n", __func__, rval);
            goto func_exit;
        }

        rval = update_data_msg(DNAME_IPVER, value);
    }

func_exit:
    return rval;
}

int dc_cfg_dcver(void *arg, int mode)
{
    int rval = 0;
    FILE *fp;
    char value[64];
    char line[512];
    int len, i;
    char *pos;

    if(mode == 0){
        fp = fopen(CONFIG_FILE, "r");
        if(NULL == fp){
            rval = 1;
            EPT(stderr, "%s:can not open config file\n", __func__);
            goto func_exit;
        }
        memset(value, 0, sizeof(value));
        while(!feof(fp)){
            memset(line, 0, sizeof(line));
            if(NULL == fgets(line, sizeof(line), fp))
                continue;
            if(NULL == strstr(line, "devcfg"))
                continue;

            //jump after ' '
            len = strlen(line);
            for(i = 0; i < len; i++){
                if(line[i] == ' ')
                    continue;
                else
                    break;
            }
            if(line[i] == '#')
                continue;

            pos = strstr(line, "./");
            pos += strlen("./");
            i = 0;
            while(1){
                if(' ' == *pos)
                    break;
                if(i >= 64){
                    EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
                    rval = 2;
                    fclose(fp);
                    fp = NULL;
                    goto func_exit;
                }
                value[i++] = *pos++;
            }
            for(i = 0; i < data_msg_cnt; i++){
                if(0 == strcmp(data_msg[i].name, DNAME_DCVER)){
                    memset(data_msg[i].pvalue, 0, 64);
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
        len = strlen((char*)arg);
        if(len < 1 || len > 64){
            EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
            rval = 2;
            goto func_exit;
        }
        memset(value, 0, sizeof(value));
        strcpy(value, (char*)arg);

        rval = chk_str(value);
        if(rval){
            rval = 2;
            goto func_exit;
        }
        rval = chk_diff_from_dmsg(DNAME_DCVER, value);
        if(rval){
            rval = 3;
            goto func_exit;
        }

        rval = mod_infile(CONFIG_FILE, "devcfg", "./", " ", value);

        if(rval){
            rval = 4;
            EPT(stderr, "%s:modify file failed, rval = %d\n", __func__, rval);
            goto func_exit;
        }

        rval = update_data_msg(DNAME_DCVER, value);
    }

func_exit:
    return rval;
}

int dc_cfg_fpgaver(void *arg, int mode)
{
    int rval = 0;
    FILE *fp;
    char value[64];
    char line[512];
    int len, i;
    char *pos;

    if(mode == 0){
        fp = fopen(FPGA_FILE, "r");
        if(NULL == fp){
            rval = 1;
            EPT(stderr, "%s:can not open config file\n", __func__);
            goto func_exit;
        }
        memset(value, 0, sizeof(value));
        while(!feof(fp)){
            memset(line, 0, sizeof(line));
            if(NULL == fgets(line, sizeof(line), fp))
                continue;
            if(NULL == strstr(line, "FPGANAME"))
                continue;

            //jump after ' '
            len = strlen(line);
            for(i = 0; i < len; i++){
                if(line[i] == ' ')
                    continue;
                else
                    break;
            }
            if(line[i] == '#')
                continue;

            pos = strstr(line, "FPGANAME=\"");
            pos += strlen("FPGANAME=\"");
            i = 0;
            while(1){
                if('\"' == *pos)
                    break;
                if(i >= 64){
                    EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
                    rval = 2;
                    fclose(fp);
                    fp = NULL;
                    goto func_exit;
                }
                value[i++] = *pos++;
            }
            for(i = 0; i < data_msg_cnt; i++){
                if(0 == strcmp(data_msg[i].name, DNAME_FPGAVER)){
                    memset(data_msg[i].pvalue, 0, 64);
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
        len = strlen((char*)arg);
        if(len < 1 || len > 64){
            EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
            rval = 2;
            goto func_exit;
        }
        memset(value, 0, sizeof(value));
        strcpy(value, (char*)arg);

        rval = chk_str(value);
        if(rval){
            rval = 2;
            goto func_exit;
        }
        rval = chk_diff_from_dmsg(DNAME_FPGAVER, value);
        if(rval){
            rval = 3;
            goto func_exit;
        }

        rval = mod_infile(FPGA_FILE, NULL, "FPGANAME=\"", "\"", value);

        if(rval){
            rval = 4;
            EPT(stderr, "%s:modify file failed, rval = %d\n", __func__, rval);
            goto func_exit;
        }

        rval = update_data_msg(DNAME_FPGAVER, value);
    }

func_exit:
    return rval;
}

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
        memset(value, 0, sizeof(value));
        while(!feof(fp)){
            memset(line, 0, sizeof(line));
            if(NULL == fgets(line, sizeof(line), fp))
                continue;
            if(NULL == strstr(line, "node_id="))
                continue;

            //jump after ' '
            len = strlen(line);
            for(i = 0; i < len; i++){
                if(line[i] == ' ')
                    continue;
                else
                    break;
            }
            if(line[i] == '#')
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
        char* p_value = (char*)arg;

        rval = chk_num(p_value);
        if(rval){
            rval = 2;
            goto func_exit;
        }
        rval = chk_diff_from_dmsg(DNAME_NDID, p_value);
        if(rval){
            rval = 3;
            goto func_exit;
        }

        rval = mod_infile(CONFIG_FILE, NULL, "node_id=", NULL, p_value);

        if(rval){
            rval = 4;
            EPT(stderr, "%s:modify file failed, rval = %d\n", __func__, rval);
            goto func_exit;
        }
        rval = update_data_msg(DNAME_NDID, p_value);
    }

func_exit:
    return rval;
}

int dc_cfg_nodename(void* arg, int mode)
{
    int rval = 0;
    FILE *fp = NULL;
    char line[512];
    char value[68];
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
            pos = strstr(line, "NodeName=");
            if(NULL == pos){
                continue;
            }

            //jump after ' '
            len = strlen(line);
            for(i = 0; i < len; i++){
                if(line[i] == ' ')
                    continue;
                else
                    break;
            }
            if(line[i] == '#')
                continue;

            pos += strlen("NodeName=");
            strcpy(value, pos);
            len = strlen(value);
            if(len >= 68 || len <= 0){
                EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
                rval = 2;
                fclose(fp);
                fp = NULL;
                goto func_exit;
            }
            while(' ' == value[0] && 2 <= len){
                strcpy(value, value+1);
                len--;
            }
            while((' ' == value[len-1] || '\n' == value[len-1]) && 2 <= len){
                value[len-1] = 0;
                len--;
            }
            if(strlen(value) > 64 || strlen(value) <= 0){
                EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
                rval = 2;
                fclose(fp);
                fp = NULL;
                goto func_exit;
            }
            for(i = 0; i < data_msg_cnt; i++){
                if(0 == strcmp(data_msg[i].name, DNAME_NDNAME)){
                    memset(data_msg[i].pvalue, 0, 64);
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
        char *p_value = (char*)arg;
        
        len = strlen(p_value);
        if(len <= 0 || len >= 64){
            EPT(stderr, "%s:wrong parameter:arg = [%s]", __func__, p_value);
            rval = 2;
            goto func_exit;
        }
        rval = chk_str(p_value);
        if(rval){
            rval = 2;
            goto func_exit;
        }
        rval = chk_diff_from_dmsg(DNAME_NDNAME, p_value);
        if(rval){
            rval = 3;
            goto func_exit;
        }

        rval = mod_infile(DEVINFO_FILE, NULL, "NodeName=", NULL, p_value);

        if(rval){
            rval = 4;
            EPT(stderr, "%s:modify file failed, rval = %d\n", __func__, rval);
            goto func_exit;
        }

        rval = update_data_msg(DNAME_NDNAME, p_value);
    }

func_exit:
    return rval;
}

int dc_cfg_freq(void *arg, int mode)
{
    int rval = 0;

    if(0 == mode){
        int i;

        int reg_271 = ad9361_read_bb(AD9361_BASE_ADDR + (0x271<<2));
        int reg_272 = ad9361_read_bb(AD9361_BASE_ADDR + (0x272<<2));
        int reg_273 = ad9361_read_bb(AD9361_BASE_ADDR + (0x273<<2));
        int reg_274 = ad9361_read_bb(AD9361_BASE_ADDR + (0x274<<2));
        int reg_275 = ad9361_read_bb(AD9361_BASE_ADDR + (0x275<<2));

        int tx_int = ((reg_272 & 0x7) << 8) + reg_271;
        int tx_frac = ((reg_275 & 0x7f) << 16) + (reg_274 << 8) + reg_273;

        const double F_ref = 25.0/2;
        double tx_freq = ((double)tx_frac/8388593 + tx_int) * F_ref;

        for(i = 0; i < data_msg_cnt; i++){
            if(0 == strcmp(data_msg[i].name, DNAME_FREQ)){
                memset(data_msg[i].pvalue, 0, 16);
                sprintf(data_msg[i].pvalue, "%d", (int)(tx_freq+0.5));
                goto func_exit;
            }
        }
    }
    else{
        int freq = *(int*)arg;
        EPT(stderr, "%s:can not config Frequency:%dMhz", __func__, freq);
        rval = 10;
        goto func_exit;
    }
func_exit:
    return rval;
}

int dc_cfg_tx1(void *arg, int mode)
{
    int rd_data = 0;
    int rval = 0;
    int i;

    if(mode == 0){
        
        rd_data = ad9361_read(0x73);
        EPT(stderr, "%s:rd_data = %d\n", __func__, rd_data);
        for(i = 0; i < data_msg_cnt; i++){
            if(0 == strcmp(data_msg[i].name, DNAME_TX1)){
                    memset(data_msg[i].pvalue, 0, 4);
                    sprintf(data_msg[i].pvalue, "%d", rd_data/4);
                    goto func_exit;
            }
        }
    }
    else{
        rval = chk_num((char*)arg);
        if(rval){
            rval = 2;
            goto func_exit;
        }
        rval = chk_diff_from_dmsg(DNAME_TX1, (char*)arg);
        if(rval){
            rval = 3;
            goto func_exit;
        }

        rd_data = atoi((char*)arg);
        rd_data = 4 * rd_data;
        ad9361_write(0x73, rd_data);

        for(i = 0; i < data_msg_cnt; i++){
            if(0 == strcmp(data_msg[i].name, DNAME_TX1)){
                    memset(data_msg[i].pvalue, 0, 4);
                    sprintf(data_msg[i].pvalue, "%d", rd_data/4);
                    goto func_exit;
            }
        }
    }

func_exit:
    return rval;
}


int dc_cfg_tx2(void *arg, int mode)
{
    int rd_data = 0;
    int rval = 0;
    int i;

    if(mode == 0){
        
        rd_data = ad9361_read(0x75);
        for(i = 0; i < data_msg_cnt; i++){
            if(0 == strcmp(data_msg[i].name, DNAME_TX2)){
                    memset(data_msg[i].pvalue, 0, 4);
                    sprintf(data_msg[i].pvalue, "%d", rd_data/4);
                    goto func_exit;
            }
        }
    }
    else{
        rval = chk_num((char*)arg);
        if(rval){
            rval = 2;
            goto func_exit;
        }
        rval = chk_diff_from_dmsg(DNAME_TX2, (char*)arg);
        if(rval){
            rval = 3;
            goto func_exit;
        }

        rd_data = atoi((char*)arg);
        rd_data = 4 * rd_data;
        ad9361_write(0x75, rd_data);

        for(i = 0; i < data_msg_cnt; i++){
            if(0 == strcmp(data_msg[i].name, DNAME_TX2)){
                    memset(data_msg[i].pvalue, 0, 4);
                    sprintf(data_msg[i].pvalue, "%d", rd_data/4);
                    goto func_exit;
            }
        }
    }

func_exit:
    return rval;
}

int dc_cfg_ipaddr(void *arg, int mode)
{
    char ipaddr[16];
    int i;
    int rval = 0;

    if(0 == mode){
        for(i = 0; i < data_msg_cnt; i++){
            if(0 == strcmp(data_msg[i].name, DNAME_IPADDR)){
                memset(data_msg[i].pvalue, 0, 16);
                sprintf(data_msg[i].pvalue, "192.168.%d.240", sa);
                goto func_exit;
            }
        }
    }
    else{
        memset(ipaddr, 0, sizeof(ipaddr));
        strcpy(ipaddr, (char*)arg);
        EPT(stderr, "%s:can not config ipaddr:%s", __func__, ipaddr);
        rval = 10;
        goto func_exit;
    }

func_exit:
    return rval;
}

int dc_cfg_ipmask(void *arg, int mode)
{
    char ipmask[16];
    int i;
    int rval = 0;

    if(0 == mode){
        for(i = 0; i < data_msg_cnt; i++){
            if(0 == strcmp(data_msg[i].name, DNAME_IPMASK)){
                memset(data_msg[i].pvalue, 0, 16);
                strcpy(data_msg[i].pvalue, "255.255.255.0");
                goto func_exit;
            }
        }
    }
    else{
        memset(ipmask, 0, sizeof(ipmask));
        strcpy(ipmask, (char*)arg);
        EPT(stderr, "%s:can not config ipmask:%s\n", __func__, ipmask);
        rval = 10;
        goto func_exit;
    }

func_exit:
    return rval;
}

int dc_cfg_ipgate(void *arg, int mode)
{
    char ipgate[16];
    int i;
    int rval = 0;

    if(0 == mode){
        for(i = 0; i < data_msg_cnt; i++){
            if(0 == strcmp(data_msg[i].name, DNAME_IPGATE)){
                memset(data_msg[i].pvalue, 0, 16);
                sprintf(data_msg[i].pvalue, "192.168.%d.1", sa);
                goto func_exit;
            }
        }
    }
    else{
        memset(ipgate, 0, sizeof(ipgate));
        strcpy(ipgate, (char*)arg);
        EPT(stderr, "%s:can not config ipgate:%s\n", __func__, ipgate);
        rval = 10;
        goto func_exit;
    }

func_exit:
    return rval;
}

int dc_cfg_rtc(void* arg, int mode)
{
    int rval = 0;
    FILE *fp = NULL;
    char line[512];
    char value;
    int i;

    if(0 == mode){
        fp = fopen(DEVINFO_FILE, "r");
        if(NULL == fp){
            rval = 1;
            EPT(stderr, "%s:can not open config file\n", __func__);
            goto func_exit;
        }
        while(!feof(fp)){
            memset(line, 0, sizeof(line));
            if(NULL == fgets(line, sizeof(line), fp))
                continue;
            if(NULL == strstr(line, "RTC="))
                continue;

            sscanf(line, "%*[^0-9]%c", &value);
            //EPT(stderr, "%s:value[%s]\n", __func__, value);
            for(i = 0; i < data_msg_cnt; i++){
                if(0 == strcmp(data_msg[i].name, DNAME_RTC)){
                    memset(data_msg[i].pvalue, 0, 1);
                    sprintf(data_msg[i].pvalue, "%c", value);
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
        value = *(char*)arg;
        EPT(stderr, "%s:can not config RTC\n", __func__);
        rval = 10;
        goto func_exit;
    }

func_exit:
    return rval;
}

int dc_cfg_btyvol(void* arg, int mode)
{
    int rval = 0;
    FILE *fp = NULL;
    char value[16];
    int i, len;

    if(0 == mode){
        char line[512];
        fp = fopen(DEVINFO_FILE, "r");
        if(NULL == fp){
            rval = 1;
            EPT(stderr, "%s:can not open config file\n", __func__);
            goto func_exit;
        }
        while(!feof(fp)){
            memset(line, 0, sizeof(line));
            if(NULL == fgets(line, sizeof(line), fp))
                continue;
            if(NULL == strstr(line, "BTYVOL="))
                continue;

            sscanf(line, "%*[^0-9]%[0-9]", value);
            //EPT(stderr, "%s:value[%s]\n", __func__, value);
            len = strlen(value);
            if(0 == len || 16 <= len){
                EPT(stderr, "%s:get bettry vol fail\n", __func__);
                rval = 2;
                fclose(fp);
                fp = NULL;
                goto func_exit;
            }
            if(2 <= len && value[0] == '0'){
                for(i = 0; i < len-1; i++){
                    value[i] = value[i+1];
                }
                value[i] = 0;
                len--;
            }
            if(strlen(value) > 8 || strlen(value) <= 0){
                EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
                rval = 2;
                fclose(fp);
                fp = NULL;
                goto func_exit;
            }
            for(i = 0; i < data_msg_cnt; i++){
                if(0 == strcmp(data_msg[i].name, DNAME_BTYVOL)){
                    memset(data_msg[i].pvalue, 0, 8);
                    sprintf(data_msg[i].pvalue, "%s", value);
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
        memset(value, 0, sizeof(value));
        strcpy(value, (char*)arg);
        EPT(stderr, "%s:can not config BTYVOL\n", __func__);
        rval = 10;
        goto func_exit;
    }

func_exit:
    return rval;
}

int dc_cfg_btytype(void *arg, int mode)
{
    int rval = 0;
    int i;
    FILE *fp;
    char value[16];

    if(mode == 0){
        char line[512];
        char *pos;
        int len;
        fp = fopen(DEVINFO_FILE, "r");
        if(NULL == fp){
            rval = 1;
            EPT(stderr, "%s:can not open config file\n", __func__);
            goto func_exit;
        }
        while(!feof(fp)){
            memset(line, 0, sizeof(line));
            if(NULL == fgets(line, sizeof(line), fp))
                continue;
            pos = strstr(line, "BTYTYPE=");
            if(NULL == pos)
                continue;
            pos += strlen("BTYTYPE=");
            strcpy(value, pos);
            len = strlen(value);
            if(len >= 16 || len <= 0){
                EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
                rval = 2;
                fclose(fp);
                fp = NULL;
                goto func_exit;
            }
            while(' ' == value[0] && 2 <= len){
                strcpy(value, value+1);
                len--;
            }
            while((' ' == value[len-1] || '\n' == value[len-1]) && 2 <= len){
                value[len-1] = 0;
                len--;
            }
            if(strlen(value) > 8 || strlen(value) <= 0){
                EPT(stderr, "%s,%d:wrong name\n", __func__, __LINE__);
                rval = 2;
                fclose(fp);
                fp = NULL;
                goto func_exit;
            }
            for(i = 0; i < data_msg_cnt; i++){
                if(0 == strcmp(data_msg[i].name, DNAME_BTYTYPE)){
                    memset(data_msg[i].pvalue, 0, 8);
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
        memset(value, 0, sizeof(value));
        strcpy(value, (char*)arg);
        EPT(stderr, "%s:can not config BTYTYPE\n", __func__);
        //maybe need more code
        rval = 10;
        goto func_exit;
    }

func_exit:
    return rval;
}

int dc_cfg_sndrate(void *arg, int mode)
{
    char value[32];
    int rval = 0;

    if(mode == 0){
        int i;

        sprintf(value, "%.2lfkbps", snd_rate);
        if(strlen(value) > 15){
            EPT(stderr, "%s,%d:wrong para\n", __func__, __LINE__);
            rval = 2;
            goto func_exit;
        }
        for(i = 0; i < data_msg_cnt; i++){
            if(0 == strcmp(data_msg[i].name, DNAME_SNDRATE)){
                memset(data_msg[i].pvalue, 0, 16);
                strcpy(data_msg[i].pvalue, value);
                goto func_exit;
            }
            else
                continue;
        }
    }
    else{
        memset(value, 0, sizeof(value));
        strcpy(value, (char*)arg);
        EPT(stderr, "%s:can not config send rate\n", __func__);
        rval = 10;
        goto func_exit;
    }

func_exit:
    return rval;
}

int dc_cfg_rcvrate(void *arg, int mode)
{
    char value[32];
    int rval = 0;

    if(mode == 0){
        int i;

        sprintf(value, "%.2lfkbps", rcv_rate);
        if(strlen(value) > 15){
            EPT(stderr, "%s,%d:wrong para\n", __func__, __LINE__);
            rval = 2;
            goto func_exit;
        }
        for(i = 0; i < data_msg_cnt; i++){
            if(0 == strcmp(data_msg[i].name, DNAME_RCVRATE)){
                memset(data_msg[i].pvalue, 0, 16);
                strcpy(data_msg[i].pvalue, value);
                goto func_exit;
            }
            else
                continue;
        }
    }
    else{
        memset(value, 0, sizeof(value));
        strcpy(value, (char*)arg);
        EPT(stderr, "%s:can not config rcv rate\n", __func__);
        rval = 10;
        goto func_exit;
    }

func_exit:
    return rval;
}

/*
 * function:
 *      according to some keywords, modify a string within an appointed file.
 *      automatic ignore the line heads of '#' or '//'.
 * parameters:
 *      file_path:          path of the file, can not be NULL.
 *      p_kw:               point to a string stores msg which indicates the line will be modified.
 *      p_head:             point to a string stores msg before the string you want to modify, such as "name=" or "id:".
 *      p_end:              point to a string stores msg after the string you want to modify, such as " " or "\n". defalut "\n".
 *      p_str:              point to a string stores msg you want to add.
 * return:
 *      0:                  success
 *      other:              failure
 */
int mod_infile(const char *file_path, const char *p_kw, const char *p_head, const char *p_end, const char *p_str)
{
    //declare and initialze variables
    int rval = 0;
    int len = 0;
    int size = 0;
    int i;
    char line[1024];
    char *pos_h, *pos_e;
    char *file_tmp = NULL;
    char *line_tmp = NULL;
    FILE *fp = NULL;
    char *pkw = NULL;
    char *ph = NULL;
    char *pe = NULL;
    char *str = NULL;

    //check and save parameters
    if(file_path == NULL || p_head == NULL || p_str == NULL){
        rval = 1;
        goto func_exit;
    }

    len = strlen(p_head);
    if(len <= 0){
        rval = 2;
        goto func_exit;
    }
    ph = (char*)malloc(len+1);
    strcpy(ph, p_head);

    if(p_kw == NULL) pkw = ph;
    else{
        len = strlen(p_kw);
        if(len <= 0){
            rval = 3;
            goto func_exit;
        }
        pkw = (char*)malloc(len+1);
        strcpy(pkw, p_kw);
    }

    if(p_end == NULL){
        pe = (char*)malloc(2);
        *pe = '\n';
        *(pe+1) = 0;
    }
    else{
        len = strlen(p_end);
        if(len <= 0){
            rval = 4;
            goto func_exit;
        }
        pe = (char*)malloc(len+1);
        strcpy(pe, p_end);
    }

    len = strlen(p_str);
    if(len <= 0){
        rval = 5;
        goto func_exit;
    }
    str = (char*)malloc(len+1);
    strcpy(str, p_str);

    size = file_size(file_path);
    len = size + strlen(p_str) + 1;
    file_tmp = (char*)malloc(len);
    memset(file_tmp, 0, len);

    //open file
    fp = fopen(file_path, "r");
    if(fp == NULL){
        rval = 6;
        goto func_exit;
    }

    //read file and insert string
    while(!feof(fp)){
        memset(line, 0, sizeof(line));
        if(NULL == fgets(line, sizeof(line), fp)){
            continue;
        }
        if(NULL == strstr(line, pkw)){
            strcat(file_tmp, line);
            continue;
        }

        len = strlen(line);
        for(i = 0; i < len; i++){
            if(' ' == line[i]) continue;
            else break;
        }
        if('#' == line[i] || ('/' == line[i] && '/' == line[i+1])){
            strcat(file_tmp, line);
            continue;
        }

        pos_h = strstr(line, ph);
        if(NULL == pos_h){
            rval = 7;
            goto func_exit;
        }
        pos_h += strlen(ph);
        pos_e = strstr(pos_h, pe);
        if(NULL == pos_e){
            rval = 8;
            goto func_exit;
        }
        len = strlen(pos_e) + 1;
        line_tmp = (char*)malloc(len);
        strcpy(line_tmp, pos_e);
        len = strlen(pos_h);
        memset(pos_h, 0, len);
        strcat(pos_h, str);
        pos_h += strlen(str);
        strcat(pos_h, line_tmp);

        free(line_tmp);
        line_tmp = NULL;

        strcat(file_tmp, line);
    }

    //write file
    fclose(fp);
    fp = fopen(file_path, "w");
    fprintf(fp, "%s", file_tmp);
    fclose(fp);
    fp = NULL;

func_exit:
    if(line_tmp != NULL){
        free(line_tmp);
        line_tmp = NULL;
    }
    if(file_tmp != NULL){
        free(file_tmp);
        file_tmp = NULL;
    }
    if(pkw != NULL){
        if(pkw == ph) pkw = NULL;
        else{
            free(pkw);
            pkw = NULL;
        }
    }
    if(ph != NULL){
        free(ph);
        ph = NULL;
    }
    if(pe != NULL){
        free(pe);
        pe = NULL;
    }
    if(str != NULL){
        free(str);
        str = NULL;
    }
    if(fp != NULL){
        fclose(fp);
        fp = NULL;
    }
    return rval;
}

/* function:
 *      check and modify the legality of number
 * parameters:
 *      buf:            the number
 * return:
 *      0:              success
 *      other:          failure
 */
int chk_num(char *buf)
{
    int i;
    int len;

    len = strlen(buf);
    if(len < 1) return 1;

    for(i = 0; i < len; i++){
        if(buf[i] != '.' && buf[i] != '-' && (buf[i] < '0' || buf[i] > '9')){
            EPT(stderr, "%s,%d:number illegal\n", __func__, __LINE__);
            return 1;
        }
    }

    while(len){
        if(buf[0] == '0' && ((len > 1) ? (buf[1] != '.') : 1)){
            strcpy(buf, buf+1);
            len--;
            continue;
        }
        else if(buf[0] == '.'){
            strcpy(buf, buf+1);
            len--;
            continue;
        }
        else break;
    }
    if(len < 1) return 1;

    return 0;
}

/* function:
 *      check and modify the legality of number
 * parameters:
 *      buf:            the string
 * return:
 *      0:              success
 *      other:          failure
 */
int chk_str(char *buf)
{
    int len;

    len = strlen(buf);
    if(len < 1) return 1;

    while(len){
        if(buf[0] == ' '){
            strcpy(buf, buf+1);
            len--;
            continue;
        }
        else if(buf[len-1] == ' '){
            buf[len-1] = 0;
            len--;
            continue;
        }
        else break;
    }
    if(len < 1) return 1;

    return 0;
}

/* function:
 *      check whether buf is same to value of data_msg.
 * parameters:
 *      buf:            value that user want to modify.
 * return:
 *      0:              different
 *      other:          same
 */
int chk_diff_from_dmsg(const char *pname, const char *buf)
{
    int i;

    for(i = 0; i < data_msg_cnt; i++){
        if(0 == strcmp(pname, data_msg[i].name)){
            if(0 == strcmp(buf, data_msg[i].pvalue)) return 1;
            else return 0;
        }
        else continue;
    }
    return 2;
}

int update_data_msg(const char *name, const char *value)
{
    int i;
    int rval = 0;

    for(i = 0; i < data_msg_cnt; i++){
        if(0 == strcmp(name, data_msg[i].name)){
            strcpy(data_msg[i].pvalue, value);
            goto func_exit;
        }
    }

    EPT(stderr, "%s:para name illegal\n", __func__);
    rval = 1;

func_exit:
    return rval;
}

int ad9361_read_bb(int addr)
{
    int rd_data = 0;
    int i = 0;

    rd_data = drvFPGA_Read(addr);
    for(i = 1; i < 100; i++){
        rd_data = drvFPGA_Read(ADDR_9361_SPI_BUSY);
        if(!(rd_data & 0x100)) break;
    }
    EPT(stderr, "%s:addr:%d rd_data:%d\n", __func__, addr, rd_data);
    return (rd_data & 0xFF);
}

int ad9361_write_bb(int addr, int data)
{
    int rd_data = 0;
    int i = 0;

    for(i = 1; i < 100; i++){
        rd_data = drvFPGA_Read(ADDR_9361_SPI_BUSY);
        if(!(rd_data & 0x100)) break;
    }
    drvFPGA_Write(addr, (data & 0xFF));

    return 0;
}

int ad9361_read(int addr)
{
    int rd_data = 0;
    int i = 0;

    rd_data = drvFPGA_Read(AD9361_BASE_ADDR + (addr<<2));
    for(i = 1; i < 100; i++){
        rd_data = drvFPGA_Read(ADDR_9361_SPI_BUSY);
        if(!(rd_data & 0x100)) break;
    }
    EPT(stderr, "%s:addr:%d rd_data:%d\n", __func__, addr, rd_data);
    return (rd_data & 0xFF);
}

int ad9361_write(int addr, int data)
{
    int rd_data = 0;
    int i = 0;

    for(i = 1; i < 100; i++){
        rd_data = drvFPGA_Read(ADDR_9361_SPI_BUSY);
        if(!(rd_data & 0x100)) break;
    }
    drvFPGA_Write(AD9361_BASE_ADDR + (addr<<2), (data & 0xFF));

    return 0;
}

int drvFPGA_Read(int io_addr)
{
    int io_data = 0;

    io_data = _FPGA_IO_(io_addr);
    EPT(stderr, "%s:io_addr:0x%x io_data:%d\n", __func__, io_addr, io_data);

    return io_data;
}

int drvFPGA_Write(int io_addr, int io_data)
{
    _FPGA_IO_(io_addr) = io_data;

    return 0;
}

int drvFPGA_Init(int *p_fd)
{
    int fd = 0;

    fd = open(DEVNAME, O_RDWR | O_SYNC);
    if(fd < 0){
        EPT(stderr, "%s:can not open /dev/mem\n", __func__);
        return 1;
    }


    g_FPGA_pntr = mmap(0, 65536, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd, FPGA_ADDR_BASE);
    if(g_FPGA_pntr == NULL){
        EPT(stderr, "%s,%d:can not open mmap\n", __func__, __LINE__);
        close(fd);
        return 2;
    }
    else if(g_FPGA_pntr == (void*)-1){
        EPT(stderr, "%s,%d:can not open mmap\n", __func__, __LINE__);
        close(fd);
        return 3;
    }
    
    *p_fd = fd;
    return 0;
}

int drvFPGA_Close(int *p_fd)
{
    int fd = *p_fd;

    close(fd);
    munmap(g_FPGA_pntr, 65536);
    *p_fd = 0;
    g_FPGA_pntr = NULL;

    return 0;
}

#include "dc_common.h"

//extern int dt_qid;

int qs = 0;
int re_qin = -1;            //index of receiving queue in qinfs 
int dc_qid = -1;            //queue id of deviceconfig process 
int boa_qid = -1;           //queue id of boa process

char pname[64];             //self process name

//mutex for msg tx func
pthread_mutex_t txm_lock;

qinfo_t qinfs[]=
{
    {PNAME_DEVCFG, SN_DEVCFG, -1, -1},
    {PNAME_BOA, SN_BOA, -1, -1}
};

/*queue count*/
const int cnt_p = sizeof(qinfs)/sizeof(qinfs[0]);

dc_tshare_t dc_share = 
{
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_COND_INITIALIZER,
    1,
    1
};

/*
 * function: 
 *      get all qid
 * parameters:
 *      arg:        self process name
 * return:
 *      0:          success
 *      other:      failure
 */
int dc_get_qids(void* arg)
{
    //declare variables here
    int i,qid;
    int rval = 0;               //error return value
    char *name;

    name = (char*)arg;
    for(i = 0; i < cnt_p; i++)
    {
        qinfs[i].key_q = ftok(PATH_CREAT_KEY, qinfs[i].sub);
        if(NULL != strstr(name, qinfs[i].pname)){
            re_qin = i;
        }
    }
    if(-1 == re_qin){
        rval = 1;
        EPT(stderr, "%s:can not create the key of myself.\n", name);
        goto func_exit;
    }
    qs = 0;
    for(i = 0; i < cnt_p; i++)
    {
        qid = msgget(qinfs[i].key_q, IPC_CREAT|QUEUE_MODE);
        if(qid == -1){
            EPT(stderr, "%s:can not get queue for %s.\n", name, qinfs[i].pname);
            break;
        }
        else{
            qs++;
            qinfs[i].qid = qid;
            
            //set the qid
            if((-1 == dc_qid) && (0 == strcmp(PNAME_DEVCFG, qinfs[i].pname)))
                dc_qid = qid;
            else if((-1 == boa_qid) && (0 == strcmp(PNAME_BOA, qinfs[i].pname)))
                boa_qid = qid;
            else
                EPT(stderr, "error occurs, qinfs[%d].name = %s\n", i, qinfs[i].pname);
        }
    }

    if(i < cnt_p)
        rval = 2;

func_exit:
    if(0 != rval)
    {
        EPT(stderr, "error occurs, rval = %d\n", rval);
    }
    return rval;
}

/*
 * function:
 *      whether the str[find] is in the str[dest]
 * parameters:
 *      find:       the string to be found
 *      dest:       the string to be searched
 * return:
 *      -1:         failure
 *      other:      position of the first character
 */
/*
int dc_strstr(char* find, char* dest)
{
    int rval = -1;
    int i = 0;
    int flen, dlen;
    char fch;               //first character
    char* pos;

    if((find == NULL) || (dest == NULL)){
        EPT(stderr, "%s,%d:error parameters,NULL ptr!\n",__func__, __LINE__);
        goto func_exit;
    }
    
    flen = strlen(find);
    dlen = strlen(dest);
    if(flen <= 0 || dlen <= 0){
        EPT(stderr, "%s,%d:error parameters,flen=%d,dlen=%d\n",__func__, __LINE__, flen, dlen);
        goto func_exit;
    }
    if(flen > dlen){
        EPT(stderr, "%s,%d:find[%s] is larger than dest[%s].\n",__func__, __LINE__, find, dest);
        goto func_exit;
    }

    //first character
    fch = find[0];
    i = 0;
    while(i < dlen)
    {
        pos = strchr(dest+i, fch);


    }

func_exit:
    return rval;
}
*/

/*
 * function:
 *      delete deviceconfig queue
 * return:
 *      0:              success
 *      other:          failure
 */
int dc_queues_delete()
{

    if(qinfs[re_qin].qid != -1)
    {
        msgctl(qinfs[re_qin].qid, IPC_RMID, NULL);
        qinfs[re_qin].qid = -1;
    }
/*
    if(dt_qid != -1)
    {
        msgctl(dt_qid, IPC_RMID, NULL);
        dt_qid = -1;
    }
*/
    return 0;
}

int dc_msg_send(int qid, void* msg, int len)
{
    int rcnt;
    int rval = 0;

    mmsg_t* p = (mmsg_t*)msg;

    if(qid < 0 || len < 0 || len > MAX_MSG_BUF + (int)sizeof(MADR)){
        rval = 1;
        EPT(stderr, "\n%s:ERROR in %s:qid = %d, mtype = %ld, len = %d\n\n", qinfs[re_qin].pname, __func__, qid, p->mtype, len);
        goto func_exit;
    }
    pthread_mutex_lock(&txm_lock);
    rcnt = msgsnd(qid, msg, len, 0);
    if(-1 == rcnt){
        EPT(stderr, "%s:error occurs in sending msg to qid = %d, len = %d, cause[%s]\n", qinfs[re_qin].pname, qid, len, strerror(errno));
        rval = 2;
    }
    EPT(stderr, "\n%s:sending message success, len = %d\n", __func__, len); 
    pthread_mutex_unlock(&txm_lock);

func_exit:
    return rval;
}

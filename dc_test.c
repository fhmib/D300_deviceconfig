#include "dc_common.h"

int main()
{
    int test_boa_qid = -1;
    int test_dc_qid = -1;
    key_t test_boa_key_q;
    key_t test_dc_key_q;
    mmsg_t test_msg_data;
    mmsg_t test_rcv_data;
    dcmsg_t* test_dc_data;
    int test_len = 0;

    memset(&test_msg_data, 0, sizeof(test_msg_data));
    memset(&test_rcv_data, 0, sizeof(test_rcv_data));
    test_boa_key_q = ftok(PATH_CREAT_KEY, SN_BOA);
    test_boa_qid = msgget(test_boa_key_q, IPC_CREAT|QUEUE_MODE);
    test_dc_key_q = ftok(PATH_CREAT_KEY, SN_DEVCFG);
    test_dc_qid = msgget(test_dc_key_q, IPC_CREAT|QUEUE_MODE);

    test_msg_data.mtype = MMSG_DC_BOAREAD;
    test_msg_data.node = 3;
    test_len += sizeof(MADR);
    test_dc_data = (dcmsg_t*)test_msg_data.data;
    test_dc_data->type = 1;
    test_len += sizeof(long);
    memset(test_dc_data->data, 0, sizeof(test_dc_data->data));
    msgsnd(test_dc_qid, &test_msg_data, test_len, 0);
    test_dc_data = (dcmsg_t*)test_rcv_data.data;
    msgrcv(test_boa_qid, &test_rcv_data, MAX_MSG_BUF, 0, 0);
    printf("data:\n%s\n", test_dc_data->data);
    return 0;

}
#include "dc_common.h"

int main()
{
    int test_boa_qid = -1;
    int test_dc_qid = -1;
    key_t test_boa_key_q;
    key_t test_dc_key_q;
    mmsg_t test_msg_data;
    int test_len = 0;

    memset(&test_msg_data, 0, sizeof(test_msg_data));

    test_boa_key_q = ftok(PATH_CREAT_KEY, SN_BOA);
    test_boa_qid = msgget(test_boa_key_q, IPC_CREAT|QUEUE_MODE);
    test_dc_key_q = ftok(PATH_CREAT_KEY, SN_DEVCFG);
    test_dc_qid = msgget(test_dc_key_q, IPC_CREAT|QUEUE_MODE);

    test_len = 0;
    test_msg_data.mtype = MMSG_DC_BOAWRITE;
    test_msg_data.node = 3;
    test_len += sizeof(MADR);
    strcpy(test_msg_data.data, "\"NodeName\": \"RZXT_NODE[3]\" \"TX1Power\": Abcc \"TX2Power\": 122a \"HaveRTC\": 0 \"Rssi_Ant0\": [12, 32, r3, ba, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1]");
    test_len += strlen(test_msg_data.data);
    msgsnd(test_dc_qid, &test_msg_data, test_len, 0);

    return 0;
}
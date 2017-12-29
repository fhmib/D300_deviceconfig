#include "dc_common.h"

int main()
{
    int test_boa_qid = -1;
    int test_dc_qid = -1;
    key_t test_boa_key_q;
    key_t test_dc_key_q;

    test_boa_key_q = ftok(PATH_CREAT_KEY, SN_BOA);
    test_boa_qid = msgget(test_boa_key_q, QUEUE_MODE);
    if(test_boa_qid != -1)
        msgctl(test_boa_qid, IPC_RMID, 0);
    test_dc_key_q = ftok(PATH_CREAT_KEY, SN_DEVCFG);
    test_dc_qid = msgget(test_dc_key_q, IPC_CREAT|QUEUE_MODE);
    if(test_dc_qid != -1)
        msgctl(test_dc_qid, IPC_RMID, 0);

    return 0;
}

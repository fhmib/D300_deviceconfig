#include "dc_common.h"

int main()
{
    int test_boa_qid = -1;
    int test_dc_qid = -1;
    key_t test_boa_key_q;
    key_t test_dc_key_q;
    mmsg_t test_msg_data;
    mmsg_t test_rcv_data;
    int test_len = 0;

    memset(&test_msg_data, 0, sizeof(test_msg_data));
    memset(&test_rcv_data, 0, sizeof(test_rcv_data));

    test_boa_key_q = ftok(PATH_CREAT_KEY, SN_BOA);
    test_boa_qid = msgget(test_boa_key_q, IPC_CREAT|QUEUE_MODE);
    test_dc_key_q = ftok(PATH_CREAT_KEY, SN_DEVCFG);
    test_dc_qid = msgget(test_dc_key_q, IPC_CREAT|QUEUE_MODE);

    test_len = 0;
    test_msg_data.mtype = MMSG_DC_BOAWRITE;
    test_msg_data.seq = 1;
    test_len += sizeof(int);
    //strcpy(test_msg_data.data, "\"NodeName\": \"RZXT_NODE[3]\" \"TX1Power\": Abcc \"TX2Power\": 122a \"HaveRTC\": 0 \"Rssi_Ant0\": [12, 32, r3, ba, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1]");
    //strcpy(test_msg_data.data, " \"TX1Power\": 10 \"TX2Power\": 10 \"NodeId\": 1 \"NodeName\": \"rzxt_[1e3]\" \"HighMacVersion\": \"highmac_0119\" \"NetLayerVersion\": \"netlayer_0119\" \"RoutingVersion\": \"routingp_0119\" \"If2TcpIpVersion\": \"if2tcpip_0119\" \"DeviceConfigVersion\": \"devcfg_0119\" \"FPGAVersion\": \"FPGAVER_0119\"");
    strcpy(test_msg_data.data, "\"NodeId\": 8 \"NodeName\": \"rzxt_mech[1r3]\" \"HighMacVersion\": \"highmac_0122\" \"NetLayerVersion\": \"netlayer_0122\" \"RoutingVersion\": \"routingp_0122\" \"If2TcpIpVersion\": \"if2tcpip_0122\" \"DeviceConfigVersion\": \"devcfg_0122\" \"FPGAVersion\": \"FPGAVER_0122\"");
    test_len += strlen(test_msg_data.data);
    msgsnd(test_dc_qid, &test_msg_data, test_len, 0);
    msgrcv(test_boa_qid, &test_rcv_data, MAX_MSG_BUF, 0, 0);
    if(test_rcv_data.mtype == MMSG_DC_RET){
        EPT(stderr, "%s:receive type=MMSG_DC_RET\n", __func__);
    }
    else if(test_rcv_data.mtype == MMSG_DC_FAIL){
        EPT(stderr, "%s:receive type=MMSG_DC_FAIL\n", __func__);
    }

    return 0;
}

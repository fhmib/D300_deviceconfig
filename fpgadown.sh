#!/bin/bash


#FPGANAME="FpgaTop_Z7030_R7_0818.bit"
#FPGANAME="XIN_MI_RGMII__DYN_ADDPAD_BB.bit"
#FPGANAME="axi_test3_0818.bit"
#FPGANAME="XIN_ADD_PAD.bit"
FPGANAME="FPGAVER_0122"


if [ -e  $FPGANAME ];
then
        echo "down fpga"
            cat $FPGANAME > /dev/xdevcfg
        fi


#ifndef L3MOBILITYSM_H
#define L3MOBILITYSM_H

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <exception>
#include "../../../../lib/Frozen/simSupport/include/simSupport.h"


#define M_T310 4
#define M_TTT  2
#define QOUT  -8
#define QIN -6
#define a3_offset 3
#define TRUE_RLF 5
#define RLF_REASON_CODE_HO_FAILURE 6
#define m1 5
#define m2 4
#define std1 2
#define std2 1

using namespace std;

class HandoverStateMachine {
public:
    int myUEid = -1;
    bool running;
    int ho_commandcycles;
    int ho_completecycles;
    bool n310_event;
    bool n311_event;
    int rlf_count;
    int mT310 = -1;
    int mTTT = -1;
    int num;
    vector<double> cellrsrp;
    int ho_command;
    int ho_complete;
    int s_gnb;
    int target_gnb;
    int prospectiveGNB = -1;
    int failureReasonCode;
    vector<int> gnbIDS;
    
    HandoverStateMachine(int ueID, ivec gNBs, int s_gnb);

    enum State { START, HO_PREPARATION, RLF_MONITORING, HO_FAILURE_RLF, HO_EXECUTION, HO_FAILURE_EXEC, DROP_UE } state;

    void updateSM();

    void handleStart();
    void handleHOPreparation();
    void handleRLF();
    void handleHOFailure_RLF();
    void handleHOExecution();
    void handleHOFailure_EXEC();
    void handleDropUE();

    bool detectA3Event();
    void incrementRLFCount();
    bool detect_n310();
    bool detect_n311();
    int getFailureReasonCode();
    int getIndexOfValue(int value);

};

#endif // L3MOBILITYSM_H


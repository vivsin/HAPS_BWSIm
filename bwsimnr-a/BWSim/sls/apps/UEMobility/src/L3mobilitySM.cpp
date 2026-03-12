#include "../include/L3mobilitySM.h"


HandoverStateMachine::HandoverStateMachine(int ueID, ivec gNBs, int s_gnb)
    : state(START), n310_event(false), n311_event(false), rlf_count(0),
      ho_commandcycles(0), ho_completecycles(0), s_gnb(s_gnb),mT310(M_T310),mTTT(M_TTT),
      target_gnb(s_gnb), failureReasonCode(0)
{
    myUEid = ueID;
    gnbIDS = ITPPVectorstdVector(gNBs);  
    cellrsrp.resize(gNBs.length(),0);
    running = true;
}


// State transition switch case
void HandoverStateMachine::updateSM() {
    cout<<"[MobilitySM][ueID : "<<myUEid<<"][State :"<<state<<"] Entered SM \n";
    cout <<"UE "<<myUEid<<" | s_gnb"<<s_gnb<<endl;
    switch (state) {
        case START:
            handleStart();
            break;
        case HO_PREPARATION:
            handleHOPreparation();
            break;
        case RLF_MONITORING:
            handleRLF();
            break;
        case HO_FAILURE_RLF:
            handleHOFailure_RLF();
            break;
        case HO_EXECUTION:
            handleHOExecution();
            break;
        case HO_FAILURE_EXEC:
            handleHOFailure_EXEC();
            break;
        case DROP_UE:
            handleDropUE();
            break;
        default:
            break;
    }
}

//Possible transitions may happen in START state

void HandoverStateMachine::handleStart() {
    cout << "State: START\n";
    if (!detect_n310()) {
        if (detectA3Event()) {
            cout << "A3 Event detected\n";
            mTTT = M_TTT;
            state = HO_PREPARATION;
        } else {
            state = START;
        }
    } else {
        if (mT310 > 0) {
            mT310--;
            cout << "number of slots left to finish timer t310: " << mT310 <<endl;
            if (detect_n311()) {
                cout << "n311 event occurred\n";
                mT310 = M_T310;
                state = START;
            }
        } else {
            cout << "TRUE RLF\n";
            failureReasonCode = TRUE_RLF;
            state = DROP_UE;
        }
    }
}

// Possible transitions may happen in HOPREPERATION state

void HandoverStateMachine::handleHOPreparation() {
    cout << "State: HO_PREPARATION\n";
    if (mTTT > 0) {
        mTTT--;
        if (!detectA3Event()) {
            cout<<"UE "<<myUEid<<" reset to Start state \n";
            state = START;
        } else {
            mT310 = M_T310;
            if (detect_n310()) {
                mT310--;
                cout << "number of slots left to finish timer t310: " << mT310 << "\n";
                state = RLF_MONITORING;
            }
        }
    } else if (mTTT == 0) {
        mTTT--;
        cout << "Measurement report\n";
        state = HO_PREPARATION;
        random_device rd;
        mt19937 gen(rd());
        normal_distribution<> d1(m1, std1);
        ho_commandcycles = floor(d1(gen));
        cout << "ho_commandcycles: " << ho_commandcycles << endl;
    } else {
        if (ho_commandcycles > 0) {
            ho_commandcycles--;
            cout << "no of cycles left to get ho_command: " << ho_commandcycles << "\n";
            mT310=M_T310;
            if (detect_n310()) {
                mT310--;
                cout << "number of slots left to finish timer t310: " << mT310 << "\n";
                state = RLF_MONITORING;
            } else {
                state = HO_PREPARATION;
            }
        } else {
            ho_command = 1;
            cout << "ho_command received\n";
            random_device rd;
            mt19937 gen(rd());
            normal_distribution<> d2(m2, std2);
            ho_completecycles = floor(d2(gen));
            int sourceGNB = getIndexOfValue(s_gnb);

            for (int i = 0; i < cellrsrp.size(); ++i) {
                if (i != sourceGNB) {
                    if (cellrsrp[i] > cellrsrp[target_gnb] + a3_offset) {
                        target_gnb = i;
                    }
                }
            }
            
            int target_gnbID = gnbIDS[target_gnb];
            if(prospectiveGNB == target_gnbID)
            {
                cout<<"prospective gNB sent HO command\n";
            }
            else
            {
                cout<<"new target gNB sent HO command\n";
            }
            cout << "target_gnb ID: " << target_gnbID << "\n";
            prospectiveGNB = -1;//resetting prospective gNB, as target gNB is decided
            state = HO_EXECUTION;
        }
    }
}

// Possible transitions may happen in RLF state

void HandoverStateMachine::handleRLF() {
    cout << "State: RLF\n";
    if (mTTT > -1) {
        if (mT310 > 0) {
            mT310--;
            if (detect_n311()) {
                cout << "n311 event occurred\n";
                mT310 = M_T310;
                state = HO_PREPARATION;
            }
        } else {
            incrementRLFCount();
            state = DROP_UE;
        }
    } else {
        if (mT310 >= 0) {
            mT310--;
            if (ho_commandcycles > 0) {
                ho_commandcycles--;
                if (detect_n311()) {
                    cout << "n311 event occurred\n";
                    mT310 = M_T310;
                    state = HO_PREPARATION;
                }
            } else if (ho_commandcycles == 0) {
                state = HO_FAILURE_RLF;
            }
        } else {
            incrementRLFCount();
            state = DROP_UE;
        }
    }
}

// Possible transitions may happen in HOfailure1 state

void HandoverStateMachine::handleHOFailure_RLF() {
    cout << "State: HO_FAILURE_RLF\n";
    cout << "number of slots left to expire T310 timer: " << mT310 << "\n";
    if (mT310 > 0) {
        if (detect_n311()) {
            cout << "n311 event occurred\n";
            mT310 = M_T310;
            state = START;
        } else if (detect_n310()) {
            mT310--;
            cout << "number of slots left to expire T310 timer: " << mT310 << "\n";
        } else {
            state = HO_FAILURE_RLF;
        }
    } else { 
        incrementRLFCount();
        cout << "RLF REASON CODE HO FAILURE\n";
        failureReasonCode = RLF_REASON_CODE_HO_FAILURE;
        state = DROP_UE;
    }
}
// Possible transitions may happen in HOexecution state

void HandoverStateMachine::handleHOExecution() {
    cout << "State: HO_EXECUTION\n";
    if (ho_completecycles > 0) {
        ho_completecycles--;
        cout << "ho_completecycles: " << ho_completecycles << "\n";
        if (cellrsrp[target_gnb] < QIN) {
            state = HO_FAILURE_EXEC;
        } else {
            state = HO_EXECUTION;
        }
    } else {
        cout << "SUCCESSFUL RACH\n";
        s_gnb = target_gnb;
        mT310 = M_T310;
        state = START;
        cout << "Target gNB position: " << s_gnb << "\n";
    }
}

// Possible transitions may happen in HOFailure_EXEC state

void HandoverStateMachine::handleHOFailure_EXEC() {
    cout << "State: HO_FAILURE_EXEC\n";
    if (detect_n310()) {
        incrementRLFCount();
        state = DROP_UE;
    }
}

// DROP_UE state
   
void HandoverStateMachine::handleDropUE() {
    cout << "State: DROP_UE\n";
    incrementRLFCount();
    running = false;
}

 // Detects A3 event
 
bool HandoverStateMachine::detectA3Event() {
    int index = getIndexOfValue(s_gnb);
    int servingCellRSRP = cellrsrp[index];

    if(prospectiveGNB != -1)
    {    
        int prospectiveGNBindex = getIndexOfValue(prospectiveGNB);
        if (cellrsrp[prospectiveGNBindex] > servingCellRSRP + a3_offset) {
            //assumption : gnbIDs vector and cellrsrp are vectors of the same size
            cout<<"TTT running | UE "<<myUEid<<" | Current source "<<s_gnb<<" with RSRP "<<servingCellRSRP<<" | A3 event still going on | Prospective gNB "<<gnbIDS[prospectiveGNBindex]<<" | current RSRP : "<<cellrsrp[prospectiveGNBindex]<<"\n";
            
            prospectiveGNB = gnbIDS[prospectiveGNBindex];
            return true;
        }
        else
        {
            cout<<"A3 event not valid anymore | TTT to be reset \n";
            mTTT = -1;
        }
    }
    for (int gnbIndex = 0; gnbIndex < cellrsrp.size(); ++gnbIndex)
    {
        if (gnbIndex != index) {
            if (cellrsrp[gnbIndex] > servingCellRSRP + a3_offset) {
                //assumption : gnbIDs vector and cellrsrp are vectors of the same size
                cout<<"UE "<<myUEid<<" | Current source "<<s_gnb<<" with RSRP "<<servingCellRSRP<<" | A3 event detected | new Prospective gNB "<<gnbIDS[gnbIndex]<<" | better RSRP : "<<cellrsrp[gnbIndex]<<"\n";
                
                prospectiveGNB = gnbIDS[gnbIndex];
                return true;
            }
        }
    }
    return false;
}

 // Incrementing number of RLFs
void HandoverStateMachine::incrementRLFCount() {
    ++rlf_count;
}

// Detect whether n310 event happens or not
bool HandoverStateMachine::detect_n310() {
    int ind = getIndexOfValue(s_gnb);
    n310_event = (cellrsrp[ind] < QOUT);
    return n310_event;
}

// Detect whether n311 event happens or not

bool HandoverStateMachine::detect_n311() {
    int m = getIndexOfValue(s_gnb);
    n311_event = (cellrsrp[m] > QIN);
    return n311_event;
}

//Return the reason for  failure

int HandoverStateMachine::getFailureReasonCode() {
    return failureReasonCode;
}

//Return the index of value in gnbID 
int HandoverStateMachine::getIndexOfValue(int value) {
    auto it = find(gnbIDS.begin(), gnbIDS.end(), value);
    if (it != gnbIDS.end()) {
        return distance(gnbIDS.begin(), it);
    } else {
        throw out_of_range("Value not found in gnbIDS vector");
    }
}





































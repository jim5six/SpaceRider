#include "GameModeManager.h"

struct SingleModeStatus {
    unsigned long startTime = 0;
    unsigned long endTime = 0;
};

struct ModeStatuses {
    SingleModeStatus superSpinner;
    SingleModeStatus superBlastOff;
    SingleModeStatus superPops;
};

ModeStatuses modeStates;

void ResetModes(void) {
    modeStates.superSpinner.startTime = 0;
    modeStates.superSpinner.endTime = 0;
    modeStates.superBlastOff.startTime = 0;
    modeStates.superBlastOff.endTime = 0;
    modeStates.superPops.startTime = 0;
    modeStates.superPops.endTime = 0;
}

void StartSuperSpinner(unsigned long curTime) {
    modeStates.superSpinner.startTime = curTime;
    modeStates.superSpinner.endTime = curTime + SUPER_SPINNER_DURATION;
}

bool IsSuperSpinnerActive(unsigned long curTime) {
    return curTime < modeStates.superSpinner.endTime;
}

unsigned long SuperSpinnerRemainingTime(unsigned long curTime) {
    if (IsSuperSpinnerActive(curTime)) {
        return modeStates.superSpinner.endTime - curTime;
    }
    return 0; // Mode is not active
}

void StartSuperBlastOff(unsigned long curTime) {
    modeStates.superBlastOff.startTime = curTime;
    modeStates.superBlastOff.endTime = curTime + SUPER_BLASTOFF_DURATION;
}

bool IsSuperSuperBlastOffActive(unsigned long curTime) {
    return curTime < modeStates.superBlastOff.endTime;
}

unsigned long SuperBlastOffRemainingTime(unsigned long curTime) {
    if (IsSuperSuperBlastOffActive(curTime)) {
        return modeStates.superBlastOff.endTime - curTime;
    }
    return 0; // Mode is not active
}

void StartSuperPops(unsigned long curTime) {
    modeStates.superPops.startTime = curTime;
    modeStates.superPops.endTime = curTime + SUPER_POP_DURATION;
}

bool IsSuperPopsActive(unsigned long curTime) {
    return curTime < modeStates.superPops.endTime;
}

unsigned long SuperPopsRemainingTime(unsigned long curTime) {
    if (IsSuperPopsActive(curTime)) {
        return modeStates.superPops.endTime - curTime;
    }
    return 0; // Mode is not active
}

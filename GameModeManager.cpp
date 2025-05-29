#include "GameModeManager.h"

struct SingleModeStatus
{
    unsigned long startTime = 0;
    unsigned long endTime = 0;
};

struct ModeStatuses
{
    SingleModeStatus superSpinner;
    SingleModeStatus superBlastOff;
    SingleModeStatus superPops;
};

ModeStatuses modeStates;

#define SUPER_SPINNER_DURATION      25000
#define SUPER_SPINNER_OVER          100
#define SUPER_BLASTOFF_DURATION     30000
#define SUPER_BLASTOFF_OVER         100
#define SUPER_POP_DURATION          25000
#define SUPER_POP_OVER              100

void ResetModes(void)
{
    modeStates.superSpinner.startTime = 0;
    modeStates.superSpinner.endTime = 0;
    modeStates.superBlastOff.startTime = 0;
    modeStates.superBlastOff.endTime = 0;
    modeStates.superPops.startTime = 0;
    modeStates.superPops.endTime = 0;
}

void StartSuperSpinner(unsigned long curTime)
{
    modeStates.superSpinner.startTime = curTime;
    modeStates.superSpinner.endTime = curTime + SUPER_SPINNER_DURATION;
}

bool IsSuperSpinnerActive(unsigned long curTime)
{
    return curTime < modeStates.superSpinner.endTime;
}

void StartSuperBlastOff(unsigned long curTime)
{
    modeStates.superBlastOff.startTime = curTime;
    modeStates.superBlastOff.endTime = curTime + SUPER_BLASTOFF_DURATION;
}

bool IsSuperSuperBlastOffActive(unsigned long curTime)
{
    return curTime < modeStates.superBlastOff.endTime;
}

void StartSuperPops(unsigned long curTime)
{
    modeStates.superPops.startTime = curTime;
    modeStates.superPops.endTime = curTime + SUPER_POP_DURATION;
}

bool IsSuperPopsActive(unsigned long curTime)
{
    return curTime < modeStates.superPops.endTime;
}


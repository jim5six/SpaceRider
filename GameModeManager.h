#ifndef GAME_MODE_MANAGER_H

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

void ResetModes(void);

void StartSuperSpinner(unsigned long curTime);

bool IsSuperSpinnerActive(unsigned long curTime);

void StartSuperBlastOff(unsigned long curTime);

bool IsSuperSuperBlastOffActive(unsigned long curTime);

void StartSuperPops(unsigned long curTime);

bool IsSuperPopsActive(unsigned long curTime);

#endif
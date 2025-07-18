#ifndef GAME_MODE_MANAGER_H
#define GAME_MODE_MANAGER_H

#define SUPER_SPINNER_DURATION      30000
#define SUPER_SPINNER_OVER          100
#define SUPER_BLASTOFF_DURATION     30000
#define SUPER_BLASTOFF_OVER         100
#define SUPER_POP_DURATION          30000
#define SUPER_POP_OVER              100

// Turn off all modes by resetting their start and end times
void ResetModes(void);

// Start Super Spinner mode
void StartSuperSpinner(unsigned long curTime);

// Returns true if the Super Spinner mode is currently active
bool IsSuperSpinnerActive(unsigned long curTime);

// Returns the remaining time for Super Spinner mode
unsigned long SuperSpinnerRemainingTime(unsigned long curTime);

// Start Super Blast Off mode
void StartSuperBlastOff(unsigned long curTime);

// Stop Super Blash Off mode
void StopSuperBlastOff();

// Returns true if the Super Blast Off mode is currently active
bool IsSuperSuperBlastOffActive(unsigned long curTime);

// Returns the remaining time for Super Blast Off mode
unsigned long SuperBlastOffRemainingTime(unsigned long curTime);

// Start Super Pops mode
void StartSuperPops(unsigned long curTime);

// Returns true if the Super Pops mode is currently active
bool IsSuperPopsActive(unsigned long curTime);

// Returns the remaining time for Super Pops mode
unsigned long SuperPopsRemainingTime(unsigned long curTime);

// Ends all modes
void StopAllModes();

#endif
/**************************************************************************
    This pinball Example Machine code is distributed in the hope that it
    will be useful, but WITHOUT ANY WARRANTY; without even the implied 
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See <https://www.gnu.org/licenses/>. 
*/

#include "RPU_Config.h"
#include "RPU.h"
//#include "DropTargets.h"
#include "SpaceRider.h"
#include "SelfTestAndAudit.h"
#include "AudioHandler.h"
#include "LampAnimations.h"
#include <EEPROM.h>

#define GAME_MAJOR_VERSION  2024
#define GAME_MINOR_VERSION  1
#define DEBUG_MESSAGES  1

/*********************************************************************

    Game specific code

*********************************************************************/

// MachineState
//  0 - Attract Mode
//  negative - self-test modes
//  positive - game play
char MachineState = 0;
boolean MachineStateChanged = true;
#define MACHINE_STATE_ATTRACT         0
#define MACHINE_STATE_INIT_GAMEPLAY   1
#define MACHINE_STATE_INIT_NEW_BALL   2
#define MACHINE_STATE_NORMAL_GAMEPLAY 4
#define MACHINE_STATE_COUNTDOWN_BONUS 99
#define MACHINE_STATE_BALL_OVER       100
#define MACHINE_STATE_MATCH_MODE      110
#define MACHINE_STATE_DIAGNOSTICS     120

#define MACHINE_STATE_ADJUST_FREEPLAY                 (MACHINE_STATE_TEST_DONE-1)
#define MACHINE_STATE_ADJUST_BALL_SAVE                (MACHINE_STATE_TEST_DONE-2)
#define MACHINE_STATE_ADJUST_SOUND_SELECTOR           (MACHINE_STATE_TEST_DONE-3)
#define MACHINE_STATE_ADJUST_MUSIC_VOLUME             (MACHINE_STATE_TEST_DONE-4)
#define MACHINE_STATE_ADJUST_SFX_VOLUME               (MACHINE_STATE_TEST_DONE-5)
#define MACHINE_STATE_ADJUST_CALLOUTS_VOLUME          (MACHINE_STATE_TEST_DONE-6)
#define MACHINE_STATE_ADJUST_TOURNAMENT_SCORING       (MACHINE_STATE_TEST_DONE-7)
#define MACHINE_STATE_ADJUST_TILT_WARNING             (MACHINE_STATE_TEST_DONE-8)
#define MACHINE_STATE_ADJUST_AWARD_OVERRIDE           (MACHINE_STATE_TEST_DONE-9)
#define MACHINE_STATE_ADJUST_BALLS_OVERRIDE           (MACHINE_STATE_TEST_DONE-10)
#define MACHINE_STATE_ADJUST_SCROLLING_SCORES         (MACHINE_STATE_TEST_DONE-11)
#define MACHINE_STATE_ADJUST_EXTRA_BALL_AWARD         (MACHINE_STATE_TEST_DONE-12)
#define MACHINE_STATE_ADJUST_SPECIAL_AWARD            (MACHINE_STATE_TEST_DONE-13)
#define MACHINE_STATE_ADJUST_CREDIT_RESET_HOLD_TIME   (MACHINE_STATE_TEST_DONE-14)
#define MACHINE_STATE_ADJUST_DONE                     (MACHINE_STATE_TEST_DONE-15)

// Game Modes
#define GAME_MODE_SKILL_SHOT                        1
#define GAME_MODE_UNSTRUCTURED_PLAY                 2
#define GAME_MODE_SPINNER_FRENZY                    3
#define GAME_MODE_SPINNER_FRENZY_OVER               4
#define GAME_MODE_POP_FRENZY                        5
#define GAME_MODE_POP_FRENZY_OVER                   6
#define GAME_MODE_BLAST_OFF_COLLECT                 7
#define GAME_MODE_BLAST_OFF_OVER                    8

// Indices of EEPROM save locations
#define EEPROM_BALL_SAVE_BYTE           100
#define EEPROM_FREE_PLAY_BYTE           101
#define EEPROM_SOUND_SELECTOR_BYTE      102
#define EEPROM_SKILL_SHOT_BYTE          103
#define EEPROM_TILT_WARNING_BYTE        104
#define EEPROM_AWARD_OVERRIDE_BYTE      105
#define EEPROM_BALLS_OVERRIDE_BYTE      106
#define EEPROM_TOURNAMENT_SCORING_BYTE  107
#define EEPROM_SFX_VOLUME_BYTE          108
#define EEPROM_MUSIC_VOLUME_BYTE        109
#define EEPROM_SCROLLING_SCORES_BYTE    110
#define EEPROM_CALLOUTS_VOLUME_BYTE     111
#define EEPROM_CRB_HOLD_TIME            118
#define EEPROM_EXTRA_BALL_SCORE_UL      140
#define EEPROM_SPECIAL_SCORE_UL         144

// Sound Effects
#define SOUND_EFFECT_NONE               0

#define SOUND_EFFECT_BONUS_COUNT_1k     2
#define SOUND_EFFECT_ROLL_OVER          3
#define SOUND_EFFECT_OUTLANE            4

#define SOUND_EFFECT_SLINGSHOT          6
#define SOUND_EFFECT_POPBUMPER          7
#define SOUND_EFFECT_SAUCER             8 //REPLACE
#define SOUND_EFFECT_DROPTARGET         9
#define SOUND_EFFECT_ADDCREDIT          10

#define SOUND_EFFECT_BALL_OVER          19
#define SOUND_EFFECT_GAME_OVER          20

#define SOUND_EFFECT_BACKGROUND1        25
#define SOUND_EFFECT_SWITCHHIT          26

#define SOUND_EFFECT_TILT_WARNING       28

#define SOUND_EFFECT_MATCH_SPIN         30

#define SOUND_EFFECT_SPINNER100         32
#define SOUND_EFFECT_SPINNER200         33
#define SOUND_EFFECT_SPINNER1000        34
#define SOUND_EFFECT_SPINNERFRENZY      35
#define SOUND_EFFECT_SPINNER2000        36
#define SOUND_EFFECT_SPINNERCENTER      37

#define SOUND_EFFECT_ROCKET_BLAST       44
#define SOUND_EFFECT_BASS_RUMBLE        45
#define SOUND_EFFECT_HURRY_UP           46
#define SOUND_EFFECT_BACKGROUND2        25
#define SOUND_EFFECT_BONUS_COUNT_2k     48
#define SOUND_EFFECT_BONUS_COUNT_3k     49
#define SOUND_EFFECT_RUBBER             50

#define SOUND_EFFECT_TILT               61

#define NUM_BACKGROUND_SONGS            1

#if (RPU_MPU_ARCHITECTURE<10) && !defined(RPU_OS_DISABLE_CPC_FOR_SPACE)
// This array maps the self-test modes to audio callouts
unsigned short SelfTestStateToCalloutMap[34] = {  136, 137, 135, 134, 133, 140, 141, 142, 139, 143, 144, 145, 146, 147, 148, 149, 138, 150, 151, 152,
                                                  153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166 };
#elif (RPU_MPU_ARCHITECTURE<10) && defined(RPU_OS_DISABLE_CPC_FOR_SPACE)
unsigned short SelfTestStateToCalloutMap[31] = {  136, 137, 135, 134, 133, 140, 141, 142, 139, 143, 144, 145, 146, 147, 148, 149, 138, 
                                                  153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166 };
#elif (RPU_MPU_ARCHITECTURE>=10) && !defined(RPU_OS_DISABLE_CPC_FOR_SPACE)
// This array maps the self-test modes to audio callouts
unsigned short SelfTestStateToCalloutMap[34] = {  134, 135, 133, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152,
                                                  153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166 };
#elif (RPU_MPU_ARCHITECTURE>=10) && defined(RPU_OS_DISABLE_CPC_FOR_SPACE)
unsigned short SelfTestStateToCalloutMap[34] = {  134, 135, 133, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 
                                                  153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166 };
#endif

#define SOUND_EFFECT_SELF_TEST_MODE_START             132
#define SOUND_EFFECT_SELF_TEST_CPC_START              180
#define SOUND_EFFECT_SELF_TEST_AUDIO_OPTIONS_START    190




// Game play status callouts
#define NUM_VOICE_NOTIFICATIONS                 35

#define SOUND_EFFECT_BALL_SAVE          300
#define SOUND_EFFECT_EXTRABALL          301
#define SOUND_EFFECT_GATEOPEN           302
#define SOUND_EFFECT_PLAYERADDED        303
#define SOUND_EFFECT_SHOOTAGAIN         304
#define SOUND_EFFECT_SKILLSHOT          305
#define SOUND_EFFECT_SUPERSPINNER_GOAL  306
#define SOUND_EFFECT_BLASTOFF_GOAL      307
#define SOUND_EFFECT_SPINNER_HELD       308
#define SOUND_EFFECT_PLAYFIELD_MULTI    309
#define SOUND_EFFECT_BLASTOFF_HELD      310
#define SOUND_EFFECT_POP_HELD           311
#define SOUND_EFFECT_SUPERPOP_GOAL      312
#define SOUND_EFFECT_SPACE_HELD         313

#define SOUND_EFFECT_GAME_START         319
#define SOUND_EFFECT_RIDER1             320
#define SOUND_EFFECT_RIDER2             321
#define SOUND_EFFECT_RIDER3             322
#define SOUND_EFFECT_RIDER4             323

#define SOUND_EFFECT_DIAG_START                   1900
#define SOUND_EFFECT_DIAG_CREDIT_RESET_BUTTON     1900
#define SOUND_EFFECT_DIAG_SELECTOR_SWITCH_ON      1901
#define SOUND_EFFECT_DIAG_SELECTOR_SWITCH_OFF     1902
#define SOUND_EFFECT_DIAG_STARTING_ORIGINAL_CODE  1903
#define SOUND_EFFECT_DIAG_STARTING_NEW_CODE       1904
#define SOUND_EFFECT_DIAG_ORIGINAL_CPU_DETECTED   1905
#define SOUND_EFFECT_DIAG_ORIGINAL_CPU_RUNNING    1906
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_U10         1907
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_U11         1908
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_1           1909
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_2           1910
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_3           1911
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_4           1912
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_5           1913
#define SOUND_EFFECT_DIAG_STARTING_DIAGNOSTICS    1914


#define MAX_DISPLAY_BONUS               10
#define TILT_WARNING_DEBOUNCE_TIME      1000

/*********************************************************************

    Machine state and options

*********************************************************************/
byte Credits = 0;
byte SoundSelector = 3;
byte BallSaveNumSeconds = 0;
byte MaximumCredits = 40;
byte BallsPerGame = 3;
byte ScoreAwardReplay = 0;
byte MusicVolume = 6;
byte SoundEffectsVolume = 8;
byte CalloutsVolume = 10;
byte ChuteCoinsInProgress[3];
byte TotalBallsLoaded = 1;
byte TimeRequiredToResetGame = 1;
byte NumberOfBallsInPlay = 0;
byte NumberOfBallsLocked = 0;
byte NumberOfSpins[4];
byte NumberOfCenterSpins[4];
byte NumberOfHits[4];
byte Goals[4]={0,0,0,0};
byte TargetBankComplete[4];
byte LampType = 0;
boolean SkillShotHit = false;
boolean FreePlayMode = true;
boolean GoalsDisplayToggle;
boolean HighScoreReplay = true;
boolean MatchFeature = true;
boolean TournamentScoring = false;
boolean ScrollingScores = false;
unsigned long ExtraBallValue = 0;
unsigned long SpecialValue = 0;
unsigned long CurrentTime = 0;
unsigned long HighScore = 0;
unsigned long AwardScores[3];
unsigned long CreditResetPressStarted = 0;
unsigned long LastTimeSpinnerHit = 0;

AudioHandler Audio;



/*********************************************************************

    Game State

*********************************************************************/
byte CurrentPlayer = 0;
byte CurrentBallInPlay = 1;
byte CurrentNumPlayers = 0;
byte Bonus[4];
byte CurrentBonus;
byte BonusX[4];
byte GameMode = GAME_MODE_SKILL_SHOT;
byte MaxTiltWarnings = 2;
byte NumTiltWarnings = 0;
byte HoldoverAwards[4];
byte AwardPhase;

boolean SamePlayerShootsAgain = false;
boolean BallSaveUsed = false;
boolean ExtraBallCollected = false;
boolean SpecialCollected = false;
boolean TimersPaused = true;
boolean AllowResetAfterBallOne = true;
boolean RightSpinner = true;

unsigned long CurrentScores[4];
unsigned long BallFirstSwitchHitTime = 0;
unsigned long BallTimeInTrough = 0;
unsigned long GameModeStartTime = 0;
unsigned long GameModeEndTime = 0;
unsigned long LastTiltWarningTime = 0;
unsigned long ScoreAdditionAnimation;
unsigned long ScoreAdditionAnimationStartTime;
unsigned long LastRemainingAnimatedScoreShown;
unsigned long PlayfieldMultiplier;
unsigned long LastTimeThroughLoop;
unsigned long LastSwitchHitTime;
unsigned long BallSaveEndTime;
unsigned long SuperSpinnerStartTime = 0;
unsigned long SuperSpinnerEndTime = 0;
unsigned long SuperSpinnerOverStartTime = 0;
unsigned long SuperSpinnerOverEndTime = 0;
unsigned long SuperBlastOffStartTime = 0;
unsigned long SuperBlastOffOverStartTime = 0;
unsigned long SuperBlastOffOverEndTime = 0;
unsigned long SuperBlastOffEndTime = 0;
unsigned long SuperPopStartTime = 0;
unsigned long SuperPopEndTime = 0;
unsigned long SuperPopOverStartTime = 0;
unsigned long SuperPopOverEndTime = 0;

#define BALL_SAVE_GRACE_PERIOD  2000


/*********************************************************************

    Game Specific State Variables

*********************************************************************/
unsigned long PlayfieldMultiplierExpiration;
unsigned long BonusChanged;
unsigned long BonusXAnimationStart;
boolean GateOpen = true;
unsigned long GateOpenTime = 0;

//DropTargetBank DropTargets(4, 1, DROP_TARGET_TYPE_BLY_1, 50);


/******************************************************
 * 
 * Adjustments Serialization
 * 
 */


void ReadStoredParameters() {
   for (byte count=0; count<3; count++) {
    ChuteCoinsInProgress[count] = 0;
  }
 
  HighScore = RPU_ReadULFromEEProm(RPU_HIGHSCORE_EEPROM_START_BYTE, 10000);
  Credits = RPU_ReadByteFromEEProm(RPU_CREDITS_EEPROM_BYTE);
  if (Credits > MaximumCredits) Credits = MaximumCredits;

  ReadSetting(EEPROM_FREE_PLAY_BYTE, 0);
  FreePlayMode = (EEPROM.read(EEPROM_FREE_PLAY_BYTE)) ? true : false;

  BallSaveNumSeconds = ReadSetting(EEPROM_BALL_SAVE_BYTE, 15);
  if (BallSaveNumSeconds > 20) BallSaveNumSeconds = 20;

  SoundSelector = ReadSetting(EEPROM_SOUND_SELECTOR_BYTE, 3);
  if (SoundSelector > 8) SoundSelector = 3;

  MusicVolume = ReadSetting(EEPROM_MUSIC_VOLUME_BYTE, 10);
  if (MusicVolume>10) MusicVolume = 10;

  SoundEffectsVolume = ReadSetting(EEPROM_SFX_VOLUME_BYTE, 10);
  if (SoundEffectsVolume>10) SoundEffectsVolume = 10;

  CalloutsVolume = ReadSetting(EEPROM_CALLOUTS_VOLUME_BYTE, 10);
  if (CalloutsVolume>10) CalloutsVolume = 10;

  Audio.SetMusicVolume(MusicVolume);
  Audio.SetSoundFXVolume(SoundEffectsVolume);
  Audio.SetNotificationsVolume(CalloutsVolume);

  TournamentScoring = (ReadSetting(EEPROM_TOURNAMENT_SCORING_BYTE, 0)) ? true : false;

  MaxTiltWarnings = ReadSetting(EEPROM_TILT_WARNING_BYTE, 2);
  if (MaxTiltWarnings > 2) MaxTiltWarnings = 2;

  byte awardOverride = ReadSetting(EEPROM_AWARD_OVERRIDE_BYTE, 99);
  if (awardOverride != 99) {
    ScoreAwardReplay = awardOverride;
  }

  byte ballsOverride = ReadSetting(EEPROM_BALLS_OVERRIDE_BYTE, 99);
  if (ballsOverride == 3 || ballsOverride == 5) {
    BallsPerGame = ballsOverride;
  } else {
    if (ballsOverride != 99) EEPROM.write(EEPROM_BALLS_OVERRIDE_BYTE, 99);
  }

  ScrollingScores = (ReadSetting(EEPROM_SCROLLING_SCORES_BYTE, 1)) ? true : false;

  ExtraBallValue = RPU_ReadULFromEEProm(EEPROM_EXTRA_BALL_SCORE_UL);
  if (ExtraBallValue % 1000 || ExtraBallValue > 100000) ExtraBallValue = 20000;

  SpecialValue = RPU_ReadULFromEEProm(EEPROM_SPECIAL_SCORE_UL);
  if (SpecialValue % 1000 || SpecialValue > 100000) SpecialValue = 40000;

  TimeRequiredToResetGame = ReadSetting(EEPROM_CRB_HOLD_TIME, 1);
  if (TimeRequiredToResetGame>3 && TimeRequiredToResetGame!=99) TimeRequiredToResetGame = 1;

  AwardScores[0] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_1_EEPROM_START_BYTE);
  AwardScores[1] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_2_EEPROM_START_BYTE);
  AwardScores[2] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_3_EEPROM_START_BYTE);

}



void QueueDIAGNotification(unsigned short notificationNum) {
  // This is optional, but the machine can play an audio message at boot
  // time to indicate any errors and whether it's going to boot to original
  // or new code.
  Audio.QueuePrioritizedNotification(notificationNum, 0, 10, CurrentTime);
}

void setup() {

  if (DEBUG_MESSAGES) {
    // If debug is on, set up the Serial port for communication
    Serial.begin(115200);
    Serial.write("Starting\n");
  }

  // Set up the Audio handler in order to play boot messages
  CurrentTime = millis();
  Audio.InitDevices(AUDIO_PLAY_TYPE_WAV_TRIGGER | AUDIO_PLAY_TYPE_ORIGINAL_SOUNDS);
  Audio.StopAllAudio();

  // Tell the OS about game-specific switches
  // (this is for software-controlled pop bumpers and slings)
#if (RPU_MPU_ARCHITECTURE<10)
  // Machines with a -17, -35, 100, and 200 architecture
  // almost always have software based switch-triggered solenoids. 
  // For those, you can define an array of solenoids and the switches
  // that will trigger them:  
  RPU_SetupGameSwitches(NUM_SWITCHES_WITH_TRIGGERS, NUM_PRIORITY_SWITCHES_WITH_TRIGGERS, SolenoidAssociatedSwitches);
#endif

  // Set up the chips and interrupts
  unsigned long initResult = 0;
  if (DEBUG_MESSAGES) Serial.write("Initializing MPU\n");

  // If the hardware has the ability to switch on the Credit/Reset button (requires Rev 4 or greater)
  // then that can be used to choose Original or New code. Otherwise, the hardware switch
  // will choose Original if open, and New if closed
  initResult = RPU_InitializeMPU(   RPU_CMD_BOOT_ORIGINAL_IF_CREDIT_RESET | RPU_CMD_BOOT_ORIGINAL_IF_NOT_SWITCH_CLOSED |
                                    RPU_CMD_INIT_AND_RETURN_EVEN_IF_ORIGINAL_CHOSEN | RPU_CMD_PERFORM_MPU_TEST, SW_CREDIT_RESET);

  if (DEBUG_MESSAGES) {
    char buf[128];
    sprintf(buf, "Return from init = 0x%04lX\n", initResult);
    Serial.write(buf);
    if (initResult&RPU_RET_6800_DETECTED) Serial.write("Detected 6800 clock\n");
    else if (initResult&RPU_RET_6802_OR_8_DETECTED) Serial.write("Detected 6802/8 clock\n");
    Serial.write("Back from init\n");
  }
  
  //if (initResult & RPU_RET_SELECTOR_SWITCH_ON) QueueDIAGNotification(SOUND_EFFECT_DIAG_SELECTOR_SWITCH_ON);
  //else QueueDIAGNotification(SOUND_EFFECT_DIAG_SELECTOR_SWITCH_OFF);
  
  if (initResult & RPU_RET_CREDIT_RESET_BUTTON_HIT) QueueDIAGNotification(SOUND_EFFECT_DIAG_CREDIT_RESET_BUTTON);

  if (initResult & RPU_RET_DIAGNOSTIC_REQUESTED) {
    QueueDIAGNotification(SOUND_EFFECT_DIAG_STARTING_DIAGNOSTICS);
    // Run diagnostics here:    
  }

  if (initResult & RPU_RET_ORIGINAL_CODE_REQUESTED) {
    if (DEBUG_MESSAGES) Serial.write("Asked to run original code\n");
    delay(100);
    QueueDIAGNotification(SOUND_EFFECT_DIAG_STARTING_ORIGINAL_CODE);
    delay(100);
    while (Audio.Update(millis()));
    // Arduino should hang if original code is running
    while (1);
  }
  //QueueDIAGNotification(SOUND_EFFECT_DIAG_STARTING_NEW_CODE);
  
  RPU_DisableSolenoidStack();
  RPU_SetDisableFlippers(true);

  // Read parameters from EEProm
  ReadStoredParameters();
  RPU_SetCoinLockout((Credits >= MaximumCredits) ? true : false);

  CurrentScores[0] = GAME_MAJOR_VERSION;
  CurrentScores[1] = GAME_MINOR_VERSION;
  CurrentScores[2] = RPU_OS_MAJOR_VERSION;
  CurrentScores[3] = RPU_OS_MINOR_VERSION;

  // Initialize any drop target variables here
//  DropTargets.DefineSwitch(3, SW_DROP_1);
//  DropTargets.DefineSwitch(2, SW_DROP_2);
//  DropTargets.DefineSwitch(1, SW_DROP_3);
//  DropTargets.DefineSwitch(0, SW_DROP_4);
//  DropTargets.DefineResetSolenoid(0, SOL_DROP_TARGET_RESET);
  RPU_PushToTimedSolenoidStack(SOL_DROP_TARGET_RESET, 10, CurrentTime, true);

  Audio.SetMusicDuckingGain(12);
  Audio.QueueSound(SOUND_EFFECT_GAME_START, AUDIO_PLAY_TYPE_WAV_TRIGGER, CurrentTime+1200);
}

byte ReadSetting(byte setting, byte defaultValue) {
  byte value = EEPROM.read(setting);
  if (value == 0xFF) {
    EEPROM.write(setting, defaultValue);
    return defaultValue;
  }
  return value;
}

// This function is useful for checking the status of drop target switches
byte CheckSequentialSwitches(byte startingSwitch, byte numSwitches) {
  byte returnSwitches = 0;
  for (byte count = 0; count < numSwitches; count++) {
    returnSwitches |= (RPU_ReadSingleSwitchState(startingSwitch + count) << count);
  }
  return returnSwitches;
}


////////////////////////////////////////////////////////////////////////////
//
//  Lamp Management functions
//
////////////////////////////////////////////////////////////////////////////
void SetGeneralIlluminationOn(boolean setGIOn = true) {
  // some machines have solenoids attached to relays 
  // to control the GI
//  WOS_SetContinuousSolenoid(!generalIlluminationOn, SOL_GI_RELAY);

  // Since this basic shell doesn't have GI,
  // this line prevents compiler warnings.
  (void)setGIOn;
}

void ShowLockLamps() {

}


void ShowBonusLamps() {
  byte bonus = CurrentBonus;
  if (bonus>MAX_DISPLAY_BONUS) bonus = MAX_DISPLAY_BONUS;
    byte cap = 9;
  
    if (bonus>cap) {
      RPU_SetLampState(LAMP_BONUS_100, 1, 0, 100);
//      bonus -= cap;
//      cap -= 1;
    }
    if (cap==0) {
      bonus = 0;
    }

    if (bonus==0) {
      for (byte count=1; count<=cap; count++)
        RPU_SetLampState(LAMP_BONUS_10+(count-1), 0, 0, 0);
      return;
    }

    byte bottom; 
      for (bottom=0; bottom<bonus; bottom++){
        RPU_SetLampState(LAMP_BONUS_10+(bottom-1), 1, 0);
      }
  
    if (bottom<=cap) {
      RPU_SetLampState(LAMP_BONUS_10+(bottom-1), 1, 0);
      for (byte count=(bottom+1); count<=cap; count++) {
        RPU_SetLampState(LAMP_BONUS_10+(count-1), 0);
      }
    } 
  }



void ShowBonusXLamps() {
}

void ShowStandupLamps() {

  RPU_SetLampState(LAMP_TARGET_1, 1, 0, 500);
  RPU_SetLampState(LAMP_TARGET_2, 1, 0, 500);
  RPU_SetLampState(LAMP_TARGET_3, 1, 0, 500);
  RPU_SetLampState(LAMP_TARGET_4, 1, 0, 500);
  RPU_SetLampState(LAMP_TARGET_5, 1, 0, 500);

}

void ShowPopBumpersLamps() {

  RPU_SetLampState(LAMP_LR_POP, 1, 0, 0);
  RPU_SetLampState(LAMP_C_POP, 1, 0, 0);

}

void ShowTopSpaceLamps() {

  RPU_SetLampState(LAMP_TOP_S, 1, 0, 500);
  RPU_SetLampState(LAMP_TOP_P, 1, 0, 500);
  RPU_SetLampState(LAMP_TOP_A, 1, 0, 500);
  RPU_SetLampState(LAMP_TOP_C, 1, 0, 500);
  RPU_SetLampState(LAMP_TOP_E, 1, 0, 500);

}

void ShowLowerSpaceLamps() {

  RPU_SetLampState(LAMP_LOWER_S, 1, 0, 500);
  RPU_SetLampState(LAMP_LOWER_P, 1, 0, 500);
  RPU_SetLampState(LAMP_LOWER_A, 1, 0, 500);
  RPU_SetLampState(LAMP_LOWER_C, 1, 0, 500);
  RPU_SetLampState(LAMP_LOWER_E, 1, 0, 500);

}

void ShowCenterSpinnerLamps() {

  RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 0);
  RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 0);
  RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 0);
  RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, 0);
  RPU_SetLampState(LAMP_C_SPINNER_5, 1, 0, 0);

}

void ShowShootAgainLamps() {

  if ( (BallFirstSwitchHitTime==0 && BallSaveNumSeconds) || (BallSaveEndTime && CurrentTime<BallSaveEndTime) ) {
    unsigned long msRemaining = 5000;
    if (BallSaveEndTime!=0) msRemaining = BallSaveEndTime - CurrentTime;
    RPU_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, (msRemaining < 5000) ? 100 : 500);
    RPU_SetLampState(LAMP_HEAD_SAME_PLAYER_SHOOTS_AGAIN, 1, 0, (msRemaining < 5000) ? 100 : 500);
  } else {
    RPU_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
    RPU_SetLampState(LAMP_HEAD_SAME_PLAYER_SHOOTS_AGAIN, SamePlayerShootsAgain);
  }
}

/*
boolean RequestedGIState;
unsigned long GIOverrideEndTime;

void SetGeneralIlluminationOn(boolean generalIlluminationOn = true) {
  RequestedGIState = generalIlluminationOn;
  if (GIOverrideEndTime) return;
  RPU_SetContinuousSolenoid(!generalIlluminationOn, SOL_GI_RELAY);
}

void OverrideGeneralIllumination(boolean generalIlluminationOn, unsigned long endTime) {
  GIOverrideEndTime = endTime;
  RPU_SetContinuousSolenoid(!generalIlluminationOn, SOL_GI_RELAY);
}
*/

////////////////////////////////////////////////////////////////////////////
//
//  Display Management functions
//
////////////////////////////////////////////////////////////////////////////
unsigned long LastTimeScoreChanged = 0;
unsigned long LastFlashOrDash = 0;
unsigned long ScoreOverrideValue[4] = {0, 0, 0, 0};
byte LastAnimationSeed[4] = {0, 0, 0, 0};
byte AnimationStartSeed[4] = {0, 0, 0, 0};
byte ScoreOverrideStatus = 0;
byte ScoreAnimation[4] = {0, 0, 0, 0};
byte AnimationDisplayOrder[4] = {0, 1, 2, 3};
#define DISPLAY_OVERRIDE_BLANK_SCORE 0xFFFFFFFF
#define DISPLAY_OVERRIDE_ANIMATION_NONE     0
#define DISPLAY_OVERRIDE_ANIMATION_BOUNCE   1
#define DISPLAY_OVERRIDE_ANIMATION_FLUTTER  2
#define DISPLAY_OVERRIDE_ANIMATION_FLYBY    3
#define DISPLAY_OVERRIDE_ANIMATION_CENTER   4
byte LastScrollPhase = 0;

byte MagnitudeOfScore(unsigned long score) {
  if (score == 0) return 0;

  byte retval = 0;
  while (score > 0) {
    score = score / 10;
    retval += 1;
  }
  return retval;
}


void OverrideScoreDisplay(byte displayNum, unsigned long value, byte animationType) {
  if (displayNum > 3) return;

  ScoreOverrideStatus |= (0x01 << displayNum);
  ScoreAnimation[displayNum] = animationType;
  ScoreOverrideValue[displayNum] = value;
  LastAnimationSeed[displayNum] = 255;
}

byte GetDisplayMask(byte numDigits) {
  byte displayMask = 0;
  for (byte digitCount = 0; digitCount < numDigits; digitCount++) {
#ifdef RPU_OS_USE_7_DIGIT_DISPLAYS
    displayMask |= (0x40 >> digitCount);
#else
    displayMask |= (0x20 >> digitCount);
#endif
  }
  return displayMask;
}

void SetAnimationDisplayOrder(byte disp0, byte disp1, byte disp2, byte disp3) {
  AnimationDisplayOrder[0] = disp0;
  AnimationDisplayOrder[1] = disp1;
  AnimationDisplayOrder[2] = disp2;
  AnimationDisplayOrder[3] = disp3;
}

void ShowAnimatedValue(byte displayNum, unsigned long displayScore, byte animationType) {
  byte overrideAnimationSeed;
  byte displayMask = RPU_OS_ALL_DIGITS_MASK;

  byte numDigits = MagnitudeOfScore(displayScore);
  if (numDigits == 0) numDigits = 1;
  if (numDigits < (RPU_OS_NUM_DIGITS - 1) && animationType == DISPLAY_OVERRIDE_ANIMATION_BOUNCE) {
    // This score is going to be animated (back and forth)
    overrideAnimationSeed = (CurrentTime / 250) % (2 * RPU_OS_NUM_DIGITS - 2 * numDigits);
    if (overrideAnimationSeed != LastAnimationSeed[displayNum]) {

      LastAnimationSeed[displayNum] = overrideAnimationSeed;
      byte shiftDigits = (overrideAnimationSeed);
      if (shiftDigits >= ((RPU_OS_NUM_DIGITS + 1) - numDigits)) shiftDigits = (RPU_OS_NUM_DIGITS - numDigits) * 2 - shiftDigits;
      byte digitCount;
      displayMask = GetDisplayMask(numDigits);
      for (digitCount = 0; digitCount < shiftDigits; digitCount++) {
        displayScore *= 10;
        displayMask = displayMask >> 1;
      }
      //RPU_SetDisplayBlank(displayNum, 0x00);
      RPU_SetDisplay(displayNum, displayScore, false);
      RPU_SetDisplayBlank(displayNum, displayMask);
    }
  } else if (animationType == DISPLAY_OVERRIDE_ANIMATION_FLUTTER) {
    overrideAnimationSeed = CurrentTime / 50;
    if (overrideAnimationSeed != LastAnimationSeed[displayNum]) {
      LastAnimationSeed[displayNum] = overrideAnimationSeed;
      displayMask = GetDisplayMask(numDigits);
      if (overrideAnimationSeed % 2) {
        displayMask &= 0x55;
      } else {
        displayMask &= 0xAA;
      }
      RPU_SetDisplay(displayNum, displayScore, false);
      RPU_SetDisplayBlank(displayNum, displayMask);
    }
  } else if (animationType == DISPLAY_OVERRIDE_ANIMATION_FLYBY) {
    overrideAnimationSeed = (CurrentTime / 75) % 256;
    if (overrideAnimationSeed != LastAnimationSeed[displayNum]) {
      if (LastAnimationSeed[displayNum] == 255) {
        AnimationStartSeed[displayNum] = overrideAnimationSeed;
      }
      LastAnimationSeed[displayNum] = overrideAnimationSeed;

      byte realAnimationSeed = overrideAnimationSeed - AnimationStartSeed[displayNum];
      if (overrideAnimationSeed < AnimationStartSeed[displayNum]) realAnimationSeed = (255 - AnimationStartSeed[displayNum]) + overrideAnimationSeed;

      if (realAnimationSeed > 34) {
        RPU_SetDisplayBlank(displayNum, 0x00);
        ScoreOverrideStatus &= ~(0x01 << displayNum);
      } else {
        int shiftDigits = (-6 * ((int)AnimationDisplayOrder[displayNum] + 1)) + realAnimationSeed;
        displayMask = GetDisplayMask(numDigits);
        if (shiftDigits < 0) {
          shiftDigits = 0 - shiftDigits;
          byte digitCount;
          for (digitCount = 0; digitCount < shiftDigits; digitCount++) {
            displayScore /= 10;
            displayMask = displayMask << 1;
          }
        } else if (shiftDigits > 0) {
          byte digitCount;
          for (digitCount = 0; digitCount < shiftDigits; digitCount++) {
            displayScore *= 10;
            displayMask = displayMask >> 1;
          }
        }
        RPU_SetDisplay(displayNum, displayScore, false);
        RPU_SetDisplayBlank(displayNum, displayMask);
      }
    }
  } else if (animationType == DISPLAY_OVERRIDE_ANIMATION_CENTER) {
    overrideAnimationSeed = CurrentTime / 250;
    if (overrideAnimationSeed != LastAnimationSeed[displayNum]) {
      LastAnimationSeed[displayNum] = overrideAnimationSeed;
      byte shiftDigits = (RPU_OS_NUM_DIGITS - numDigits) / 2;

      byte digitCount;
      displayMask = GetDisplayMask(numDigits);
      for (digitCount = 0; digitCount < shiftDigits; digitCount++) {
        displayScore *= 10;
        displayMask = displayMask >> 1;
      }
      //RPU_SetDisplayBlank(displayNum, 0x00);
      RPU_SetDisplay(displayNum, displayScore, false);
      RPU_SetDisplayBlank(displayNum, displayMask);
    }
  } else {
    RPU_SetDisplay(displayNum, displayScore, true, 1);
  }

}

void ShowPlayerScores(byte displayToUpdate, boolean flashCurrent, boolean dashCurrent, unsigned long allScoresShowValue = 0) {

  if (displayToUpdate == 0xFF) ScoreOverrideStatus = 0;
  byte displayMask = RPU_OS_ALL_DIGITS_MASK;
  unsigned long displayScore = 0;
  byte scrollPhaseChanged = false;

  byte scrollPhase = ((CurrentTime - LastTimeScoreChanged) / 125) % 16;
  if (scrollPhase != LastScrollPhase) {
    LastScrollPhase = scrollPhase;
    scrollPhaseChanged = true;
  }

  for (byte scoreCount = 0; scoreCount < 4; scoreCount++) {

    // If this display is currently being overriden, then we should update it
    if (allScoresShowValue == 0 && (ScoreOverrideStatus & (0x01 << scoreCount))) {
      displayScore = ScoreOverrideValue[scoreCount];
      if (displayScore != DISPLAY_OVERRIDE_BLANK_SCORE) {
        ShowAnimatedValue(scoreCount, displayScore, ScoreAnimation[scoreCount]);
      } else {
        RPU_SetDisplayBlank(scoreCount, 0);
      }

    } else {
      boolean showingCurrentGoals = false;
      // No override, update scores designated by displayToUpdate
      if (allScoresShowValue == 0) {
        displayScore = CurrentScores[scoreCount];
        displayScore += (Goals[scoreCount] % 10);
        if (Goals[scoreCount]) showingCurrentGoals = true;
      }
      else displayScore = allScoresShowValue;

      // If we're updating all displays, or the one currently matching the loop, or if we have to scroll
      if (displayToUpdate == 0xFF || displayToUpdate == scoreCount || displayScore > RPU_OS_MAX_DISPLAY_SCORE || showingCurrentGoals) {

        // Don't show this score if it's not a current player score (even if it's scrollable)
        if (displayToUpdate == 0xFF && (scoreCount >= CurrentNumPlayers && CurrentNumPlayers != 0) && allScoresShowValue == 0) {
          RPU_SetDisplayBlank(scoreCount, 0x00);
          continue;
        }

        if (displayScore > RPU_OS_MAX_DISPLAY_SCORE) {
          // Score needs to be scrolled
          if ((CurrentTime - LastTimeScoreChanged) < 2000) {
            // show score for four seconds after change
            RPU_SetDisplay(scoreCount, displayScore % (RPU_OS_MAX_DISPLAY_SCORE + 1), false);
            byte blank = RPU_OS_ALL_DIGITS_MASK;
            if (showingCurrentGoals && (CurrentTime / 200) % 2) {
              blank &= ~(0x01 << (RPU_OS_NUM_DIGITS - 1));
            }
            RPU_SetDisplayBlank(scoreCount, blank);
          } else {
            // Scores are scrolled 10 digits and then we wait for 6
            if (scrollPhase < 11 && scrollPhaseChanged) {
              byte numDigits = MagnitudeOfScore(displayScore);

              // Figure out top part of score
              unsigned long tempScore = displayScore;
              if (scrollPhase < RPU_OS_NUM_DIGITS) {
                displayMask = RPU_OS_ALL_DIGITS_MASK;
                for (byte scrollCount = 0; scrollCount < scrollPhase; scrollCount++) {
                  displayScore = (displayScore % (RPU_OS_MAX_DISPLAY_SCORE + 1)) * 10;
                  displayMask = displayMask >> 1;
                }
              } else {
                displayScore = 0;
                displayMask = 0x00;
              }

              // Add in lower part of score
              if ((numDigits + scrollPhase) > 10) {
                byte numDigitsNeeded = (numDigits + scrollPhase) - 10;
                for (byte scrollCount = 0; scrollCount < (numDigits - numDigitsNeeded); scrollCount++) {
                  tempScore /= 10;
                }
                displayMask |= GetDisplayMask(MagnitudeOfScore(tempScore));
                displayScore += tempScore;
              }
              RPU_SetDisplayBlank(scoreCount, displayMask);
              RPU_SetDisplay(scoreCount, displayScore);
            }
          }
        } else {
          if (flashCurrent && displayToUpdate == scoreCount) {
            unsigned long flashSeed = CurrentTime / 250;
            if (flashSeed != LastFlashOrDash) {
              LastFlashOrDash = flashSeed;
              if (((CurrentTime / 250) % 2) == 0) RPU_SetDisplayBlank(scoreCount, 0x00);
              else RPU_SetDisplay(scoreCount, displayScore, true, 2);
            }
          } else if (dashCurrent && displayToUpdate == scoreCount) {
            unsigned long dashSeed = CurrentTime / 50;
            if (dashSeed != LastFlashOrDash) {
              LastFlashOrDash = dashSeed;
              byte dashPhase = (CurrentTime / 60) % (2 * RPU_OS_NUM_DIGITS * 3);
              byte numDigits = MagnitudeOfScore(displayScore);
              if (dashPhase < (2 * RPU_OS_NUM_DIGITS)) {
                displayMask = GetDisplayMask((numDigits == 0) ? 2 : numDigits);
                if (dashPhase < (RPU_OS_NUM_DIGITS + 1)) {
                  for (byte maskCount = 0; maskCount < dashPhase; maskCount++) {
                    displayMask &= ~(0x01 << maskCount);
                  }
                } else {
                  for (byte maskCount = (2 * RPU_OS_NUM_DIGITS); maskCount > dashPhase; maskCount--) {                    
                    byte firstDigit;
                    firstDigit = (0x20) << (RPU_OS_NUM_DIGITS - 6);                   
                    displayMask &= ~(firstDigit >> (maskCount - dashPhase - 1));
                  }
                }
                RPU_SetDisplay(scoreCount, displayScore);
                RPU_SetDisplayBlank(scoreCount, displayMask);
              } else {
                RPU_SetDisplay(scoreCount, displayScore, true, 2);
              }
            }
          } else {
            byte blank;
            blank = RPU_SetDisplay(scoreCount, displayScore, false, 2);
            if (showingCurrentGoals && (CurrentTime / 200) % 2) {
              blank &= ~(0x01 << (RPU_OS_NUM_DIGITS - 1));
            }
            RPU_SetDisplayBlank(scoreCount, blank);
          }
        }
      } // End if this display should be updated
    } // End on non-overridden
  } // End loop on scores

}

void ShowFlybyValue(byte numToShow, unsigned long timeBase) {
  byte shiftDigits = (CurrentTime - timeBase) / 120;
  byte rightSideBlank = 0;

  unsigned long bigVersionOfNum = (unsigned long)numToShow;
  for (byte count = 0; count < shiftDigits; count++) {
    bigVersionOfNum *= 10;
    rightSideBlank /= 2;
    if (count > 2) rightSideBlank |= 0x20;
  }
  bigVersionOfNum /= 1000;

  byte curMask = RPU_SetDisplay(CurrentPlayer, bigVersionOfNum, false, 0);
  if (bigVersionOfNum == 0) curMask = 0;
  RPU_SetDisplayBlank(CurrentPlayer, ~(~curMask | rightSideBlank));
}

void StartScoreAnimation(unsigned long scoreToAnimate) {
  if (ScoreAdditionAnimation != 0) {
    //CurrentScores[CurrentPlayer] += ScoreAdditionAnimation;
  }
  ScoreAdditionAnimation += scoreToAnimate;
  ScoreAdditionAnimationStartTime = CurrentTime;
  LastRemainingAnimatedScoreShown = 0;
}

////////////////////////////////////////////////////////////////////////////
//
//  Machine State Helper functions
//
////////////////////////////////////////////////////////////////////////////
boolean AddPlayer(boolean resetNumPlayers = false) {

  if (Credits < 1 && !FreePlayMode) return false;
  if (resetNumPlayers) CurrentNumPlayers = 0;
  if (CurrentNumPlayers >= 4) return false;

  CurrentNumPlayers += 1;
  RPU_SetDisplay(CurrentNumPlayers - 1, 0, true, 2);
//  RPU_SetDisplayBlank(CurrentNumPlayers - 1, 0x30);

//  RPU_SetLampState(LAMP_HEAD_1_PLAYER, CurrentNumPlayers==1, 0, 500);
//  RPU_SetLampState(LAMP_HEAD_2_PLAYERS, CurrentNumPlayers==2, 0, 500);
//  RPU_SetLampState(LAMP_HEAD_3_PLAYERS, CurrentNumPlayers==3, 0, 500);
//  RPU_SetLampState(LAMP_HEAD_4_PLAYERS, CurrentNumPlayers==4, 0, 500);

  if (!FreePlayMode) {
    Credits -= 1;
    RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    RPU_SetCoinLockout(false);
  }
  if (CurrentNumPlayers==1) Audio.StopAllAudio();
  QueueNotification(SOUND_EFFECT_PLAYERADDED, 1);

  RPU_WriteULToEEProm(RPU_TOTAL_PLAYS_EEPROM_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}

unsigned short ChuteAuditByte[] = {RPU_CHUTE_1_COINS_START_BYTE, RPU_CHUTE_2_COINS_START_BYTE, RPU_CHUTE_3_COINS_START_BYTE};
void AddCoinToAudit(byte chuteNum) {
  if (chuteNum>2) return;
  unsigned short coinAuditStartByte = ChuteAuditByte[chuteNum];
  RPU_WriteULToEEProm(coinAuditStartByte, RPU_ReadULFromEEProm(coinAuditStartByte) + 1);
}


void AddCredit(boolean playSound = false, byte numToAdd = 1) {
  if (Credits < MaximumCredits) {
    Credits += numToAdd;
    if (Credits > MaximumCredits) Credits = MaximumCredits;
    RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
    if (playSound) {
      //PlaySoundEffect(SOUND_EFFECT_ADD_CREDIT);
      RPU_PushToSolenoidStack(SOL_KNOCKER, 20, true); 
    }
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    RPU_SetCoinLockout(false);
  } else {
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    RPU_SetCoinLockout(false);
  }

}

byte SwitchToChuteNum(byte switchHit) {
  byte chuteNum = 0;
  if (switchHit==SW_COIN_2) chuteNum = 1;
  else if (switchHit==SW_COIN_3) chuteNum = 2;
  return chuteNum;
}

boolean AddCoin(byte chuteNum) {
  boolean creditAdded = false;
  if (chuteNum>2) return false;
  byte cpcSelection = GetCPCSelection(chuteNum);

  // Find the lowest chute num with the same ratio selection
  // and use that ChuteCoinsInProgress counter
  byte chuteNumToUse;
  for (chuteNumToUse=0; chuteNumToUse<=chuteNum; chuteNumToUse++) {
    if (GetCPCSelection(chuteNumToUse)==cpcSelection) break;
  }

  //PlaySoundEffect(SOUND_EFFECT_COIN_DROP_1+(CurrentTime%3));

  byte cpcCoins = GetCPCCoins(cpcSelection);
  byte cpcCredits = GetCPCCredits(cpcSelection);
  byte coinProgressBefore = ChuteCoinsInProgress[chuteNumToUse];
  ChuteCoinsInProgress[chuteNumToUse] += 1;

  if (ChuteCoinsInProgress[chuteNumToUse]==cpcCoins) {
    if (cpcCredits>cpcCoins) AddCredit(cpcCredits - (coinProgressBefore));
    else AddCredit(cpcCredits);
    ChuteCoinsInProgress[chuteNumToUse] = 0;
    creditAdded = true;
  } else {
    if (cpcCredits>cpcCoins) {
      AddCredit(1);
      creditAdded = true;
    } else {
    }
  }

  return creditAdded;
}


void AddSpecialCredit() {
  AddCredit(false, 1);
  RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 20, CurrentTime, true);
  RPU_WriteULToEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE) + 1);
}

void AwardSpecial() {
  if (SpecialCollected) return;
  SpecialCollected = true;
  if (TournamentScoring) {
    CurrentScores[CurrentPlayer] += SpecialValue * PlayfieldMultiplier;
  } else {
    AddSpecialCredit();
  }
}

boolean AwardExtraBall() {
  if (ExtraBallCollected) return false;
  ExtraBallCollected = true;
  if (TournamentScoring) {
    CurrentScores[CurrentPlayer] += ExtraBallValue * PlayfieldMultiplier;
  } else {
    SamePlayerShootsAgain = true;
    RPU_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
    RPU_SetLampState(LAMP_HEAD_SAME_PLAYER_SHOOTS_AGAIN, SamePlayerShootsAgain);
    QueueNotification(SOUND_EFFECT_EXTRABALL, 1);
  }
  return true;
}

void SpinnerToggle() {
  if (RightSpinner == false) {
    RPU_SetLampState(LAMP_CR_WHENLIT, 1);
    RPU_SetLampState(LAMP_CL_WHENLIT, 0);
    RightSpinner = true;
  } else {
    RPU_SetLampState(LAMP_CR_WHENLIT, 0);
    RPU_SetLampState(LAMP_CL_WHENLIT, 1);
    RightSpinner = false;
  }
}

void TargetBank() {
  if (TargetBankComplete[CurrentPlayer] == 1){
      RPU_SetLampState(LAMP_BONUS_200, 0, 0, 0);
      RPU_SetLampState(LAMP_BONUS_300, 0, 0, 0);
      RPU_SetLampState(LAMP_BONUS_400, 0, 0, 0);
    } else if (TargetBankComplete[CurrentPlayer] == 2){
      RPU_SetLampState(LAMP_BONUS_200, 1, 0, 0);
      RPU_SetLampState(LAMP_BONUS_300, 0, 0, 0);
      RPU_SetLampState(LAMP_BONUS_400, 0, 0, 0);
    } else if (TargetBankComplete[CurrentPlayer] == 3){
      RPU_SetLampState(LAMP_BONUS_200, 0, 0, 0);
      RPU_SetLampState(LAMP_BONUS_300, 1, 0, 0);
      RPU_SetLampState(LAMP_BONUS_400, 0, 0, 0);
    } else if (TargetBankComplete[CurrentPlayer] == 4){
      RPU_SetLampState(LAMP_BONUS_200, 0, 0, 0);
      RPU_SetLampState(LAMP_BONUS_300, 0, 0, 0);
      RPU_SetLampState(LAMP_BONUS_400, 1, 0, 0);
    } else if (TargetBankComplete[CurrentPlayer] > 4) {
      TargetBankComplete[CurrentPlayer] == 4;
    }
}

void IncreasePlayfieldMultiplier(unsigned long duration) {
  if (PlayfieldMultiplierExpiration) PlayfieldMultiplierExpiration += duration;
  else PlayfieldMultiplierExpiration = CurrentTime + duration;
  PlayfieldMultiplier += 1;
  if (PlayfieldMultiplier > 3) {
    PlayfieldMultiplier = 5;
  }
  if (PlayfieldMultiplier == 2) {
    RPU_SetLampState(LAMP_BONUS_2X, 1, 0, 0);
  } else if (PlayfieldMultiplier == 3) {
    RPU_SetLampState(LAMP_BONUS_2X, 0, 0, 0);
    RPU_SetLampState(LAMP_BONUS_3X, 1, 0, 0);
  } else if (PlayfieldMultiplier == 5) {
    RPU_SetLampState(LAMP_BONUS_2X, 1, 0, 0);
    RPU_SetLampState(LAMP_BONUS_3X, 1, 0, 0);
    RPU_SetLampState(LAMP_DROP_SPECIAL, 1, 0, 500);
    SetGoals(4);
  }
}

/*

// Some example lock functionality
unsigned long UpperLockSwitchDownTime[3] = {0, 0, 0};
unsigned long UpperLockSwitchUpTime[3] = {0, 0, 0};
boolean UpperLockSwitchState[3] = {false, false, false};

void UpdateLockStatus() {
  boolean lockSwitchDownTransition;
  boolean lockSwitchUpTransition;

  for (byte count=0; count<3; count++) {
    lockSwitchDownTransition = false;
    lockSwitchUpTransition = false;

    if (RPU_ReadSingleSwitchState(SW_LOCK_1+count)) {
      UpperLockSwitchUpTime[count] = 0;
      if (UpperLockSwitchDownTime[count]==0) {
        UpperLockSwitchDownTime[count] = CurrentTime;
      } else if (CurrentTime > (UpperLockSwitchDownTime[count] + 250)) {
        lockSwitchDownTransition = true;
      }
    } else {
      UpperLockSwitchDownTime[count] = 0;
      if (UpperLockSwitchUpTime[count]==0) {
        UpperLockSwitchUpTime[count] = CurrentTime;
      } else if (CurrentTime > (UpperLockSwitchUpTime[count] + 250)) {
        lockSwitchUpTransition = true;
      }
    }

    if (lockSwitchUpTransition && UpperLockSwitchState[count]) {
      // if we used to be down & now we're up
      UpperLockSwitchState[count] = false;
    } else if (lockSwitchDownTransition && !UpperLockSwitchState[count]) {
      // if we used to be up & now we're down
      UpperLockSwitchState[count] = true;
      HandleLockSwitch(count);
    }
  }
}

byte InitializeMachineLocksBasedOnSwitches() {
  byte returnLocks = 0;

  if (RPU_ReadSingleSwitchState(SW_LOCK_1)) returnLocks |= LOCK_1_ENGAGED;
  if (RPU_ReadSingleSwitchState(SW_LOCK_2)) returnLocks |= LOCK_2_ENGAGED;
  if (RPU_ReadSingleSwitchState(SW_LOCK_3)) returnLocks |= LOCK_3_ENGAGED;
  
  return returnLocks;
}

void LockBall(byte lockIndex) {
  PlayerLockStatus[CurrentPlayer] &= ~(LOCK_1_AVAILABLE<<lockIndex);
  PlayerLockStatus[CurrentPlayer] |= (LOCK_1_ENGAGED<<lockIndex);
  MachineLocks |= (LOCK_1_ENGAGED<<lockIndex);
  NumberOfBallsLocked = CountBits(MachineLocks & LOCKS_ENGAGED_MASK);
}
*/


#define ADJ_TYPE_LIST                 1
#define ADJ_TYPE_MIN_MAX              2
#define ADJ_TYPE_MIN_MAX_DEFAULT      3
#define ADJ_TYPE_SCORE                4
#define ADJ_TYPE_SCORE_WITH_DEFAULT   5
#define ADJ_TYPE_SCORE_NO_DEFAULT     6
byte AdjustmentType = 0;
byte NumAdjustmentValues = 0;
byte AdjustmentValues[8];
byte CurrentAdjustmentStorageByte = 0;
byte TempValue = 0;
byte *CurrentAdjustmentByte = NULL;
unsigned long *CurrentAdjustmentUL = NULL;
unsigned long SoundSettingTimeout = 0;
unsigned long AdjustmentScore;


int RunSelfTest(int curState, boolean curStateChanged) {
  int returnState = curState;
  CurrentNumPlayers = 0;

  if (curStateChanged) {
    // Send a stop-all command and reset the sample-rate offset, in case we have
    //  reset while the WAV Trigger was already playing.
    Audio.StopAllAudio();
    RPU_TurnOffAllLamps();
    Audio.StopAllAudio();
    int modeMapping = SelfTestStateToCalloutMap[-1 - curState];
    Audio.PlaySound((unsigned short)modeMapping, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
  } else {
    if (SoundSettingTimeout && CurrentTime>SoundSettingTimeout) {
      SoundSettingTimeout = 0;
      Audio.StopAllAudio();
    }
  }

  // Any state that's greater than MACHINE_STATE_TEST_DONE is handled by the Base Self-test code
  // Any that's less, is machine specific, so we handle it here.
  if (curState >= MACHINE_STATE_TEST_DONE) {
    byte cpcSelection = 0xFF;
    byte chuteNum = 0xFF;
    if (curState==MACHINE_STATE_ADJUST_CPC_CHUTE_1) chuteNum = 0;
    if (curState==MACHINE_STATE_ADJUST_CPC_CHUTE_2) chuteNum = 1;
    if (curState==MACHINE_STATE_ADJUST_CPC_CHUTE_3) chuteNum = 2;
    if (chuteNum!=0xFF) cpcSelection = GetCPCSelection(chuteNum);
    returnState = RunBaseSelfTest(returnState, curStateChanged, CurrentTime, SW_CREDIT_RESET, SW_SLAM);
    if (chuteNum!=0xFF) {
      if (cpcSelection != GetCPCSelection(chuteNum)) {
        byte newCPC = GetCPCSelection(chuteNum);
        Audio.StopAllAudio();
        Audio.PlaySound(SOUND_EFFECT_SELF_TEST_CPC_START+newCPC, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
      }
    }
  } else {
    byte curSwitch = RPU_PullFirstFromSwitchStack();

    if (curSwitch == SW_SELF_TEST_SWITCH && (CurrentTime - GetLastSelfTestChangedTime()) > 250) {
      SetLastSelfTestChangedTime(CurrentTime);
      if (RPU_GetUpDownSwitchState()) returnState -= 1;
      else returnState += 1;
    }

    if (curSwitch == SW_SLAM) {
      returnState = MACHINE_STATE_ATTRACT;
    }

    if (curStateChanged) {
      for (int count = 0; count < 4; count++) {
        RPU_SetDisplay(count, 0);
        RPU_SetDisplayBlank(count, 0x00);
      }
      RPU_SetDisplayCredits(0, false);
#if (RPU_MPU_ARCHITECTURE<10)
      RPU_SetDisplayBallInPlay(MACHINE_STATE_TEST_SOUNDS - curState);
#else
      RPU_SetDisplayBallInPlay(MACHINE_STATE_TEST_BOOT - curState);
#endif      
      CurrentAdjustmentByte = NULL;
      CurrentAdjustmentUL = NULL;
      CurrentAdjustmentStorageByte = 0;

      AdjustmentType = ADJ_TYPE_MIN_MAX;
      AdjustmentValues[0] = 0;
      AdjustmentValues[1] = 1;
      TempValue = 0;

      switch (curState) {
        case MACHINE_STATE_ADJUST_FREEPLAY:
          CurrentAdjustmentByte = (byte *)&FreePlayMode;
          CurrentAdjustmentStorageByte = EEPROM_FREE_PLAY_BYTE;
          break;
        case MACHINE_STATE_ADJUST_BALL_SAVE:
          AdjustmentType = ADJ_TYPE_LIST;
          NumAdjustmentValues = 5;
          AdjustmentValues[1] = 5;
          AdjustmentValues[2] = 10;
          AdjustmentValues[3] = 15;
          AdjustmentValues[4] = 20;
          CurrentAdjustmentByte = &BallSaveNumSeconds;
          CurrentAdjustmentStorageByte = EEPROM_BALL_SAVE_BYTE;
          break;
        case MACHINE_STATE_ADJUST_SOUND_SELECTOR:
          AdjustmentType = ADJ_TYPE_MIN_MAX;
          AdjustmentValues[1] = 9;
          CurrentAdjustmentByte = &SoundSelector;
          CurrentAdjustmentStorageByte = EEPROM_SOUND_SELECTOR_BYTE;
          break;
        case MACHINE_STATE_ADJUST_MUSIC_VOLUME:
          AdjustmentType = ADJ_TYPE_MIN_MAX;
          AdjustmentValues[0] = 0;
          AdjustmentValues[1] = 10;
          CurrentAdjustmentByte = &MusicVolume;
          CurrentAdjustmentStorageByte = EEPROM_MUSIC_VOLUME_BYTE;
          break;
        case MACHINE_STATE_ADJUST_SFX_VOLUME:
          AdjustmentType = ADJ_TYPE_MIN_MAX;
          AdjustmentValues[0] = 0;
          AdjustmentValues[1] = 10;
          CurrentAdjustmentByte = &SoundEffectsVolume;
          CurrentAdjustmentStorageByte = EEPROM_SFX_VOLUME_BYTE;
          break;
        case MACHINE_STATE_ADJUST_CALLOUTS_VOLUME:
          AdjustmentType = ADJ_TYPE_MIN_MAX;
          AdjustmentValues[0] = 0;
          AdjustmentValues[1] = 10;
          CurrentAdjustmentByte = &CalloutsVolume;
          CurrentAdjustmentStorageByte = EEPROM_CALLOUTS_VOLUME_BYTE;
          break;
        case MACHINE_STATE_ADJUST_TOURNAMENT_SCORING:
          CurrentAdjustmentByte = (byte *)&TournamentScoring;
          CurrentAdjustmentStorageByte = EEPROM_TOURNAMENT_SCORING_BYTE;
          break;
        case MACHINE_STATE_ADJUST_TILT_WARNING:
          AdjustmentValues[1] = 2;
          CurrentAdjustmentByte = &MaxTiltWarnings;
          CurrentAdjustmentStorageByte = EEPROM_TILT_WARNING_BYTE;
          break;
        case MACHINE_STATE_ADJUST_AWARD_OVERRIDE:
          AdjustmentType = ADJ_TYPE_MIN_MAX_DEFAULT;
          AdjustmentValues[1] = 7;
          CurrentAdjustmentByte = &ScoreAwardReplay;
          CurrentAdjustmentStorageByte = EEPROM_AWARD_OVERRIDE_BYTE;
          break;
        case MACHINE_STATE_ADJUST_BALLS_OVERRIDE:
          AdjustmentType = ADJ_TYPE_LIST;
          NumAdjustmentValues = 3;
          AdjustmentValues[0] = 3;
          AdjustmentValues[1] = 5;
          AdjustmentValues[2] = 99;
          CurrentAdjustmentByte = &BallsPerGame;
          CurrentAdjustmentStorageByte = EEPROM_BALLS_OVERRIDE_BYTE;
          break;
        case MACHINE_STATE_ADJUST_SCROLLING_SCORES:
          CurrentAdjustmentByte = (byte *)&ScrollingScores;
          CurrentAdjustmentStorageByte = EEPROM_SCROLLING_SCORES_BYTE;
          break;
        case MACHINE_STATE_ADJUST_EXTRA_BALL_AWARD:
          AdjustmentType = ADJ_TYPE_SCORE_WITH_DEFAULT;
          CurrentAdjustmentUL = &ExtraBallValue;
          CurrentAdjustmentStorageByte = EEPROM_EXTRA_BALL_SCORE_UL;
          break;
        case MACHINE_STATE_ADJUST_SPECIAL_AWARD:
          AdjustmentType = ADJ_TYPE_SCORE_WITH_DEFAULT;
          CurrentAdjustmentUL = &SpecialValue;
          CurrentAdjustmentStorageByte = EEPROM_SPECIAL_SCORE_UL;
          break;
        case MACHINE_STATE_ADJUST_CREDIT_RESET_HOLD_TIME:
          AdjustmentType = ADJ_TYPE_LIST;
          NumAdjustmentValues = 5;
          AdjustmentValues[0] = 0;
          AdjustmentValues[1] = 1;
          AdjustmentValues[2] = 2;
          AdjustmentValues[3] = 3;
          AdjustmentValues[4] = 99;
          CurrentAdjustmentByte = &TimeRequiredToResetGame;
          CurrentAdjustmentStorageByte = EEPROM_CRB_HOLD_TIME; 
          break;
        case MACHINE_STATE_ADJUST_DONE:
          returnState = MACHINE_STATE_ATTRACT;
          break;
      }
    }

    // Change value, if the switch is hit
    if (curSwitch == SW_CREDIT_RESET) {

      if (CurrentAdjustmentByte && (AdjustmentType == ADJ_TYPE_MIN_MAX || AdjustmentType == ADJ_TYPE_MIN_MAX_DEFAULT)) {
        byte curVal = *CurrentAdjustmentByte;

        if (RPU_GetUpDownSwitchState()) {
          curVal += 1;
          if (curVal > AdjustmentValues[1]) {
            if (AdjustmentType == ADJ_TYPE_MIN_MAX) curVal = AdjustmentValues[0];
            else {
              if (curVal > 99) curVal = AdjustmentValues[0];
              else curVal = 99;
            }
          }
        } else {
          if (curVal==AdjustmentValues[0]) {            
            if (AdjustmentType==ADJ_TYPE_MIN_MAX_DEFAULT) curVal = 99;
            else curVal = AdjustmentValues[1];
          } else {
            curVal -= 1;
          }
        }

        *CurrentAdjustmentByte = curVal;
        if (CurrentAdjustmentStorageByte) EEPROM.write(CurrentAdjustmentStorageByte, curVal);

        if (curState==MACHINE_STATE_ADJUST_SOUND_SELECTOR) {
          Audio.StopAllAudio();
          Audio.PlaySound(SOUND_EFFECT_SELF_TEST_AUDIO_OPTIONS_START+curVal, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
        } else if (curState==MACHINE_STATE_ADJUST_MUSIC_VOLUME) {
          if (SoundSettingTimeout) Audio.StopAllAudio();
          Audio.PlaySound(SOUND_EFFECT_BACKGROUND1, AUDIO_PLAY_TYPE_WAV_TRIGGER, curVal);
          Audio.SetMusicVolume(curVal);
          SoundSettingTimeout = CurrentTime+5000;
        } else if (curState==MACHINE_STATE_ADJUST_SFX_VOLUME) {
          if (SoundSettingTimeout) Audio.StopAllAudio();
          Audio.PlaySound(SOUND_EFFECT_BONUS_COUNT_1k, AUDIO_PLAY_TYPE_WAV_TRIGGER, curVal);
          Audio.SetSoundFXVolume(curVal);
          SoundSettingTimeout = CurrentTime+5000;
        } else if (curState==MACHINE_STATE_ADJUST_CALLOUTS_VOLUME) {
          if (SoundSettingTimeout) Audio.StopAllAudio();
          Audio.PlaySound(SOUND_EFFECT_EXTRABALL, AUDIO_PLAY_TYPE_WAV_TRIGGER, curVal);
          Audio.SetNotificationsVolume(curVal);
          SoundSettingTimeout = CurrentTime+3000;
        }
        
      } else if (CurrentAdjustmentByte && AdjustmentType == ADJ_TYPE_LIST) {
        byte valCount = 0;
        byte curVal = *CurrentAdjustmentByte;
        byte newIndex = 0;
        boolean upDownState = RPU_GetUpDownSwitchState();
        for (valCount = 0; valCount < (NumAdjustmentValues); valCount++) {
          if (curVal == AdjustmentValues[valCount]) {
            if (upDownState) {
              if (valCount<(NumAdjustmentValues-1)) newIndex = valCount + 1;
            } else {
              if (valCount>0) newIndex = valCount - 1;
            }
          }
        }
        *CurrentAdjustmentByte = AdjustmentValues[newIndex];
        if (CurrentAdjustmentStorageByte) EEPROM.write(CurrentAdjustmentStorageByte, AdjustmentValues[newIndex]);
      } else if (CurrentAdjustmentUL && (AdjustmentType == ADJ_TYPE_SCORE_WITH_DEFAULT || AdjustmentType == ADJ_TYPE_SCORE_NO_DEFAULT)) {
        unsigned long curVal = *CurrentAdjustmentUL;

        if (RPU_GetUpDownSwitchState()) curVal += 5000;
        else if (curVal>=5000) curVal -= 5000;
        if (curVal > 100000) curVal = 0;
        if (AdjustmentType == ADJ_TYPE_SCORE_NO_DEFAULT && curVal == 0) curVal = 5000;
        *CurrentAdjustmentUL = curVal;
        if (CurrentAdjustmentStorageByte) RPU_WriteULToEEProm(CurrentAdjustmentStorageByte, curVal);
      }

    }

    // Show current value
    if (CurrentAdjustmentByte != NULL) {
      RPU_SetDisplay(0, (unsigned long)(*CurrentAdjustmentByte), true);
    } else if (CurrentAdjustmentUL != NULL) {
      RPU_SetDisplay(0, (*CurrentAdjustmentUL), true);
    }

  }

  if (returnState == MACHINE_STATE_ATTRACT) {
    // If any variables have been set to non-override (99), return
    // them to dip switch settings
    // Balls Per Game, Player Loses On Ties, Novelty Scoring, Award Score
    //    DecodeDIPSwitchParameters();
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    ReadStoredParameters();
  }

  return returnState;
}




////////////////////////////////////////////////////////////////////////////
//
//  Audio Output functions
//
////////////////////////////////////////////////////////////////////////////
void PlayBackgroundSong(unsigned int songNum) {

  if (MusicVolume==0) return;

  Audio.PlayBackgroundSong(songNum);
}


unsigned long NextSoundEffectTime = 0;

void PlaySoundEffect(unsigned int soundEffectNum) {

  if (MachineState==MACHINE_STATE_INIT_GAMEPLAY) return;

  // Play digital samples on the WAV trigger (numbered same
  // as SOUND_EFFECT_ defines)
  Audio.PlaySound(soundEffectNum, AUDIO_PLAY_TYPE_WAV_TRIGGER);

  // SOUND_EFFECT_ defines can also be translated into
  // commands for the sound card
  switch (soundEffectNum) {
/*    
    case SOUND_EFFECT_LEFT_SHOOTER_LANE:
      Audio.PlaySoundCardWhenPossible(12, CurrentTime, 0, 500, 7);
      break;
    case SOUND_EFFECT_RETURN_TO_SHOOTER_LANE:
      Audio.PlaySoundCardWhenPossible(22, CurrentTime, 0, 500, 8);
      break;
    case SOUND_EFFECT_SAUCER:
      Audio.PlaySoundCardWhenPossible(14, CurrentTime, 0, 500, 7);
      break;
    case SOUND_EFFECT_DROP_TARGET_HURRY:
      Audio.PlaySoundCardWhenPossible(2, CurrentTime, 0, 45, 3);
      break;
    case SOUND_EFFECT_DROP_TARGET_COMPLETE:
      Audio.PlaySoundCardWhenPossible(9, CurrentTime, 0, 1400, 4);
      Audio.PlaySoundCardWhenPossible(19, CurrentTime, 1500, 10, 4);
      break;
    case SOUND_EFFECT_HOOFBEATS:
      Audio.PlaySoundCardWhenPossible(12, CurrentTime, 0, 100, 10);
      break;
    case SOUND_EFFECT_STOP_BACKGROUND:
      Audio.PlaySoundCardWhenPossible(19, CurrentTime, 0, 10, 10);
      break;
    case SOUND_EFFECT_DROP_TARGET_HIT:
      Audio.PlaySoundCardWhenPossible(7, CurrentTime, 0, 150, 5);
      break;
    case SOUND_EFFECT_SPINNER:
      Audio.PlaySoundCardWhenPossible(6, CurrentTime, 0, 25, 2);
      break;
*/      
  }
}


void QueueNotification(unsigned int soundEffectNum, byte priority) {
  if (CalloutsVolume==0) return;
  if (SoundSelector<3 || SoundSelector==4 || SoundSelector==7 || SoundSelector==9) return; 
  if (soundEffectNum < SOUND_EFFECT_BALL_SAVE || soundEffectNum >= (SOUND_EFFECT_BALL_SAVE + NUM_VOICE_NOTIFICATIONS)) return;

  // With RPU_OS_HARDWARE_REV 4 and above, the WAV trigger has two-way communication,
  // so it's not necesary to tell it the length of a notification. For support for 
  // earlier hardware, you'll need an array of VoicePromptLengths for each prompt
  // played (for queueing and ducking)
//  Audio.QueuePrioritizedNotification(soundEffectNum, VoicePromptLengths[soundEffectNum-SOUND_EFFECT_VP_VOICE_NOTIFICATIONS_START], priority, CurrentTime);
  Audio.QueuePrioritizedNotification(soundEffectNum, 0, priority, CurrentTime);

}


//void AlertPlayerUp(byte playerNum) {
//  (void)playerNum;
  // if (AlertPlayerUp(1)){
  //   PlaySoundEffect(SOUND_EFFECT_RIDER1);
  // }
  // if (AlertPlayerUp(2)){
  //   PlaySoundEffect(SOUND_EFFECT_RIDER2);
  // }
  // if (AlertPlayerUp(3)){
  //   PlaySoundEffect(SOUND_EFFECT_RIDER3);
  // }
  // if (AlertPlayerUp(4)){
  //   PlaySoundEffect(SOUND_EFFECT_RIDER4);
  // }
//  QueueNotification(SOUND_EFFECT_VP_PLAYER, 1);
//  QueueNotification(SOUND_EFFECT_VP_ONE + playerNum, 1);
//  QueueNotification(SOUND_EFFECT_RIDER1, 1); 
//}





////////////////////////////////////////////////////////////////////////////
//
//  Diagnostics Mode
//
////////////////////////////////////////////////////////////////////////////

int RunDiagnosticsMode(int curState, boolean curStateChanged) {

  int returnState = curState;

  if (curStateChanged) {

/*
    char buf[256];
    boolean errorSeen;

    Serial.write("Testing Volatile RAM at IC13 (0x0000 - 0x0080): writing & reading... ");
    Serial.write("3 ");
    delay(500);
    Serial.write("2 ");
    delay(500);
    Serial.write("1 \n");
    delay(500);
    errorSeen = false;
    for (byte valueCount=0; valueCount<0xFF; valueCount++) {
      for (unsigned short address=0x0000; address<0x0080; address++) {
        RPU_DataWrite(address, valueCount);
      }
      for (unsigned short address=0x0000; address<0x0080; address++) {
        byte readValue = RPU_DataRead(address);
        if (readValue!=valueCount) {
          sprintf(buf, "Write/Read failure at address=0x%04X (expected 0x%02X, read 0x%02X)\n", address, valueCount, readValue);
          Serial.write(buf);
          errorSeen = true;
        }
        if (errorSeen) break;
      }
      if (errorSeen) break;
    }
    if (errorSeen) {
      Serial.write("!!! Error in Volatile RAM\n");
    }

    Serial.write("Testing Volatile RAM at IC16 (0x0080 - 0x0100): writing & reading... ");
    Serial.write("3 ");
    delay(500);
    Serial.write("2 ");
    delay(500);
    Serial.write("1 \n");
    delay(500);
    errorSeen = false;
    for (byte valueCount=0; valueCount<0xFF; valueCount++) {
      for (unsigned short address=0x0080; address<0x0100; address++) {
        RPU_DataWrite(address, valueCount);
      }
      for (unsigned short address=0x0080; address<0x0100; address++) {
        byte readValue = RPU_DataRead(address);
        if (readValue!=valueCount) {
          sprintf(buf, "Write/Read failure at address=0x%04X (expected 0x%02X, read 0x%02X)\n", address, valueCount, readValue);
          Serial.write(buf);
          errorSeen = true;
        }
        if (errorSeen) break;
      }
      if (errorSeen) break;
    }
    if (errorSeen) {
      Serial.write("!!! Error in Volatile RAM\n");
    }
    
    // Check the CMOS RAM to see if it's operating correctly
    errorSeen = false;
    Serial.write("Testing CMOS RAM: writing & reading... ");
    Serial.write("3 ");
    delay(500);
    Serial.write("2 ");
    delay(500);
    Serial.write("1 \n");
    delay(500);
    for (byte valueCount=0; valueCount<0x10; valueCount++) {
      for (unsigned short address=0x0100; address<0x0200; address++) {
        RPU_DataWrite(address, valueCount);
      }
      for (unsigned short address=0x0100; address<0x0200; address++) {
        byte readValue = RPU_DataRead(address);
        if ((readValue&0x0F)!=valueCount) {
          sprintf(buf, "Write/Read failure at address=0x%04X (expected 0x%02X, read 0x%02X)\n", address, valueCount, (readValue&0x0F));
          Serial.write(buf);
          errorSeen = true;
        }
        if (errorSeen) break;
      }
      if (errorSeen) break;
    }
    
    if (errorSeen) {
      Serial.write("!!! Error in CMOS RAM\n");
    }
    
    
    // Check the ROMs
    Serial.write("CMOS RAM dump... ");
    Serial.write("3 ");
    delay(500);
    Serial.write("2 ");
    delay(500);
    Serial.write("1 \n");
    delay(500);
    for (unsigned short address=0x0100; address<0x0200; address++) {
      if ((address&0x000F)==0x0000) {
        sprintf(buf, "0x%04X:  ", address);
        Serial.write(buf);
      }
//      RPU_DataWrite(address, address&0xFF);
      sprintf(buf, "0x%02X ", RPU_DataRead(address));
      Serial.write(buf);
      if ((address&0x000F)==0x000F) {
        Serial.write("\n");
      }
    }

*/

//    RPU_EnableSolenoidStack();
//    RPU_SetDisableFlippers(false);
        
  }

  return returnState;
}



////////////////////////////////////////////////////////////////////////////
//
//  Attract Mode
//
////////////////////////////////////////////////////////////////////////////

unsigned long AttractLastLadderTime = 0;
byte AttractLastLadderBonus = 0;
unsigned long AttractDisplayRampStart = 0;
byte AttractLastHeadMode = 255;
byte AttractLastPlayfieldMode = 255;
byte InAttractMode = false;
unsigned long ShowLampTimeOffset = 0;

byte LastSolPhase = 0;

int RunAttractMode(int curState, boolean curStateChanged) {

  int returnState = curState;

  if (curStateChanged) {
    // Some sound cards have a special index
    // for a "sound" that will turn off 
    // the current background drone or currently
    // playing sound
//    PlaySoundEffect(SOUND_EFFECT_STOP_BACKGROUND);
    RPU_DisableSolenoidStack();
    RPU_TurnOffAllLamps();
    RPU_SetDisableFlippers(true);
    if (DEBUG_MESSAGES) {
      Serial.write("Entering Attract Mode\n\r");
    }
    AttractLastHeadMode = 0;
    AttractLastPlayfieldMode = 0;
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    for (byte count = 0; count < 4; count++) {
//      RPU_SetLampState(LAMP_HEAD_PLAYER_1_UP + count, 0);
    }

//    RPU_SetLampState(LAMP_HEAD_1_PLAYER, 0);
//    RPU_SetLampState(LAMP_HEAD_2_PLAYERS, 0);
//    RPU_SetLampState(LAMP_HEAD_3_PLAYERS, 0);
//    RPU_SetLampState(LAMP_HEAD_4_PLAYERS, 0);

    // If this machine has a saucer, clear it in attract mode
    
    if (RPU_ReadSingleSwitchState(SW_C_SAUCER)) {
      RPU_PushToSolenoidStack(SOL_C_SAUCER, 16, true);
    }
    if (RPU_ReadSingleSwitchState(SW_R_SAUCER)) {
      RPU_PushToSolenoidStack(SOL_R_SAUCER, 16, true);
    }

  }

  // Some machines have a kicker to move the ball
  // from the outhole to the re-shooter ramp
//  MoveBallFromOutholeToRamp();
  
  // Alternate displays between high score and blank
  if (CurrentTime < 16000) {
    if (AttractLastHeadMode != 1) {
      ShowPlayerScores(0xFF, false, false);
      RPU_SetDisplayCredits(Credits, !FreePlayMode);
      RPU_SetDisplayBallInPlay(0, true);
    }
  } else if ((CurrentTime / 8000) % 2 == 0) {

    if (AttractLastHeadMode != 2) {
      RPU_SetLampState(LAMP_HEAD_HIGH_SCORE, 1, 0, 250);
      RPU_SetLampState(LAMP_HEAD_GAME_OVER, 0);
      LastTimeScoreChanged = CurrentTime;
    }
    AttractLastHeadMode = 2;
    ShowPlayerScores(0xFF, false, false, HighScore);
  } else {
    if (AttractLastHeadMode != 3) {
      if (CurrentTime < 32000) {
        for (int count = 0; count < 4; count++) {
          CurrentScores[count] = 0;
        }
        CurrentNumPlayers = 0;
      }
      RPU_SetLampState(LAMP_HEAD_HIGH_SCORE, 0);
      RPU_SetLampState(LAMP_HEAD_GAME_OVER, 1);
      LastTimeScoreChanged = CurrentTime;
    }
    ShowPlayerScores(0xFF, false, false);

    AttractLastHeadMode = 3;
  }

  byte attractPlayfieldPhase = ((CurrentTime / 5000) % 6);

  if (attractPlayfieldPhase != AttractLastPlayfieldMode) {
    RPU_TurnOffAllLamps();
    AttractLastPlayfieldMode = attractPlayfieldPhase;
    if (attractPlayfieldPhase == 2) GameMode = GAME_MODE_SKILL_SHOT;
    else GameMode = GAME_MODE_UNSTRUCTURED_PLAY;
    AttractLastLadderBonus = 1;
    AttractLastLadderTime = CurrentTime;
  }
  
  ShowLampAnimation(1, 120, CurrentTime, 14, false, false);
//  ShowLampAnimation(0, 40, CurrentTime, 14, false, false);

  byte switchHit;
  while ( (switchHit = RPU_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY ) {
    if (switchHit == SW_CREDIT_RESET) {
      if (AddPlayer(true)) returnState = MACHINE_STATE_INIT_GAMEPLAY;
    }
    if (switchHit == SW_COIN_1 || switchHit == SW_COIN_2 || switchHit == SW_COIN_3) {
      AddCoinToAudit(SwitchToChuteNum(switchHit));
      AddCoin(SwitchToChuteNum(switchHit));
    }
    if (switchHit == SW_SELF_TEST_SWITCH && (CurrentTime - GetLastSelfTestChangedTime()) > 250) {
#if (RPU_MPU_ARCHITECTURE<10)
      returnState = MACHINE_STATE_TEST_LAMPS;
#else      
      returnState = MACHINE_STATE_TEST_BOOT;
#endif      
      SetLastSelfTestChangedTime(CurrentTime);
    }
  }

  return returnState;
}



////////////////////////////////////////////////////////////////////////////
//
//  Game Play functions
//
////////////////////////////////////////////////////////////////////////////


byte CountBits(unsigned short intToBeCounted) {
  byte numBits = 0;

  for (byte count = 0; count < 16; count++) {
    numBits += (intToBeCounted & 0x01);
    intToBeCounted = intToBeCounted >> 1;
  }

  return numBits;
}


void SetGameMode(byte newGameMode) {
  GameMode = newGameMode;
  GameModeStartTime = 0;
  GameModeEndTime = 0;
}

byte CountBallsInTrough() {
  
  byte numBalls = RPU_ReadSingleSwitchState(SW_OUTHOLE);

  return numBalls;
}
      
void AddToBonus(byte amountToAdd=1) {
  CurrentBonus += amountToAdd;
  if (CurrentBonus>MAX_DISPLAY_BONUS) {
    CurrentBonus = MAX_DISPLAY_BONUS;
  } else {
    BonusChanged = CurrentTime;
  }
}

void IncreaseBonusX() {
  boolean soundPlayed = false;
  if (BonusX[CurrentPlayer] < 10) {
    BonusX[CurrentPlayer] += 1;
    BonusXAnimationStart = CurrentTime;

    if (BonusX[CurrentPlayer] == 9) {
      BonusX[CurrentPlayer] = 10;
//      QueueNotification(SOUND_EFFECT_VP_BONUSX_MAX, 2);
    } else {
//      QueueNotification(SOUND_EFFECT_VP_BONUS_X_INCREASED, 1);
    }
  }

  if (!soundPlayed) {
    //    PlaySoundEffect(SOUND_EFFECT_BONUS_X_INCREASED);
  }

}

unsigned long GameStartNotificationTime = 0;
boolean WaitForBallToReachOuthole = false;
unsigned long UpperBallEjectTime = 0;
unsigned long SaucerEjectTime = 0;

int InitGamePlay(boolean curStateChanged) {
  RPU_TurnOffAllLamps();
  SetGeneralIlluminationOn(true);

  if (curStateChanged) {
    GameStartNotificationTime = CurrentTime;
  }

/*
  if (RPU_ReadSingleSwitchState(SW_LOCK_1)) {
    if (CurrentTime > (UpperBallEjectTime+1000)) {
      RPU_PushToSolenoidStack(SOL_UPPER_BALL_EJECT, 12, true);
      UpperBallEjectTime = CurrentTime;
    }
    return MACHINE_STATE_INIT_GAMEPLAY;
  }
*/
  if (RPU_ReadSingleSwitchState(SW_C_SAUCER)) {
    if (CurrentTime > (SaucerEjectTime+1500)) {
      RPU_PushToSolenoidStack(SOL_C_SAUCER, 12, true);
      SaucerEjectTime = CurrentTime;
    }
  }
  if (RPU_ReadSingleSwitchState(SW_R_SAUCER)) {
    if (CurrentTime > (SaucerEjectTime+2500)) {
      RPU_PushToSolenoidStack(SOL_R_SAUCER, 12, true);
      SaucerEjectTime = CurrentTime;
    }
  }

  if (CountBallsInTrough()==0) {

    // Some machines have a kicker to move the ball
    // from the outhole to the re-shooter ramp
//    MoveBallFromOutholeToRamp();

    if (CurrentTime>(GameStartNotificationTime+5000)) {
      GameStartNotificationTime = CurrentTime;
//      QueueNotification(SOUND_EFFECT_VP_BALL_MISSING, 10);
    }
    
    return MACHINE_STATE_INIT_GAMEPLAY;
  }
  
//  MachineLocks = InitializeMachineLocksBasedOnSwitches();
//  NumberOfBallsLocked = CountBits(MachineLocks & LOCKS_ENGAGED_MASK);

  if (CountBallsInTrough()<(TotalBallsLoaded - NumberOfBallsLocked)) {
    return MACHINE_STATE_INIT_GAMEPLAY;
  }

  // The start button has been hit only once to get
  // us into this mode, so we assume a 1-player game
  // at the moment
  RPU_EnableSolenoidStack();
  RPU_SetCoinLockout((Credits >= MaximumCredits) ? true : false);

  // Reset displays & game state variables
  for (int count = 0; count < 4; count++) {
    // Initialize game-specific variables
    BonusX[count] = 1;
    Goals[count] = 0;
    NumberOfSpins[count] = 0;
    NumberOfHits[count] = 0;
    Bonus[count] = 0;
    TargetBankComplete[count] = 1;
  }
  
  memset(CurrentScores, 0, 4 * sizeof(unsigned long));

  SamePlayerShootsAgain = false;
  CurrentBallInPlay = 1;
  CurrentNumPlayers = 1;
//  MachineLocks = 0;
  CurrentPlayer = 0;
  NumberOfBallsInPlay = 0;
//  NumberOfBallsLocked = CountBits(MachineLocks & LOCKS_ENGAGED_MASK);
  NumberOfBallsLocked = 0;
  ShowPlayerScores(0xFF, false, false);

  return MACHINE_STATE_INIT_NEW_BALL;
}


int InitNewBall(bool curStateChanged, byte playerNum, int ballNum) {

  // If we're coming into this mode for the first time
  // then we have to do everything to set up the new ball
  if (curStateChanged) {
    RPU_TurnOffAllLamps();
    BallFirstSwitchHitTime = 0;
    RPU_SetDisableGate(false);
    RPU_SetDisableFlippers(false);
    RPU_EnableSolenoidStack();
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    if (CurrentNumPlayers > 1 && (ballNum != 1 || playerNum != 0) && !SamePlayerShootsAgain)
      PlaySoundEffect(SOUND_EFFECT_RIDER1 + playerNum);
    SamePlayerShootsAgain = false;

    RPU_SetDisplayBallInPlay(CurrentBallInPlay);
    RPU_SetLampState(LAMP_HEAD_TILT, 0);

    if (BallSaveNumSeconds > 0) {
      RPU_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, 500);
      RPU_SetLampState(LAMP_HEAD_SAME_PLAYER_SHOOTS_AGAIN, 1, 0, 500);
    }

    BallSaveUsed = false;
    BallTimeInTrough = 0;
    NumTiltWarnings = 0;
    LastTiltWarningTime = 0;

    // Initialize game-specific start-of-ball lights & variables
    GameModeStartTime = 0;
    GameModeEndTime = 0;
    GameMode = GAME_MODE_SKILL_SHOT;
    SkillShotHit = false;
    ExtraBallCollected = false;
    SpecialCollected = false;
    PlayfieldMultiplier = 1;
    PlayfieldMultiplierExpiration = 0;
    ScoreAdditionAnimation = 0;
    ScoreAdditionAnimationStartTime = 0;
    BonusXAnimationStart = 0;
    Bonus[CurrentPlayer] = 0;
    BallSaveEndTime = 0;
    CurrentBonus = Bonus[CurrentPlayer];
    for (int count = 0; count < 4; count++) {
      NumberOfSpins[count] = 1;
      NumberOfCenterSpins[count] = 1;
      NumberOfHits[count] = 0;
      //Goals[count] = 0;
      TargetBankComplete[count] = 1;
    }
    SuperSpinnerEndTime = 0;
    SuperPopEndTime = 0;
    SuperBlastOffEndTime = 0;
    TargetBank();

  // Reset gate
    GateOpen = true;        // Unpowered gate is open, gate open when true
    GateOpenTime = 0;

    if (CurrentPlayer==0) {
      // Only change skill shot on first ball of round.
//      SkillShotTarget = CurrentTime % 3;      
    }

    // Reset Drop Targets
    RPU_PushToTimedSolenoidStack(SOL_DROP_TARGET_RESET, 10, CurrentTime, true);

    RPU_PushToTimedSolenoidStack(SOL_OUTHOLE, 16, CurrentTime + 1000);
    NumberOfBallsInPlay = 1;
//    QueueNotification(SOUND_EFFECT_GAME_START, 1);
    PlayBackgroundSong(SOUND_EFFECT_BACKGROUND1);
  }

  // We should only consider the ball initialized when
  // the ball is no longer triggering the SW_OUTHOLE
  if (CountBallsInTrough()==(TotalBallsLoaded-NumberOfBallsLocked)) {
    return MACHINE_STATE_INIT_NEW_BALL;
  } else {
    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }
  
     
  LastTimeThroughLoop = CurrentTime;
}

/*
boolean AddABall(boolean ballLocked = false, boolean ballSave = true) {
  if (NumberOfBallsInPlay>=TotalBallsLoaded) return false;

  if (ballLocked) {
    NumberOfBallsLocked += 1;
  } else {
    NumberOfBallsInPlay += 1;
  }

  if (CountBallsInTrough()) {
    RPU_PushToTimedSolenoidStack(SOL_OUTHOLE, 16, CurrentTime + 100);    
  } else {
    if (ballLocked) {
      if (NumberOfBallsInPlay) NumberOfBallsInPlay -= 1;
    } else {
      return false;
    }
  }

  if (ballSave) {
    if (BallSaveEndTime) BallSaveEndTime += 10000;
    else BallSaveEndTime = CurrentTime + 20000;
  }

  return true;
}
*/

//void UpdateDropTargets() {
//  DropTargets.Update(CurrentTime);
//}

byte GameModeStage;
boolean DisplaysNeedRefreshing = false;
unsigned long LastTimePromptPlayed = 0;
unsigned short CurrentBattleLetterPosition = 0xFF;

#define NUM_GI_FLASH_SEQUENCE_ENTRIES 10
byte GIFlashIndex = 0;
unsigned long GIFlashTimer = 0;
unsigned int GIFlashChangeState[NUM_GI_FLASH_SEQUENCE_ENTRIES] = {1000, 1250, 2000, 2250, 3000, 3300, 3500, 3900, 5000, 6500};

// This function manages all timers, flags, and lights
int ManageGameMode() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

  boolean specialAnimationRunning = false;

  if ((CurrentTime - LastSwitchHitTime) > 3000) TimersPaused = true;
  else TimersPaused = false;

  switch ( GameMode ) {
    case GAME_MODE_SKILL_SHOT:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = 0;
        LastTimePromptPlayed = CurrentTime;
        GameModeStage = 0;
        SetGeneralIlluminationOn(false);
      }
      ShowLampAnimation(3, 200, CurrentTime, 14, false, false, 4);

    if (RPU_ReadSingleSwitchState(SW_C_SAUCER)){
      if (RPU_ReadLampState(LAMP_TOP_S)) {
        RPU_SetLampState(LAMP_LOWER_S, 1, 0, 0);
        RPU_SetLampState(LAMP_LOWER_P, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_A, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_C, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_E, 0, 0, 0);
      }if (RPU_ReadLampState(LAMP_TOP_P)) {
        RPU_SetLampState(LAMP_LOWER_S, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_P, 1, 0, 0);
        RPU_SetLampState(LAMP_LOWER_A, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_C, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_E, 0, 0, 0);
      }if (RPU_ReadLampState(LAMP_TOP_A)) {
        RPU_SetLampState(LAMP_LOWER_S, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_P, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_A, 1, 0, 0);
        RPU_SetLampState(LAMP_LOWER_C, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_E, 0, 0, 0);
      }if (RPU_ReadLampState(LAMP_TOP_C)) {
        RPU_SetLampState(LAMP_LOWER_S, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_P, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_A, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_C, 1, 0, 0);
        RPU_SetLampState(LAMP_LOWER_E, 0, 0, 0);
      }if (RPU_ReadLampState(LAMP_TOP_E)) {
        RPU_SetLampState(LAMP_LOWER_S, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_P, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_A, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_C, 0, 0, 0);
        RPU_SetLampState(LAMP_LOWER_E, 1, 0, 0);
      }
    }

      // The switch handler will award the skill shot
      // (when applicable) and this mode will move
      // to unstructured play when any valid switch is 
      // recorded

      if (BallFirstSwitchHitTime != 0) {
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }      
      break;

    case GAME_MODE_UNSTRUCTURED_PLAY:
      // If this is the first time in this mode
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        DisplaysNeedRefreshing = true;
        SetGeneralIlluminationOn(true);        
      }
      // Playfield X value is only reset during unstructured play
      if (PlayfieldMultiplierExpiration) {
        if (CurrentTime > PlayfieldMultiplierExpiration) {
          PlayfieldMultiplierExpiration = 0;
          PlayfieldMultiplier = 1;
          RPU_SetLampState(LAMP_BONUS_2X, 0, 0, 0);
          RPU_SetLampState(LAMP_BONUS_3X, 0, 0, 0);
        } else {
          DisplaysNeedRefreshing = true;
        }
      } else if (DisplaysNeedRefreshing) {
        DisplaysNeedRefreshing = false;
        ShowPlayerScores(0xFF, false, false);
      }

    if (CurrentTime>SuperSpinnerEndTime) {
      SuperSpinnerEndTime = 0;
    }

    if (CurrentTime>SuperPopEndTime) {
      SuperPopEndTime = 0;
    }

    if (CurrentTime>SuperBlastOffEndTime) {
      SuperBlastOffEndTime = 0;
    }  
      ShowLampAnimation(3, 200, CurrentTime, 14, false, false, 4);

      if (RPU_ReadSingleSwitchState(SW_C_SAUCER)){
        if (RPU_ReadLampState(LAMP_TOP_S)) {
          RPU_SetLampState(LAMP_LOWER_S, 1, 0, 0);
      } if (RPU_ReadLampState(LAMP_TOP_P)) {
          RPU_SetLampState(LAMP_LOWER_P, 1, 0, 0);
      } if (RPU_ReadLampState(LAMP_TOP_A)) {
          RPU_SetLampState(LAMP_LOWER_A, 1, 0, 0);
      } if (RPU_ReadLampState(LAMP_TOP_C)) {
          RPU_SetLampState(LAMP_LOWER_C, 1, 0, 0);
      } if (RPU_ReadLampState(LAMP_TOP_E)) {
          RPU_SetLampState(LAMP_LOWER_E, 1, 0, 0);
      }
    } 
      
      RPU_SetLampState(LAMP_LR_POP, 1, 0, 0);
      RPU_SetLampState(LAMP_C_POP, 1, 0, 0);
      
      if ( (GateOpenTime != 0) && ((CurrentTime-GateOpenTime) > 1000) ) {  
        RPU_SetDisableGate(false);
        GateOpenTime = 0;
      }

      if (CurrentTime<PlayfieldMultiplierExpiration) {
        for (byte count=0; count<4; count++) {
          if (count!=CurrentPlayer) OverrideScoreDisplay(count, (PlayfieldMultiplierExpiration-CurrentTime)/1000, DISPLAY_OVERRIDE_ANIMATION_FLUTTER);
          }
        } else
          ShowPlayerScores(0xFF, false, false);
  
      break;

    case GAME_MODE_SPINNER_FRENZY:
      //Goals[CurrentPlayer] |= GOAL_SUPER_SPINNER_ACHIEVED;
      SetGoals(1);
      ShowLampAnimation(4, 120, CurrentTime, 4, false, false);
      
      if (SuperSpinnerStartTime == 0) {
        SuperSpinnerStartTime = CurrentTime;
        SuperSpinnerEndTime = CurrentTime + SUPER_SPINNER_DURATION;
      }

      if (CurrentTime<SuperSpinnerEndTime) {
        for (byte count=0; count<4; count++) {
          if (count!=CurrentPlayer) OverrideScoreDisplay(count, (SuperSpinnerEndTime-CurrentTime)/1000, DISPLAY_OVERRIDE_ANIMATION_FLUTTER);
        }
      } else if (SuperSpinnerOverEndTime) {
          ShowPlayerScores(0xFF, false, false);
      }
      
      if (CurrentTime>SuperSpinnerEndTime) {
        SetGameMode(GAME_MODE_SPINNER_FRENZY_OVER);
      }

      if ( (GateOpenTime != 0) && ((CurrentTime-GateOpenTime) > 1000) ) {  
        RPU_SetDisableGate(false);
        GateOpenTime = 0;
      }
      break;

    case GAME_MODE_SPINNER_FRENZY_OVER:
      if (SuperSpinnerOverStartTime == 0) {
        //RPU_TurnOffAllLamps();
        SuperSpinnerOverStartTime = CurrentTime;
        SuperSpinnerOverEndTime = CurrentTime + SUPER_POP_OVER;
      }
      
      if (CurrentTime>SuperSpinnerOverEndTime) {
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }
      if ( (GateOpenTime != 0) && ((CurrentTime-GateOpenTime) > 1000) ) {  
        RPU_SetDisableGate(false);
        GateOpenTime = 0;
      }
      break;

    case GAME_MODE_BLAST_OFF_COLLECT:
      RPU_SetLampState(LAMP_TOP_S, 1, 0, 500);
      RPU_SetLampState(LAMP_TOP_P, 1, 0, 500);
      RPU_SetLampState(LAMP_TOP_A, 1, 0, 500);
      RPU_SetLampState(LAMP_TOP_C, 1, 0, 500);
      RPU_SetLampState(LAMP_TOP_E, 1, 0, 500);
      
      if (RPU_ReadSingleSwitchState(SW_C_SAUCER)) {
        SuperBlastOffEndTime = 0;
        SetGoals(3);
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
        QueueNotification(SOUND_EFFECT_BLASTOFF_GOAL, 1);
      } 
      
      if (CurrentTime<SuperBlastOffEndTime) {
        for (byte count=0; count<4; count++) {
          if (count!=CurrentPlayer) OverrideScoreDisplay(count, (SuperBlastOffEndTime-CurrentTime)/1000, DISPLAY_OVERRIDE_ANIMATION_FLUTTER);
          }
        } else if (SuperBlastOffOverEndTime) {
          ShowPlayerScores(0xFF, false, false);
      }

      if (CurrentTime>SuperBlastOffEndTime) {
        SetGameMode(GAME_MODE_BLAST_OFF_OVER);
      }

      if (SuperBlastOffStartTime == 0) {
        SuperBlastOffStartTime = CurrentTime;
        SuperBlastOffEndTime = CurrentTime + SUPER_BLASTOFF_DURATION;
      }
      
      if ( (GateOpenTime != 0) && ((CurrentTime-GateOpenTime) > 1000) ) {  
        RPU_SetDisableGate(false);
        GateOpenTime = 0;
      }
      break;

    case GAME_MODE_BLAST_OFF_OVER:
      if (SuperBlastOffOverStartTime == 0) {
        //RPU_TurnOffAllLamps();
        SuperBlastOffOverStartTime = CurrentTime;
        SuperBlastOffOverEndTime = CurrentTime + SUPER_BLASTOFF_OVER;
      }
      
      if (CurrentTime>SuperBlastOffOverEndTime) {
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }
      break;


    case GAME_MODE_POP_FRENZY:
      //Goals[CurrentPlayer] |= GOAL_SUPER_POP_ACHIEVED;
      SetGoals(2);
      RPU_SetLampState(LAMP_LR_POP, 1, 0, 100);
      RPU_SetLampState(LAMP_C_POP, 1, 0, 100);
      
      if (SuperPopStartTime == 0) {
        SuperPopStartTime = CurrentTime;
        SuperPopEndTime = CurrentTime + SUPER_POP_DURATION;
      }

      if (CurrentTime<SuperPopEndTime) {
        for (byte count=0; count<4; count++) {
          if (count!=CurrentPlayer) OverrideScoreDisplay(count, (SuperPopEndTime-CurrentTime)/1000, DISPLAY_OVERRIDE_ANIMATION_FLUTTER);
          }
        } else if (SuperPopOverEndTime) {
          ShowPlayerScores(0xFF, false, false);
      }
      
      if (CurrentTime>SuperPopEndTime) {
        SetGameMode(GAME_MODE_POP_FRENZY_OVER);
      }

      if ( (GateOpenTime != 0) && ((CurrentTime-GateOpenTime) > 1000) ) {  
        RPU_SetDisableGate(false);
        GateOpenTime = 0;
      }  
      break;

    case GAME_MODE_POP_FRENZY_OVER:
      if (SuperPopOverStartTime == 0) {
        //RPU_TurnOffAllLamps();
        SuperPopOverStartTime = CurrentTime;
        SuperPopOverEndTime = CurrentTime + SUPER_POP_OVER;
      }
      
      if (CurrentTime>SuperPopOverEndTime) {
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }

      if ( (GateOpenTime != 0) && ((CurrentTime-GateOpenTime) > 1000) ) {  
        RPU_SetDisableGate(false);
        GateOpenTime = 0;
      }
      break;
  }
  
if ((BallFirstSwitchHitTime == 0) && GoalsDisplayValue(Goals[CurrentPlayer])) {   // If ball not in play and if any goals have been reached
    for (byte count = 0; count < 4; count++) {
      if (count != CurrentPlayer) {
        OverrideScoreDisplay(count, GoalsDisplayValue(Goals[CurrentPlayer]), false);  // Show achieved goals
      }
    }
    GoalsDisplayToggle = true;
  } else if ((BallFirstSwitchHitTime > 0) && GoalsDisplayToggle) {
    ShowPlayerScores(0xFF, false, false);                                             //  Reset all score displays
    GoalsDisplayToggle = false;
  }   

  if ( !specialAnimationRunning && NumTiltWarnings <= MaxTiltWarnings ) {
//    ShowTopSpaceLamps();
//    ShowLowerSpaceLamps();
    ShowBonusLamps();
//    ShowStandupLamps();
    ShowShootAgainLamps();
//    ShowPopBumpersLamps();
//    ShowCenterSpinnerLamps();
  }


  // Three types of display modes are shown here:
  // 1) score animation
  // 2) fly-bys
  // 3) normal scores
  if (ScoreAdditionAnimationStartTime != 0) {
    // Score animation
    if ((CurrentTime - ScoreAdditionAnimationStartTime) < 2000) {
      byte displayPhase = (CurrentTime - ScoreAdditionAnimationStartTime) / 60;
      byte digitsToShow = 1 + displayPhase / 6;
      if (digitsToShow > 6) digitsToShow = 6;
      unsigned long scoreToShow = ScoreAdditionAnimation;
      for (byte count = 0; count < (6 - digitsToShow); count++) {
        scoreToShow = scoreToShow / 10;
      }
      if (scoreToShow == 0 || displayPhase % 2) scoreToShow = DISPLAY_OVERRIDE_BLANK_SCORE;
      byte countdownDisplay = (1 + CurrentPlayer) % 4;

      for (byte count = 0; count < 4; count++) {
        if (count == countdownDisplay) OverrideScoreDisplay(count, scoreToShow, DISPLAY_OVERRIDE_ANIMATION_NONE);
        else if (count != CurrentPlayer) OverrideScoreDisplay(count, DISPLAY_OVERRIDE_BLANK_SCORE, DISPLAY_OVERRIDE_ANIMATION_NONE);
      }
    } else {
      byte countdownDisplay = (1 + CurrentPlayer) % 4;
      unsigned long remainingScore = 0;
      if ( (CurrentTime - ScoreAdditionAnimationStartTime) < 5000 ) {
        remainingScore = (((CurrentTime - ScoreAdditionAnimationStartTime) - 2000) * ScoreAdditionAnimation) / 3000;
        if ((remainingScore / 1000) != (LastRemainingAnimatedScoreShown / 1000)) {
          LastRemainingAnimatedScoreShown = remainingScore;
          PlaySoundEffect(SOUND_EFFECT_RUBBER);
        }
      } else {
        CurrentScores[CurrentPlayer] += ScoreAdditionAnimation;
        remainingScore = 0;
        ScoreAdditionAnimationStartTime = 0;
        ScoreAdditionAnimation = 0;
      }

      for (byte count = 0; count < 4; count++) {
        if (count == countdownDisplay) OverrideScoreDisplay(count, ScoreAdditionAnimation - remainingScore, DISPLAY_OVERRIDE_ANIMATION_NONE);
        else if (count != CurrentPlayer) OverrideScoreDisplay(count, DISPLAY_OVERRIDE_BLANK_SCORE, DISPLAY_OVERRIDE_ANIMATION_NONE);
        else OverrideScoreDisplay(count, CurrentScores[CurrentPlayer] + remainingScore, DISPLAY_OVERRIDE_ANIMATION_NONE);
      }
    }
    if (ScoreAdditionAnimationStartTime) {
      ShowPlayerScores(CurrentPlayer, false, false);
    } else {
      ShowPlayerScores(0xFF, false, false); 
    }
  } else {
    ShowPlayerScores(CurrentPlayer, (BallFirstSwitchHitTime == 0) ? true : false, (BallFirstSwitchHitTime > 0 && ((CurrentTime - LastTimeScoreChanged) > 2000)) ? true : false);

    // Show the player up lamp
    if (BallFirstSwitchHitTime == 0) {
      for (byte count = 0; count < 4; count++) {
//        RPU_SetLampState(LAMP_HEAD_PLAYER_1_UP + count, (((CurrentTime / 250) % 2) == 0 || CurrentPlayer != count) ? false : true);
//        RPU_SetLampState(LAMP_HEAD_1_PLAYER + count, ((count+1)==CurrentNumPlayers) ? true : false);
      }
    } else {
      for (byte count = 0; count < 4; count++) {
//        RPU_SetLampState(LAMP_HEAD_PLAYER_1_UP + count, (CurrentPlayer == count) ? true : false);
//        RPU_SetLampState(LAMP_HEAD_1_PLAYER + count, ((count+1)==CurrentNumPlayers) ? true : false);
      }
    }
  }

  // Check to see if ball is in the outhole
  if (CountBallsInTrough()>(TotalBallsLoaded-(NumberOfBallsInPlay+NumberOfBallsLocked))) {

    if (BallTimeInTrough == 0) {
      // If this is the first time we're seeing too many balls in the trough, we'll wait to make sure 
      // everything is settled
      BallTimeInTrough = CurrentTime;
    } else {
      
      // Make sure the ball stays on the sensor for at least
      // 0.5 seconds to be sure that it's not bouncing or passing through
      if ((CurrentTime - BallTimeInTrough) > 750) {

        if (BallFirstSwitchHitTime == 0 && NumTiltWarnings <= MaxTiltWarnings) {
          // Nothing hit yet, so return the ball to the player
          RPU_PushToTimedSolenoidStack(SOL_OUTHOLE, 16, CurrentTime);
          BallTimeInTrough = 0;
          returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
        } else {
          // if we haven't used the ball save, and we're under the time limit, then save the ball
          if (BallSaveEndTime && CurrentTime<(BallSaveEndTime+BALL_SAVE_GRACE_PERIOD)) {
            RPU_PushToTimedSolenoidStack(SOL_OUTHOLE, 16, CurrentTime + 100);
            QueueNotification(SOUND_EFFECT_BALL_SAVE, 1);
            RPU_SetLampState(LAMP_SHOOT_AGAIN, 0);
            BallTimeInTrough = CurrentTime;
            returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

            // Only 1 ball save if one ball in play
            if (NumberOfBallsInPlay==1) {
              BallSaveEndTime = CurrentTime + 1000;
            } else {
              if (CurrentTime>BallSaveEndTime) BallSaveEndTime += 1000;
            }
           
          } else {

            NumberOfBallsInPlay -= 1;
            if (NumberOfBallsInPlay==0) {
              ShowPlayerScores(0xFF, false, false);
              Audio.StopAllAudio();
              returnState = MACHINE_STATE_COUNTDOWN_BONUS;
            }
          }
        }
      }
    }
  } else {
    BallTimeInTrough = 0;
  }

  LastTimeThroughLoop = CurrentTime;
  return returnState;
}



unsigned long CountdownStartTime = 0;
unsigned long LastCountdownReportTime = 0;
unsigned long BonusCountDownEndTime = 0;
byte DecrementingBonusCounter;
byte IncrementingBonusXCounter;
byte TotalBonus = 0;
byte TotalBonusX = 0;
boolean CountdownBonusHurryUp = false;

int CountDownDelayTimes[] = {175, 130, 105, 90, 80, 70, 60, 40, 30, 20};

int CountdownBonus(boolean curStateChanged) {

  // If this is the first time through the countdown loop
  if (curStateChanged) {

    Bonus[CurrentPlayer] = CurrentBonus;
    CountdownStartTime = CurrentTime;
    LastCountdownReportTime = CurrentTime;
    ShowBonusLamps();
    IncrementingBonusXCounter = 1;
    DecrementingBonusCounter = CurrentBonus;
    TotalBonus = CurrentBonus;
    CountdownBonusHurryUp = false;

    BonusCountDownEndTime = 0xFFFFFFFF;
    // Some sound cards have a special index
    // for a "sound" that will turn off 
    // the current background drone or currently
    // playing sound
//    PlaySoundEffect(SOUND_EFFECT_STOP_BACKGROUND);
  }

  unsigned long countdownDelayTime = (unsigned long)(CountDownDelayTimes[IncrementingBonusXCounter-1]);
  if (CountdownBonusHurryUp && countdownDelayTime>((unsigned long)CountDownDelayTimes[9])) countdownDelayTime=CountDownDelayTimes[9];
  
  if ((CurrentTime - LastCountdownReportTime) > countdownDelayTime) {

    if (DecrementingBonusCounter) {

      // Only give sound & score if this isn't a tilt
      if (NumTiltWarnings <= MaxTiltWarnings) {
        PlaySoundEffect(SOUND_EFFECT_BONUS_COUNT_1k);
        CurrentScores[CurrentPlayer] += 1000;        
      }

      DecrementingBonusCounter -= 1;
      CurrentBonus = DecrementingBonusCounter;
      ShowBonusLamps();

    } else if (BonusCountDownEndTime==0xFFFFFFFF) {
      IncrementingBonusXCounter += 1;
      if (BonusX[CurrentPlayer]>1) {
        DecrementingBonusCounter = TotalBonus;
        CurrentBonus = TotalBonus;
        ShowBonusLamps();
        BonusX[CurrentPlayer] -= 1;
        if (BonusX[CurrentPlayer]==9) BonusX[CurrentPlayer] = 8;
      } else {
        BonusX[CurrentPlayer] = TotalBonusX;
        CurrentBonus = TotalBonus;
        BonusCountDownEndTime = CurrentTime + 1000;
      }
    }
    LastCountdownReportTime = CurrentTime;
  }

  if (CurrentTime > BonusCountDownEndTime) {

    // Reset any lights & variables of goals that weren't completed
    BonusCountDownEndTime = 0xFFFFFFFF;
    return MACHINE_STATE_BALL_OVER;
  }

  return MACHINE_STATE_COUNTDOWN_BONUS;
}



void CheckHighScores() {
  unsigned long highestScore = 0;
  int highScorePlayerNum = 0;
  for (int count = 0; count < CurrentNumPlayers; count++) {
    if (CurrentScores[count] > highestScore) highestScore = CurrentScores[count];
    highScorePlayerNum = count;
  }

  if (highestScore > HighScore) {
    HighScore = highestScore;
    if (HighScoreReplay) {
      AddCredit(false, 3);
      RPU_WriteULToEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE) + 3);
    }
    RPU_WriteULToEEProm(RPU_HIGHSCORE_EEPROM_START_BYTE, highestScore);
    RPU_WriteULToEEProm(RPU_TOTAL_HISCORE_BEATEN_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_HISCORE_BEATEN_START_BYTE) + 1);

    for (int count = 0; count < 4; count++) {
      if (count == highScorePlayerNum) {
        RPU_SetDisplay(count, CurrentScores[count], true, 2);
      } else {
        RPU_SetDisplayBlank(count, 0x00);
      }
    }

    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 20, CurrentTime, true);
    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 20, CurrentTime + 300, true);
    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 20, CurrentTime + 600, true);
  }
}


unsigned long MatchSequenceStartTime = 0;
unsigned long MatchDelay = 150;
byte MatchDigit = 0;
byte NumMatchSpins = 0;
byte ScoreMatches = 0;

int ShowMatchSequence(boolean curStateChanged) {
  if (!MatchFeature) return MACHINE_STATE_ATTRACT;

  if (curStateChanged) {
    MatchSequenceStartTime = CurrentTime + 4000;
    MatchDelay = 1500;
    MatchDigit = CurrentTime % 10;
    NumMatchSpins = 0;
    RPU_SetLampState(LAMP_HEAD_MATCH, 1, 0);
    RPU_SetDisableFlippers();
    ScoreMatches = 0;
  }

  if (NumMatchSpins < 40) {
    if (CurrentTime > (MatchSequenceStartTime + MatchDelay)) {
      MatchDigit += 1;
      if (MatchDigit > 9) MatchDigit = 0;
      //PlaySoundEffect(10+(MatchDigit%2));
      PlaySoundEffect(SOUND_EFFECT_MATCH_SPIN);
      RPU_SetDisplayBallInPlay((int)MatchDigit * 10);
      MatchDelay += 50 + 4 * NumMatchSpins;
      NumMatchSpins += 1;
      RPU_SetLampState(LAMP_HEAD_MATCH, NumMatchSpins % 2, 0);

      if (NumMatchSpins == 40) {
        RPU_SetLampState(LAMP_HEAD_MATCH, 0);
        MatchDelay = CurrentTime - MatchSequenceStartTime;
      }
    }
  }

  if (NumMatchSpins >= 40 && NumMatchSpins <= 43) {
    if (CurrentTime > (MatchSequenceStartTime + MatchDelay)) {
      if ( (CurrentNumPlayers > (NumMatchSpins - 40)) && ((CurrentScores[NumMatchSpins - 40] / 10) % 10) == MatchDigit) {
        ScoreMatches |= (1 << (NumMatchSpins - 40));
        AddSpecialCredit();
        MatchDelay += 1000;
        NumMatchSpins += 1;
        RPU_SetLampState(LAMP_HEAD_MATCH, 1);
      } else {
        NumMatchSpins += 1;
      }
      if (NumMatchSpins == 44) {
        MatchDelay += 5000;
      }
    }
  }

  if (NumMatchSpins > 43) {
    if (CurrentTime > (MatchSequenceStartTime + MatchDelay)) {
      return MACHINE_STATE_ATTRACT;
    }
  }

  for (int count = 0; count < 4; count++) {
    if ((ScoreMatches >> count) & 0x01) {
      // If this score matches, we're going to flash the last two digits
      byte upperMask = 0x0F;
      byte lowerMask = 0x30;
      if (RPU_OS_NUM_DIGITS==7) {
        upperMask = 0x1F;
        lowerMask = 0x60;      
      }
      if ( (CurrentTime / 200) % 2 ) {
        RPU_SetDisplayBlank(count, RPU_GetDisplayBlank(count) & upperMask);
      } else {
        RPU_SetDisplayBlank(count, RPU_GetDisplayBlank(count) | lowerMask);
      }
    }
  }

  return MACHINE_STATE_MATCH_MODE;
}




////////////////////////////////////////////////////////////////////////////
//
//  Switch Handling functions
//
////////////////////////////////////////////////////////////////////////////
/* 
// Example lock function  

void HandleLockSwitch(byte lockIndex) {

  if (GameMode==GAME_MODE_UNSTRUCTURED_PLAY) {
    // If this player has a lock available  
    if (PlayerLockStatus[CurrentPlayer] & (LOCK_1_AVAILABLE<<lockIndex)) {
      // Lock the ball
      LockBall(lockIndex);
      SetGameMode(GAME_MODE_OFFER_LOCK);
    } else {
      if ((MachineLocks & (LOCK_1_ENGAGED<<lockIndex))==0) {
        // Kick unlocked ball
        RPU_PushToSolenoidStack(SOL_UPPER_BALL_EJECT, 12, true);
      }
    }
  }
}

*/

int HandleSystemSwitches(int curState, byte switchHit) {
  int returnState = curState;
  switch (switchHit) {
    case SW_SELF_TEST_SWITCH:
#if (RPU_MPU_ARCHITECTURE<10)
      returnState = MACHINE_STATE_TEST_LAMPS;
#else      
      returnState = MACHINE_STATE_TEST_BOOT;
#endif      
      SetLastSelfTestChangedTime(CurrentTime);
      break;
    case SW_COIN_1:
    case SW_COIN_2:
    case SW_COIN_3:
      AddCoinToAudit(SwitchToChuteNum(switchHit));
      AddCoin(SwitchToChuteNum(switchHit));
      break;
    case SW_CREDIT_RESET:
      if (MachineState == MACHINE_STATE_MATCH_MODE) {
        // If the first ball is over, pressing start again resets the game
        if (Credits >= 1 || FreePlayMode) {
          if (!FreePlayMode) {
            Credits -= 1;
            RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
            RPU_SetDisplayCredits(Credits, !FreePlayMode);
          }
          returnState = MACHINE_STATE_INIT_GAMEPLAY;
        }
      } else {
        CreditResetPressStarted = CurrentTime;
      }
      break;
    case SW_OUTHOLE:
      // Some machines have a kicker to move the ball
      // from the outhole to the re-shooter ramp
//      MoveBallFromOutholeToRamp(true);
      break;
    case SW_PLUMB_TILT:
//    case SW_ROLL_TILT:
    case SW_PLAYFIELD_TILT:
      // This should be debounced
      if ((CurrentTime - LastTiltWarningTime) > TILT_WARNING_DEBOUNCE_TIME) {
        LastTiltWarningTime = CurrentTime;
        NumTiltWarnings += 1;
        if (NumTiltWarnings > MaxTiltWarnings) {
          RPU_DisableSolenoidStack();
          RPU_SetDisableFlippers(true);
          RPU_TurnOffAllLamps();
          RPU_SetLampState(LAMP_HEAD_TILT, 1);
          Audio.StopAllAudio();
        }
        PlaySoundEffect(SOUND_EFFECT_TILT_WARNING);
      }
      break;  
  }

  return returnState;
}


// void HandleDropTarget(byte switchHit) {

//   byte result;
//   unsigned long numTargetsDown = 0;
//   result = HandleDropTarget(switchHit);
//   numTargetsDown = result;
//     CurrentScores[CurrentPlayer] += PlayfieldMultiplier * numTargetsDown * 1000;

//   boolean cleared = DropTargets.CheckIfBankCleared();
//   if (cleared) {
//     PlaySoundEffect(SOUND_EFFECT_ROCKET_BLAST);
//     RPU_SetLampState(LAMP_DROP_TARGET, 1, 0, 500);
//   } else if (numTargetsDown = 1) {
//     RPU_SetLampState(LAMP_CL_WHENLIT, 1, 0, 0);
//     PlaySoundEffect(SOUND_EFFECT_DROPTARGET);
//   } else {
//     PlaySoundEffect(SOUND_EFFECT_DROPTARGET);    
//   }
// }



void HandleGamePlaySwitches(byte switchHit) {

  switch (switchHit) {

    case SW_LEFT_SLING:
    case SW_RIGHT_SLING:
      CurrentScores[CurrentPlayer] += 150 * PlayfieldMultiplier;
      PlaySoundEffect(SOUND_EFFECT_SLINGSHOT);
      SpinnerToggle();
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;

    case SW_L_POP_BUMPER:
    case SW_C_POP_BUMPER:
    case SW_R_POP_BUMPER:
      if (CurrentTime < SuperPopEndTime) {
      SetGameMode(GAME_MODE_POP_FRENZY);
      PlaySoundEffect(SOUND_EFFECT_POPBUMPER);
      CurrentScores[CurrentPlayer] += (SCORE_POPFRENZY) * PlayfieldMultiplier;
      } else { 
      SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }
      if (GameMode==GAME_MODE_UNSTRUCTURED_PLAY) {
        NumberOfHits[CurrentPlayer] += 1;
        if (NumberOfHits[CurrentPlayer] > 25) {
          NumberOfHits[CurrentPlayer] = 0;
          SuperPopEndTime = CurrentTime + SUPER_POP_DURATION;
          RPU_SetDisplayCredits(Credits);
          QueueNotification(SOUND_EFFECT_SUPERPOP_GOAL, 1);
          } else {
        RPU_SetDisplayCredits(0+NumberOfHits[CurrentPlayer]);
        }
      CurrentScores[CurrentPlayer] += 100 * PlayfieldMultiplier;
      PlaySoundEffect(SOUND_EFFECT_POPBUMPER);
      }  
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;  

    case SW_DROP_1:
      CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
      AddToBonus(1);
      PlaySoundEffect(SOUND_EFFECT_DROPTARGET);
      SpinnerToggle();
      RPU_SetLampState(LAMP_5000, 1, 0, 0);
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;
    case SW_DROP_2:
      CurrentScores[CurrentPlayer] += 2000 * PlayfieldMultiplier;
      AddToBonus(1);
      PlaySoundEffect(SOUND_EFFECT_DROPTARGET);
      SpinnerToggle();
      RPU_SetLampState(LAMP_OPENGATE, 1, 0, 0);
      RPU_SetLampState(LAMP_5000, 0, 0, 0);
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;
    case SW_DROP_3:
      CurrentScores[CurrentPlayer] += 3000 * PlayfieldMultiplier;
      AddToBonus(1);
      PlaySoundEffect(SOUND_EFFECT_DROPTARGET);
      SpinnerToggle();
      RPU_SetLampState(LAMP_L_OUTLANE, 1, 0, 0);
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;
    case SW_DROP_4:
      CurrentScores[CurrentPlayer] += 4000 * PlayfieldMultiplier;
      AddToBonus(1);
      PlaySoundEffect(SOUND_EFFECT_BASS_RUMBLE);
      SpinnerToggle();
      RPU_SetLampState(LAMP_DROP_TARGET, 1, 0, 500);
      RPU_SetLampState(LAMP_EXTRABALL, 1, 0, 0);
      RPU_SetLampState(LAMP_OPENGATE, 0, 0, 0);
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;

    case SW_L_SPINNER:
      if (CurrentTime < SuperSpinnerEndTime) {
      SetGameMode(GAME_MODE_SPINNER_FRENZY);
      PlaySoundEffect(SOUND_EFFECT_SPINNERFRENZY);
      CurrentScores[CurrentPlayer] += (SCORE_SPINNERFRENZY) * PlayfieldMultiplier;
      } else { 
      SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }
      if (GameMode==GAME_MODE_UNSTRUCTURED_PLAY) {
        NumberOfSpins[CurrentPlayer] += 1;
        if (NumberOfSpins[CurrentPlayer] > 200) {
          NumberOfSpins[CurrentPlayer] = 1;
          SuperSpinnerEndTime = CurrentTime + SUPER_SPINNER_DURATION;
          RPU_SetDisplayCredits(Credits);
          QueueNotification(SOUND_EFFECT_SUPERSPINNER_GOAL, 1);
          } else {
        RPU_SetDisplayCredits(0+NumberOfSpins[CurrentPlayer]);
        }
        if (NumberOfSpins[CurrentPlayer] > 0 && NumberOfSpins[CurrentPlayer] < 51){
          CurrentScores[CurrentPlayer] += (SCORE_SPINNER1) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_L_SPINNER_100, 1, 0, 0);
          RPU_SetLampState(LAMP_L_SPINNER_200, 0, 0, 0);
          RPU_SetLampState(LAMP_L_SPINNER_1000, 0, 0, 0);
          RPU_SetLampState(LAMP_L_SPINNER_2000, 0, 0, 0);
          PlaySoundEffect(SOUND_EFFECT_SPINNER100);
        }
        if (NumberOfSpins[CurrentPlayer] > 50 && NumberOfSpins[CurrentPlayer] < 101){
          CurrentScores[CurrentPlayer] += (SCORE_SPINNER2) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_L_SPINNER_100, 0, 0, 0);
          RPU_SetLampState(LAMP_L_SPINNER_200, 1, 0, 0);
          RPU_SetLampState(LAMP_L_SPINNER_1000, 0, 0, 0);
          RPU_SetLampState(LAMP_L_SPINNER_2000, 0, 0, 0);
          PlaySoundEffect(SOUND_EFFECT_SPINNER200);
        }
        if (NumberOfSpins[CurrentPlayer] > 100 && NumberOfSpins[CurrentPlayer] < 151){
          CurrentScores[CurrentPlayer] += (SCORE_SPINNER3) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_L_SPINNER_100, 0, 0, 0);
          RPU_SetLampState(LAMP_L_SPINNER_200, 0, 0, 0);
          RPU_SetLampState(LAMP_L_SPINNER_1000, 1, 0, 0);
          RPU_SetLampState(LAMP_L_SPINNER_2000, 0, 0, 0);
          PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
        }
        if (NumberOfSpins[CurrentPlayer] > 150 && NumberOfSpins[CurrentPlayer] < 201){
          CurrentScores[CurrentPlayer] += (SCORE_SPINNER4) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_L_SPINNER_100, 0, 0, 0);
          RPU_SetLampState(LAMP_L_SPINNER_200, 0, 0, 0);
          RPU_SetLampState(LAMP_L_SPINNER_1000, 0, 0, 0);
          RPU_SetLampState(LAMP_L_SPINNER_2000, 1, 0, 0);
          PlaySoundEffect(SOUND_EFFECT_SPINNER2000);
        }
      }
      LastTimeSpinnerHit = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;

    case SW_CL_SPINNER:
      if (CurrentTime < SuperBlastOffEndTime) {
      SetGameMode(GAME_MODE_BLAST_OFF_COLLECT);
      PlaySoundEffect(SOUND_EFFECT_SPINNERCENTER);
      } else { 
      SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }
      if (GameMode==GAME_MODE_UNSTRUCTURED_PLAY) {
        if (NumberOfCenterSpins[CurrentPlayer] > 200) {
            NumberOfCenterSpins[CurrentPlayer] = 1;
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_5, 1, 0, 0);
            SuperBlastOffEndTime = CurrentTime + SUPER_BLASTOFF_DURATION;
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1) * PlayfieldMultiplier;
            RPU_SetDisplayBallInPlay(CurrentBallInPlay);
          } else if (NumberOfCenterSpins[CurrentPlayer] < 1) {
            NumberOfCenterSpins[CurrentPlayer] = 1;
          } else {
            RPU_SetDisplayBallInPlay(0+NumberOfCenterSpins[CurrentPlayer]);
          }
      if (RPU_ReadLampState(LAMP_CL_WHENLIT)) {
        NumberOfCenterSpins[CurrentPlayer] += 1;
        if (NumberOfCenterSpins[CurrentPlayer] > 0 && NumberOfCenterSpins[CurrentPlayer] < 41){
          CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_2, 0, 0, 0);
          RPU_SetLampState(LAMP_C_SPINNER_3, 0, 0, 0);
          RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
          RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
          PlaySoundEffect(SOUND_EFFECT_SPINNER100);
        }
        if (NumberOfCenterSpins[CurrentPlayer] > 40 && NumberOfCenterSpins[CurrentPlayer] < 81){
          CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER2) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_3, 0, 0, 0);  
          RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
          RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
          PlaySoundEffect(SOUND_EFFECT_SPINNER100);
        }
        if (NumberOfCenterSpins[CurrentPlayer] > 80 && NumberOfCenterSpins[CurrentPlayer] < 121){
          CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER3) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
          RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
          PlaySoundEffect(SOUND_EFFECT_SPINNER200);
        }
        if (NumberOfCenterSpins[CurrentPlayer] > 120 && NumberOfCenterSpins[CurrentPlayer] < 161){
          CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER4) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
          PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
        }
        if (NumberOfCenterSpins[CurrentPlayer] > 160 && NumberOfCenterSpins[CurrentPlayer] < 200){
          CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER5) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_5, 1, 0, 100);
          PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
        }
      } else if (RPU_ReadLampState(LAMP_CR_WHENLIT)){
        NumberOfCenterSpins[CurrentPlayer] -= 1;
        if (NumberOfCenterSpins[CurrentPlayer] > 0 && NumberOfCenterSpins[CurrentPlayer] < 41){
          CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_2, 0, 0, 0);
          RPU_SetLampState(LAMP_C_SPINNER_3, 0, 0, 0);
          RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
          RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
          PlaySoundEffect(SOUND_EFFECT_SPINNER100);
        }
        if (NumberOfCenterSpins[CurrentPlayer] > 40 && NumberOfCenterSpins[CurrentPlayer] < 81){
          CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER2) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_3, 0, 0, 0);
          RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
          RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
          PlaySoundEffect(SOUND_EFFECT_SPINNER100);
        }
        if (NumberOfCenterSpins[CurrentPlayer] > 80 && NumberOfCenterSpins[CurrentPlayer] < 121){
          CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER3) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
          RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
          PlaySoundEffect(SOUND_EFFECT_SPINNER200);
        }
        if (NumberOfCenterSpins[CurrentPlayer] > 120 && NumberOfCenterSpins[CurrentPlayer] < 161){
          CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER4) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
          PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
        }
        if (NumberOfCenterSpins[CurrentPlayer] > 160 && NumberOfCenterSpins[CurrentPlayer] < 200){
          CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER5) * PlayfieldMultiplier;
          RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, 100);
          RPU_SetLampState(LAMP_C_SPINNER_5, 1, 0, 100);
          PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
        }
        } else
          CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER0) * PlayfieldMultiplier;
          PlaySoundEffect(SOUND_EFFECT_SPINNER100);
    }
      
    if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
    break;

    case SW_CR_SPINNER:
      if (CurrentTime < SuperBlastOffEndTime) {
      SetGameMode(GAME_MODE_BLAST_OFF_COLLECT);
      PlaySoundEffect(SOUND_EFFECT_SPINNERCENTER);
      CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1) * PlayfieldMultiplier;
      } else { 
      SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }
      if (GameMode==GAME_MODE_UNSTRUCTURED_PLAY) {
        if (NumberOfCenterSpins[CurrentPlayer] > 200) {
            NumberOfCenterSpins[CurrentPlayer] = 1;
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_5, 1, 0, 0);
            SuperBlastOffEndTime = CurrentTime + SUPER_BLASTOFF_DURATION;
            RPU_SetDisplayBallInPlay(CurrentBallInPlay);
            } else if (NumberOfCenterSpins[CurrentPlayer] < 1) {
            NumberOfCenterSpins[CurrentPlayer] = 1;
          } else {
            RPU_SetDisplayBallInPlay(0+NumberOfCenterSpins[CurrentPlayer]);
            }
        if (RPU_ReadLampState(LAMP_CR_WHENLIT)) {
          NumberOfCenterSpins[CurrentPlayer] += 1;
          if (NumberOfCenterSpins[CurrentPlayer] > 0 && NumberOfCenterSpins[CurrentPlayer] < 41){
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1) * PlayfieldMultiplier;
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_3, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
            PlaySoundEffect(SOUND_EFFECT_SPINNER100);
          }
          if (NumberOfCenterSpins[CurrentPlayer] > 40 && NumberOfCenterSpins[CurrentPlayer] < 81){
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER2) * PlayfieldMultiplier;
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_3, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
            PlaySoundEffect(SOUND_EFFECT_SPINNER100);
          }
          if (NumberOfCenterSpins[CurrentPlayer] > 80 && NumberOfCenterSpins[CurrentPlayer] < 121){
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER3) * PlayfieldMultiplier;
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
            PlaySoundEffect(SOUND_EFFECT_SPINNER200);
          }
          if (NumberOfCenterSpins[CurrentPlayer] > 120 && NumberOfCenterSpins[CurrentPlayer] < 161){
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER4) * PlayfieldMultiplier;
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
            PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
          }
          if (NumberOfCenterSpins[CurrentPlayer] > 160 && NumberOfCenterSpins[CurrentPlayer] < 200){
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER5) * PlayfieldMultiplier;
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_5, 1, 0, 100);
            PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
          }
        } else if (RPU_ReadLampState(LAMP_CL_WHENLIT)){
          NumberOfCenterSpins[CurrentPlayer] -= 1;
          if (NumberOfCenterSpins[CurrentPlayer] > 0 && NumberOfCenterSpins[CurrentPlayer] < 41){
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1) * PlayfieldMultiplier;
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_3, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
            PlaySoundEffect(SOUND_EFFECT_SPINNER100);
          }
          if (NumberOfCenterSpins[CurrentPlayer] > 40 && NumberOfCenterSpins[CurrentPlayer] < 81){
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER2) * PlayfieldMultiplier;
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_3, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
            PlaySoundEffect(SOUND_EFFECT_SPINNER100);
          }
          if (NumberOfCenterSpins[CurrentPlayer] > 80 && NumberOfCenterSpins[CurrentPlayer] < 121){
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER3) * PlayfieldMultiplier;
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
            PlaySoundEffect(SOUND_EFFECT_SPINNER200);
          }
          if (NumberOfCenterSpins[CurrentPlayer] > 120 && NumberOfCenterSpins[CurrentPlayer] < 161){
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER4) * PlayfieldMultiplier;
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
            PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
          }
          if (NumberOfCenterSpins[CurrentPlayer] > 160 && NumberOfCenterSpins[CurrentPlayer] < 200){
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER5) * PlayfieldMultiplier;
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_5, 1, 0, 100);
            PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
          }
        } else {
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER0) * PlayfieldMultiplier;
            PlaySoundEffect(SOUND_EFFECT_SPINNER100);
          }
    }
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      
    break;

    case SW_C_SAUCER:
      if (GameMode==GAME_MODE_SKILL_SHOT) {
            QueueNotification(SOUND_EFFECT_SKILLSHOT, 1);
            RPU_PushToTimedSolenoidStack(SOL_C_SAUCER, 16, CurrentTime + 1500, true);
            //RPU_TurnOffAllLamps();
            CurrentScores[CurrentPlayer] += SCORE_SKILL_SHOT;
            SkillShotHit = true;
        } else if (GameMode==GAME_MODE_BLAST_OFF_COLLECT) {
            QueueNotification(SOUND_EFFECT_BLASTOFF_GOAL, 1);
            RPU_PushToTimedSolenoidStack(SOL_C_SAUCER, 16, CurrentTime + 1500, true);
            CurrentScores[CurrentPlayer] += SCORE_BLASTOFF_COLLECT;
            //Goals[CurrentPlayer] |= GOAL_BLAST_OFF_ACHIEVED;
        } else if (GameMode==GAME_MODE_UNSTRUCTURED_PLAY) {
            CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
            PlaySoundEffect(SOUND_EFFECT_ROLL_OVER);
            RPU_PushToTimedSolenoidStack(SOL_C_SAUCER, 16, CurrentTime + 500, true);
        }
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;

    case SW_R_SAUCER:
      CurrentScores[CurrentPlayer] += 25000 * PlayfieldMultiplier;
      QueueNotification(SOUND_EFFECT_PLAYFIELD_MULTI, 1);
      RPU_PushToTimedSolenoidStack(SOL_R_SAUCER, 10, CurrentTime + 3000, true);
      RPU_SetLampState(LAMP_DROP_TARGET, 0, 0, 0);
      IncreasePlayfieldMultiplier(25000);
      RPU_PushToTimedSolenoidStack(SOL_DROP_TARGET_RESET, 10, CurrentTime + 1500, true);
      LastSwitchHitTime = CurrentTime;
      break;

    case SW_R_TARGET:
      TargetBank();
      if (RPU_ReadLampState(LAMP_TARGET_1) && RPU_ReadLampState(LAMP_TARGET_2) && RPU_ReadLampState(LAMP_TARGET_3) &&
        RPU_ReadLampState(LAMP_TARGET_4) && RPU_ReadLampState(LAMP_TARGET_5)) {
        RPU_SetLampState(LAMP_TARGET_1, 0, 0, 0);
        RPU_SetLampState(LAMP_TARGET_2, 0, 0, 0);
        RPU_SetLampState(LAMP_TARGET_3, 0, 0, 0);
        RPU_SetLampState(LAMP_TARGET_4, 0, 0, 0);
        RPU_SetLampState(LAMP_TARGET_5, 0, 0, 0);
        }
      if (RPU_ReadLampState(LAMP_OPENGATE)){
        CurrentScores[CurrentPlayer] += 100 * PlayfieldMultiplier;
        QueueNotification(SOUND_EFFECT_GATEOPEN, 1);
        RPU_SetDisableGate(true);
        GateOpen = false;
        RPU_SetLampState(LAMP_R_OUTLANE, 1, 0, 500);
        RPU_SetLampState(LAMP_OPENGATE, 0, 0, 0);
        } else if (RPU_ReadLampState(LAMP_EXTRABALL)){
        CurrentScores[CurrentPlayer] += 100 * PlayfieldMultiplier;
        AwardExtraBall();
        RPU_SetLampState(LAMP_EXTRABALL, 0, 0, 0);
        RPU_SetLampState(LAMP_5000, 1, 0, 0);
        } else if (RPU_ReadLampState(LAMP_5000)){    
        CurrentScores[CurrentPlayer] += 5000 * PlayfieldMultiplier;
        PlaySoundEffect(SOUND_EFFECT_SWITCHHIT);
        } else
        CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
        PlaySoundEffect(SOUND_EFFECT_SWITCHHIT);
        AddToBonus(1);
      
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;  

    case SW_TARGET1:
      if (RPU_ReadLampState(LAMP_TARGET_2) && RPU_ReadLampState(LAMP_TARGET_3) &&
          RPU_ReadLampState(LAMP_TARGET_4) && RPU_ReadLampState(LAMP_TARGET_5)) {
          TargetBankComplete[CurrentPlayer] += 1;
        }
      CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
      AddToBonus(1);
      PlaySoundEffect(SOUND_EFFECT_OUTLANE);
      RPU_SetLampState(LAMP_TARGET_1, 1, 0, 0);
      SpinnerToggle();
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;

    case SW_TARGET2:
      if (RPU_ReadLampState(LAMP_TARGET_1) && RPU_ReadLampState(LAMP_TARGET_3) &&
          RPU_ReadLampState(LAMP_TARGET_4) && RPU_ReadLampState(LAMP_TARGET_5)) {
          TargetBankComplete[CurrentPlayer] += 1;
        }
      CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
      AddToBonus(1);
      PlaySoundEffect(SOUND_EFFECT_OUTLANE);
      RPU_SetLampState(LAMP_TARGET_2, 1, 0, 0);
      SpinnerToggle();
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;

    case SW_TARGET3:
      if (RPU_ReadLampState(LAMP_TARGET_1) && RPU_ReadLampState(LAMP_TARGET_2) &&
          RPU_ReadLampState(LAMP_TARGET_4) && RPU_ReadLampState(LAMP_TARGET_5)) {
          TargetBankComplete[CurrentPlayer] += 1;
        }
      CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
      AddToBonus(1);
      PlaySoundEffect(SOUND_EFFECT_OUTLANE);
      RPU_SetLampState(LAMP_TARGET_3, 1, 0, 0);
      SpinnerToggle();
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;

    case SW_TARGET4:
      if (RPU_ReadLampState(LAMP_TARGET_1) && RPU_ReadLampState(LAMP_TARGET_2) &&
          RPU_ReadLampState(LAMP_TARGET_3) && RPU_ReadLampState(LAMP_TARGET_5)) {
          TargetBankComplete[CurrentPlayer] += 1;
        }
      CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
      AddToBonus(1);
      PlaySoundEffect(SOUND_EFFECT_OUTLANE);
      RPU_SetLampState(LAMP_TARGET_4, 1, 0, 0);
      SpinnerToggle();
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;

    case SW_TARGET5:
      if (RPU_ReadLampState(LAMP_TARGET_1) && RPU_ReadLampState(LAMP_TARGET_2) &&
          RPU_ReadLampState(LAMP_TARGET_3) && RPU_ReadLampState(LAMP_TARGET_4)) {
          TargetBankComplete[CurrentPlayer] += 1;
        }
      CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
      AddToBonus(1);
      PlaySoundEffect(SOUND_EFFECT_OUTLANE);
      RPU_SetLampState(LAMP_TARGET_5, 1, 0, 0);
      SpinnerToggle();
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;

    case SW_R_INLANE:
      if (RPU_ReadLampState(LAMP_BONUS_200)){
        CurrentScores[CurrentPlayer] += 20000 * PlayfieldMultiplier;
        RPU_SetLampState(LAMP_BONUS_200, 0, 0, 0);
        } else if (RPU_ReadLampState(LAMP_BONUS_300)){
        CurrentScores[CurrentPlayer] += 30000 * PlayfieldMultiplier;
        RPU_SetLampState(LAMP_BONUS_300, 0, 0, 0);
        } else if (RPU_ReadLampState(LAMP_BONUS_400)){
        CurrentScores[CurrentPlayer] += 40000 * PlayfieldMultiplier;
        RPU_SetLampState(LAMP_BONUS_400, 0, 0, 0);
        } else {
        CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
      }
      PlaySoundEffect(SOUND_EFFECT_ROLL_OVER);
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;

    case SW_R_OUTLANE:
      if (GateOpen == false) {
            PlaySoundEffect(SOUND_EFFECT_BALL_SAVE);
            CurrentScores[CurrentPlayer] += 100;
            RPU_SetLampState(LAMP_OPENGATE, 0, 0, 0);
            RPU_SetLampState(LAMP_5000, 1, 0, 0);
            RPU_SetLampState(LAMP_R_OUTLANE, 0, 0, 0);
            GateOpen = true;
            GateOpenTime = CurrentTime;
          } else {
            PlaySoundEffect(SOUND_EFFECT_OUTLANE);
            CurrentScores[CurrentPlayer] += 1000;
          }
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;

    case SW_L_OUTLANE:
      if (RPU_ReadLampState(LAMP_L_OUTLANE)) {
        CurrentScores[CurrentPlayer] += 25000;
      } else {
        CurrentScores[CurrentPlayer] += 1000;
      }
      PlaySoundEffect(SOUND_EFFECT_OUTLANE);
      LastSwitchHitTime = CurrentTime;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;

    case SW_RUBBER:
      CurrentScores[CurrentPlayer] += 10;
      PlaySoundEffect(SOUND_EFFECT_RUBBER);
      SpinnerToggle();
      break;
  }
  
}


int RunGamePlayMode(int curState, boolean curStateChanged) {
  int returnState = curState;
  unsigned long scoreAtTop = CurrentScores[CurrentPlayer];

  // Very first time into gameplay loop
  if (curState == MACHINE_STATE_INIT_GAMEPLAY) {
    returnState = InitGamePlay(curStateChanged);
  } else if (curState == MACHINE_STATE_INIT_NEW_BALL) {
    returnState = InitNewBall(curStateChanged, CurrentPlayer, CurrentBallInPlay);
  } else if (curState == MACHINE_STATE_NORMAL_GAMEPLAY) {
    returnState = ManageGameMode();
  } else if (curState == MACHINE_STATE_COUNTDOWN_BONUS) {
    returnState = CountdownBonus(curStateChanged);
    ShowPlayerScores(0xFF, false, false);
  } else if (curState == MACHINE_STATE_BALL_OVER) {
    RPU_SetDisplayCredits(Credits, !FreePlayMode);

    if (SamePlayerShootsAgain) {
      QueueNotification(SOUND_EFFECT_SHOOTAGAIN, 1);
      returnState = MACHINE_STATE_INIT_NEW_BALL;
    } else {

      CurrentPlayer += 1;
      if (CurrentPlayer >= CurrentNumPlayers) {
        CurrentPlayer = 0;
        CurrentBallInPlay += 1;
      }

      scoreAtTop = CurrentScores[CurrentPlayer];

      if (CurrentBallInPlay > BallsPerGame) {
        CheckHighScores();
        PlaySoundEffect(SOUND_EFFECT_GAME_OVER);
        for (int count = 0; count < CurrentNumPlayers; count++) {
          RPU_SetDisplay(count, CurrentScores[count], true, 2);
        }

        returnState = MACHINE_STATE_MATCH_MODE;
      }
      else returnState = MACHINE_STATE_INIT_NEW_BALL;
    }
  } else if (curState == MACHINE_STATE_MATCH_MODE) {
    returnState = ShowMatchSequence(curStateChanged);
  }

  byte switchHit;
  unsigned long lastBallFirstSwitchHitTime = BallFirstSwitchHitTime;

  while ( (switchHit = RPU_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY ) {
    returnState = HandleSystemSwitches(curState, switchHit);
    if (NumTiltWarnings <= MaxTiltWarnings) HandleGamePlaySwitches(switchHit);
  }

  if (CreditResetPressStarted) {
    if (CurrentBallInPlay < 2) {
      // If we haven't finished the first ball, we can add players
      AddPlayer();
      if (DEBUG_MESSAGES) {
        Serial.write("Start game button pressed\n\r");
      }
      CreditResetPressStarted = 0;
    } else {
      if (RPU_ReadSingleSwitchState(SW_CREDIT_RESET)) {
        if (TimeRequiredToResetGame != 99 && (CurrentTime - CreditResetPressStarted) >= ((unsigned long)TimeRequiredToResetGame*1000)) {
          // If the first ball is over, pressing start again resets the game
          if (Credits >= 1 || FreePlayMode) {
            if (!FreePlayMode) {
              Credits -= 1;
              RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
              RPU_SetDisplayCredits(Credits, !FreePlayMode);
            }
            returnState = MACHINE_STATE_INIT_GAMEPLAY;
            CreditResetPressStarted = 0;
          }
        }
      } else {
        CreditResetPressStarted = 0;
      }
    }

  }

  if (lastBallFirstSwitchHitTime==0 && BallFirstSwitchHitTime!=0) {
    BallSaveEndTime = BallFirstSwitchHitTime + ((unsigned long)BallSaveNumSeconds)*1000;
  }
  if (CurrentTime>(BallSaveEndTime+BALL_SAVE_GRACE_PERIOD)) {
    BallSaveEndTime = 0;
  }

  if (!ScrollingScores && CurrentScores[CurrentPlayer] > RPU_OS_MAX_DISPLAY_SCORE) {
    CurrentScores[CurrentPlayer] -= RPU_OS_MAX_DISPLAY_SCORE;
    if (!TournamentScoring) AddSpecialCredit();
  }

  if (scoreAtTop != CurrentScores[CurrentPlayer]) {
    LastTimeScoreChanged = CurrentTime;
    if (!TournamentScoring) {
      for (int awardCount = 0; awardCount < 3; awardCount++) {
        if (AwardScores[awardCount] != 0 && scoreAtTop < AwardScores[awardCount] && CurrentScores[CurrentPlayer] >= AwardScores[awardCount]) {
          // Player has just passed an award score, so we need to award it
          if (((ScoreAwardReplay >> awardCount) & 0x01)) {
            AddSpecialCredit();
          } else if (!ExtraBallCollected) {
            AwardExtraBall();
          }
        }
      }
    }

  }

  return returnState;
}

unsigned long GoalsDisplayValue(byte currentgoals) {
  unsigned long Result = 0;
  for(int i=0; i<6; i++) {                     // Filter lower 6 goals
    Result = Result * 10;
    if (Goals[CurrentPlayer] & (0b100000 >> i)) {
      Result +=1;
    }
  }
  return Result;
}

//
//  SetGoals Ver 1 
//
// Bit 1 - SuperSpinner Achieved
// Bit 2 - SuperPop Achieved
// Bit 3 - Blast Off Achieved
// Bit 4 - Playfield 5x Achieved
// Bit 5 - SPACE Achieved
// Bit 6 - 3 Goals achieved
// Bit 7 - 5 Goals achieved

void SetGoals(byte goalnum) {   // Set goal flag and update display score

  Goals[CurrentPlayer] = (Goals[CurrentPlayer] | (0b1<<(goalnum - 1))); // Raise flag

  // Count how many goals are met and update display
  unsigned int countOnes = Goals[CurrentPlayer];
  byte numOnes = 0;
  for (int count = 0; count < 6; count++) {
    if ( (countOnes & 0b1) == 1 ) {
      numOnes++;
    }
    countOnes >>= 1;
  }
  
  CurrentScores[CurrentPlayer] = (CurrentScores[CurrentPlayer]/10*10 + numOnes);
}

#if (RPU_MPU_ARCHITECTURE>=10)
unsigned long LastLEDUpdateTime = 0;
byte LEDPhase = 0;
#endif
//unsigned long NumLoops = 0;
//unsigned long LastLoopReportTime = 0;

void loop() {

/*
  if (DEBUG_MESSAGES) {
    NumLoops += 1;
    if (CurrentTime>(LastLoopReportTime+1000)) {
      LastLoopReportTime = CurrentTime;
      char buf[128];
      sprintf(buf, "Loop running at %lu Hz\n", NumLoops);
      Serial.write(buf);
      NumLoops = 0;
    }
  }
*/
  
  CurrentTime = millis();
  int newMachineState = MachineState;

  if (MachineState < 0) {
    newMachineState = RunSelfTest(MachineState, MachineStateChanged);
  } else if (MachineState == MACHINE_STATE_ATTRACT) {
    newMachineState = RunAttractMode(MachineState, MachineStateChanged);
  } else if (MachineState == MACHINE_STATE_DIAGNOSTICS) {
    newMachineState = RunDiagnosticsMode(MachineState, MachineStateChanged);
  } else {
    newMachineState = RunGamePlayMode(MachineState, MachineStateChanged);
  }

  if (newMachineState != MachineState) {
    MachineState = newMachineState;
    MachineStateChanged = true;
  } else {
    MachineStateChanged = false;
  }

  RPU_Update(CurrentTime);
  Audio.Update(CurrentTime);

#if (RPU_MPU_ARCHITECTURE>=10)
  if (LastLEDUpdateTime == 0 || (CurrentTime - LastLEDUpdateTime) > 250) {
    LastLEDUpdateTime = CurrentTime;
    RPU_SetBoardLEDs((LEDPhase % 8) == 1 || (LEDPhase % 8) == 3, (LEDPhase % 8) == 5 || (LEDPhase % 8) == 7);
    LEDPhase += 1;
  }
#endif  
  
}

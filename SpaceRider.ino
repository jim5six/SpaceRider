/**************************************************************************
    This is replacement code for Geiger Space Rider, a German Retheme of Harlem Globe Trotters
    and is available WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See <https://www.gnu.org/licenses/>.
***************************************************************************/

#include "RPU.h"
#include "RPU_Config.h"

// #include "DropTargets.h"
#include "AudioHandler.h"
#include "GameModeManager.h"
#include "LampAnimations.h"
#include "TrevorLampAnimations.h"
#include "SelfTestAndAudit.h"
#include "SpaceRider.h"
#include <EEPROM.h>

#define WIZARD_TEST_MODE (false)

#define GAME_MAJOR_VERSION 2024
#define GAME_MINOR_VERSION 1
#define DEBUG_MESSAGES 1

/*********************************************************************

    Game specific code

*********************************************************************/

// MachineState
//  0 - Attract Mode
//  negative - self-test modes
//  positive - game play
char MachineState = 0;
boolean MachineStateChanged = true;
boolean FirstGame = true;
#define MACHINE_STATE_ATTRACT 0
#define MACHINE_STATE_INIT_GAMEPLAY 1
#define MACHINE_STATE_INIT_NEW_BALL 2
#define MACHINE_STATE_NORMAL_GAMEPLAY 4
#define MACHINE_STATE_COUNTDOWN_BONUS 99
#define MACHINE_STATE_BALL_OVER 100
#define MACHINE_STATE_MATCH_MODE 110
#define MACHINE_STATE_DIAGNOSTICS 120

#define MACHINE_STATE_ADJUST_FREEPLAY (MACHINE_STATE_TEST_DONE - 1)
#define MACHINE_STATE_ADJUST_BALL_SAVE (MACHINE_STATE_TEST_DONE - 2)
#define MACHINE_STATE_ADJUST_SOUND_SELECTOR (MACHINE_STATE_TEST_DONE - 3)
#define MACHINE_STATE_ADJUST_MUSIC_VOLUME (MACHINE_STATE_TEST_DONE - 4)
#define MACHINE_STATE_ADJUST_SFX_VOLUME (MACHINE_STATE_TEST_DONE - 5)
#define MACHINE_STATE_ADJUST_CALLOUTS_VOLUME (MACHINE_STATE_TEST_DONE - 6)
#define MACHINE_STATE_ADJUST_TOURNAMENT_SCORING (MACHINE_STATE_TEST_DONE - 7)
#define MACHINE_STATE_ADJUST_TILT_WARNING (MACHINE_STATE_TEST_DONE - 8)
#define MACHINE_STATE_ADJUST_AWARD_OVERRIDE (MACHINE_STATE_TEST_DONE - 9)
#define MACHINE_STATE_ADJUST_BALLS_OVERRIDE (MACHINE_STATE_TEST_DONE - 10)
#define MACHINE_STATE_ADJUST_SCROLLING_SCORES (MACHINE_STATE_TEST_DONE - 11)
#define MACHINE_STATE_ADJUST_EXTRA_BALL_AWARD (MACHINE_STATE_TEST_DONE - 12)
#define MACHINE_STATE_ADJUST_SPECIAL_AWARD (MACHINE_STATE_TEST_DONE - 13)
#define MACHINE_STATE_ADJUST_CREDIT_RESET_HOLD_TIME (MACHINE_STATE_TEST_DONE - 14)
#define MACHINE_STATE_ADJUST_WIZARD_DIFFICULTY (MACHINE_STATE_TEST_DONE - 15)
#define MACHINE_STATE_ADJUST_DONE (MACHINE_STATE_TEST_DONE - 16)

// Indices of EEPROM save locations
#define EEPROM_BALL_SAVE_BYTE 100
#define EEPROM_FREE_PLAY_BYTE 101
#define EEPROM_SOUND_SELECTOR_BYTE 102
#define EEPROM_SKILL_SHOT_BYTE 103
#define EEPROM_TILT_WARNING_BYTE 104
#define EEPROM_AWARD_OVERRIDE_BYTE 105
#define EEPROM_BALLS_OVERRIDE_BYTE 106
#define EEPROM_TOURNAMENT_SCORING_BYTE 107
#define EEPROM_SFX_VOLUME_BYTE 108
#define EEPROM_MUSIC_VOLUME_BYTE 109
#define EEPROM_SCROLLING_SCORES_BYTE 110
#define EEPROM_CALLOUTS_VOLUME_BYTE 111
#define EEPROM_CRB_HOLD_TIME 118
#define EEPROM_EXTRA_BALL_SCORE_UL 140
#define EEPROM_SPECIAL_SCORE_UL 144
#define EEPROM_WIZARD_HARD_MODE_BYTE 148 // TODO: Byte offset is TBD, do we need to program EEPROM first?

// Sound Effects
#define SOUND_EFFECT_NONE               0
#define SOUND_EFFECT_BONUS_COUNT_1k     2
#define SOUND_EFFECT_ROLL_OVER          3
#define SOUND_EFFECT_OUTLANE            4
#define SOUND_EFFECT_ALARM              5
#define SOUND_EFFECT_SLINGSHOT          6
#define SOUND_EFFECT_POPBUMPER          7
#define SOUND_EFFECT_WIZARD_BG          8
#define SOUND_EFFECT_DROPTARGET         9
#define SOUND_EFFECT_ADDCREDIT          10
#define SOUND_EFFECT_WIZARDTARGET1      11
#define SOUND_EFFECT_WIZARDTARGET2      12
#define SOUND_EFFECT_WIZARDTARGET3      13
#define SOUND_EFFECT_WIZARDTARGET4      14
#define SOUND_EFFECT_WIZARDTARGET5      15
#define SOUND_EFFECT_WIZARDTARGET6      16
#define SOUND_EFFECT_BALL_OVER          19
#define SOUND_EFFECT_GAME_OVER          20
#define SOUND_EFFECT_BACKGROUND1        21
#define SOUND_EFFECT_BACKGROUND2        22
#define SOUND_EFFECT_BACKGROUND3        23
#define SOUND_EFFECT_BACKGROUND4        24
#define SOUND_EFFECT_SWITCHHIT          26
#define SOUND_EFFECT_TILT_WARNING       28
#define SOUND_EFFECT_MATCH_SPIN         30
#define SOUND_EFFECT_SPINNER100         32
#define SOUND_EFFECT_SPINNER200         33
#define SOUND_EFFECT_SPINNER1000        34
#define SOUND_EFFECT_SPINNERFRENZY      35
#define SOUND_EFFECT_SPINNER2000        36
#define SOUND_EFFECT_SPINNERCENTER      37
#define SOUND_EFFECT_SPINNER6           38
#define SOUND_EFFECT_SPINNER7           39
#define SOUND_EFFECT_SPINNER8           40
#define SOUND_EFFECT_SPINNER9           41
#define SOUND_EFFECT_BOING              45
#define SOUND_EFFECT_HURRY_UP           46
#define SOUND_EFFECT_BACKGROUND5        47
#define SOUND_EFFECT_BONUS_COUNT_2k     48
#define SOUND_EFFECT_BONUS_COUNT_3k     49
#define SOUND_EFFECT_RUBBER             50
#define SOUND_EFFECT_BACKGROUND6        51

#if (RPU_MPU_ARCHITECTURE<10) && !defined(RPU_OS_DISABLE_CPC_FOR_SPACE)
// This array maps the self-test modes to audio callouts
unsigned short SelfTestStateToCalloutMap[34] = {136, 137, 135, 134, 133, 140, 141, 142, 139, 143, 144, 145, 146, 147, 148, 149, 138, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166};
#elif (RPU_MPU_ARCHITECTURE < 10) && defined(RPU_OS_DISABLE_CPC_FOR_SPACE)
unsigned short SelfTestStateToCalloutMap[31] = {136, 137, 135, 134, 133, 140, 141, 142, 139, 143, 144, 145, 146, 147, 148, 149, 138, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166};
#elif (RPU_MPU_ARCHITECTURE >= 10) && !defined(RPU_OS_DISABLE_CPC_FOR_SPACE)
// This array maps the self-test modes to audio callouts
unsigned short SelfTestStateToCalloutMap[34] = {134, 135, 133, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166};
#elif (RPU_MPU_ARCHITECTURE >= 10) && defined(RPU_OS_DISABLE_CPC_FOR_SPACE)
unsigned short SelfTestStateToCalloutMap[34] = {134, 135, 133, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166};
#endif

#define SOUND_EFFECT_SELF_TEST_MODE_START 132
#define SOUND_EFFECT_SELF_TEST_CPC_START 180
#define SOUND_EFFECT_SELF_TEST_AUDIO_OPTIONS_START 190

// Game play status callouts
#define NUM_VOICE_NOTIFICATIONS 64

#define SOUND_EFFECT_BALL_SAVE 300
#define SOUND_EFFECT_EXTRABALL 301
#define SOUND_EFFECT_GATEOPEN 302
#define SOUND_EFFECT_PLAYERADDED 303
#define SOUND_EFFECT_SHOOTAGAIN 304
#define SOUND_EFFECT_SKILLSHOT 305
#define SOUND_EFFECT_SUPERSPINNER_GOAL 306
#define SOUND_EFFECT_BLASTOFF_GOAL 307
#define SOUND_EFFECT_SPINNER_HELD 308
#define SOUND_EFFECT_SKILLSHOT_MISSED 309
#define SOUND_EFFECT_BLASTOFF_HELD 310
#define SOUND_EFFECT_POP_HELD 311
#define SOUND_EFFECT_SUPERPOP_GOAL 312
#define SOUND_EFFECT_PLAYFIELDX_HELD 313
#define SOUND_EFFECT_GATE_LIT 314
#define SOUND_EFFECT_GATE_CLOSED 315
#define SOUND_EFFECT_L_OUTLANE_LIT 316
#define SOUND_EFFECT_EXTRABALL_LIT 317
#define SOUND_EFFECT_GOAL_ACHIEVED 318
#define SOUND_EFFECT_GAME_START 319
#define SOUND_EFFECT_RIDER1 320
#define SOUND_EFFECT_RIDER2 321
#define SOUND_EFFECT_RIDER3 322
#define SOUND_EFFECT_RIDER4 323
#define SOUND_EFFECT_MULTI_2X 324
#define SOUND_EFFECT_MULTI_3X 325
#define SOUND_EFFECT_MULTI_5X 326
#define SOUND_EFFECT_MULTI_GOAL 327
#define SOUND_EFFECT_SPACE_GOAL 328
#define SOUND_EFFECT_BONUS_HELD 329
#define SOUND_EFFECT_SPECIAL 330
#define SOUND_EFFECT_WIZARD_MODE_START 331
#define SOUND_EFFECT_WIZARD_MODE_END 332
#define SOUND_EFFECT_WIZARD_MODE_COMPLETE 333
#define SOUND_EFFECT_WIZARD_MODE_FAILED 334
#define SOUND_EFFECT_WIZARD_MODE_INSTRUCT 335
#define SOUND_EFFECT_SUPERSPINNER_EXTRABALL 336
#define SOUND_EFFECT_BLASTOFF_EXTRABALL 337
#define SOUND_EFFECT_SUPERPOPS_EXTRABALL 338
#define SOUND_EFFECT_PLAYFIELDX_EXTRABALL 339
#define SOUND_EFFECT_BONUS_ACHIEVED 340
#define SOUND_EFFECT_BONUS_EXTRABALL 341
#define SOUND_EFFECT_BONUS_WIZARD 342
#define SOUND_EFFECT_SUPERSPINNER_WIZARD 343
#define SOUND_EFFECT_BLASTOFF_WIZARD 344
#define SOUND_EFFECT_SUPERPOPS_WIZARD 345
#define SOUND_EFFECT_PLAYFIELDX_WIZARD 346
#define SOUND_EFFECT_WIZARD_COLLECT 347
#define SOUND_EFFECT_FIRE_ROCKET 348

#define SOUND_EFFECT_DIAG_START 1900
#define SOUND_EFFECT_DIAG_CREDIT_RESET_BUTTON 1900
#define SOUND_EFFECT_DIAG_SELECTOR_SWITCH_ON 1901
#define SOUND_EFFECT_DIAG_SELECTOR_SWITCH_OFF 1902
#define SOUND_EFFECT_DIAG_STARTING_ORIGINAL_CODE 1903
#define SOUND_EFFECT_DIAG_STARTING_NEW_CODE 1904
#define SOUND_EFFECT_DIAG_ORIGINAL_CPU_DETECTED 1905
#define SOUND_EFFECT_DIAG_ORIGINAL_CPU_RUNNING 1906
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_U10 1907
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_U11 1908
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_1 1909
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_2 1910
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_3 1911
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_4 1912
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_5 1913
#define SOUND_EFFECT_DIAG_STARTING_DIAGNOSTICS 1914

#define MAX_DISPLAY_BONUS 40
#define TILT_WARNING_DEBOUNCE_TIME 1000

// Wizard mode goals JIm - moved to the spacerider.h file with the other score settings

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
byte NumberOfSpins[4]; // Left spinner spins
byte NumberOfCenterSpins[4]; // Center spinner spins
byte NumberOfHits[4];
byte PlayfieldX[4];
byte Goals[4] = {0, 0, 0, 0};
byte TargetBankComplete[4];
byte LampType = 0;
boolean SkillShotHit = false;
boolean FreePlayMode = true;
boolean GoalsDisplayToggle;
boolean HighScoreReplay = true;
boolean MatchFeature = true;
boolean TournamentScoring = false;
boolean ScrollingScores = false;
boolean WizardHardMode = false; // Makes wizard mode harder to achieve
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
struct GameGoals {
    bool S_Complete; // Spinner
    bool P_Complete; // Pop bumpers
    bool A_Complete; // Blast Off
    bool C_Complete; // Bonus X
    bool E_Complete; // Playfield X
};

GameGoals PlayerGoalProgress[4] = {
    {false, false, false, false, false}, // Player 1
    {false, false, false, false, false}, // Player 2
    {false, false, false, false, false}, // Player 3
    {false, false, false, false, false}  // Player 4
};

struct WizardModeTracker {
    bool TopPopsHit = false;
    bool BottomPopHit = false;
    unsigned long LeftSpinnerSpins = 0;
    unsigned long CenterSpinnerSpins = 0;
    bool InlineTargetsCompleted = 0;
    bool CenterSaucerHit = false;
    bool RightTargetHit = false;
    bool Target1Hit = false;
    bool Target2Hit = false;
    bool Target3Hit = false;
    bool Target4Hit = false;
    bool Target5Hit = false;
};

WizardModeTracker WizardModeProgress = {};

byte CurrentPlayer = 0;
byte CurrentBallInPlay = 1;
byte CurrentNumPlayers = 0;
byte Bonus[4];
byte BonusX[4];
unsigned long PlayfieldMultiplier[4];
byte MaxTiltWarnings = 2;
byte NumTiltWarnings = 0;
byte AwardPhase;
bool SkillShotActive = false; // Means no switches have been hit yet
const unsigned long SkillShotGracePeriodMs = 30000;
unsigned long SkillShotGracePeroidEnd = 0;
unsigned long SkillShotCelebrationBlinkEndTime = 0;
bool IsAnyModeActive = false;
bool HOLD_SPINNER_PROGRESS[4];  //"S"
bool HOLD_POP_PROGRESS[4];      //"P"
bool HOLD_BLASTOFF_PROGRESS[4]; //"A"
bool HOLD_BONUS[4];             //"C"
bool HOLD_PLAYFIELDX[4];        //"E"

bool SamePlayerShootsAgain = false;
bool PreparingWizardMode = false; // Goals have been achieved and ball is draining to start wizard
unsigned long BonusBeforeWizardMode = 0; // Player's bonus before wizard mode started, to return when it is over
bool WizardModeActive = false; // Wizard mode is in play, goals have not been completed
bool WizardModeEnding = false; // Wizard mode has been completed and game is returning to normal play
unsigned long WizardModeEndTime = 0;
bool BallSaveUsed = false;
bool ExtraBallCollected = false;
bool GoalExtraBallCollected = false;
bool SpecialCollected = false;
bool AllowResetAfterBallOne = true;
bool DisableBallSaveThisBall = false;

enum EnumCenterSpinnerStatus {
    CENTER_LEFT_SPINNER_LIT = 0,
    CENTER_RIGHT_SPINNER_LIT
};

EnumCenterSpinnerStatus CenterSpinnerStatus = CENTER_LEFT_SPINNER_LIT; // Whether the right spinner is lit for blast off count

enum EnumSpaceStatus {
    SPACE_LIT = 0,
    SPACE_UNLIT
};

EnumSpaceStatus SpaceStatus = SPACE_LIT;

unsigned long CurrentScores[4];
unsigned long BallFirstSwitchHitTime = 0;
unsigned long BallTimeInTrough = 0;
unsigned long LastTiltWarningTime = 0;
unsigned long ScoreAdditionAnimation;
unsigned long ScoreAdditionAnimationStartTime;
unsigned long LastRemainingAnimatedScoreShown;
unsigned long LastTimeThroughLoop;
unsigned long LastSwitchHitTime;
unsigned long BallSaveEndTime;
unsigned long SuperBlastOffCollectedHoldTime = 0;

#define BALL_SAVE_GRACE_PERIOD 2000

/*********************************************************************

    Game Specific State Variables

*********************************************************************/
unsigned long BonusChanged;
unsigned long BonusXAnimationStart;
boolean GateOpen = true;
unsigned long GateOpenTime = 0;

// DropTargetBank DropTargets(4, 1, DROP_TARGET_TYPE_BLY_1, 50);

/******************************************************
 *
 * Adjustments Serialization
 *
 */

void ReadStoredParameters() {
    for (byte count = 0; count < 3; count++) {
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
    if (MusicVolume > 10) MusicVolume = 10;

    SoundEffectsVolume = ReadSetting(EEPROM_SFX_VOLUME_BYTE, 10);
    if (SoundEffectsVolume > 10) SoundEffectsVolume = 10;

    CalloutsVolume = ReadSetting(EEPROM_CALLOUTS_VOLUME_BYTE, 10);
    if (CalloutsVolume > 10) CalloutsVolume = 10;

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

    //Just want to run this once to initialize
    //EEPROM.write(EEPROM_WIZARD_HARD_MODE_BYTE, 0); 
    
    //WizardHardMode = ReadSetting(EEPROM_WIZARD_HARD_MODE_BYTE, 1) ? true : false;

    TimeRequiredToResetGame = ReadSetting(EEPROM_CRB_HOLD_TIME, 1);
    if (TimeRequiredToResetGame > 3 && TimeRequiredToResetGame != 99) TimeRequiredToResetGame = 1;

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
        Serial.println("Starting");
    }

    // Set up the Audio handler in order to play boot messages
    CurrentTime = millis();
    Audio.InitDevices(AUDIO_PLAY_TYPE_WAV_TRIGGER | AUDIO_PLAY_TYPE_ORIGINAL_SOUNDS);
    Audio.StopAllAudio();

    // Tell the OS about game-specific switches
    // (this is for software-controlled pop bumpers and slings)
#if (RPU_MPU_ARCHITECTURE < 10)
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
    initResult = RPU_InitializeMPU(RPU_CMD_BOOT_ORIGINAL_IF_CREDIT_RESET | RPU_CMD_BOOT_ORIGINAL_IF_NOT_SWITCH_CLOSED |
                                       RPU_CMD_INIT_AND_RETURN_EVEN_IF_ORIGINAL_CHOSEN | RPU_CMD_PERFORM_MPU_TEST,
                                   SW_CREDIT_RESET);

    if (DEBUG_MESSAGES) {
        char buf[128];
        sprintf(buf, "Return from init = 0x%04lX\n", initResult);
        Serial.write(buf);
        if (initResult & RPU_RET_6800_DETECTED)
            Serial.write("Detected 6800 clock\n");
        else if (initResult & RPU_RET_6802_OR_8_DETECTED)
            Serial.write("Detected 6802/8 clock\n");
        Serial.write("Back from init\n");
    }

    // if (initResult & RPU_RET_SELECTOR_SWITCH_ON) QueueDIAGNotification(SOUND_EFFECT_DIAG_SELECTOR_SWITCH_ON);
    // else QueueDIAGNotification(SOUND_EFFECT_DIAG_SELECTOR_SWITCH_OFF);

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
        while (Audio.Update(millis()))
            ;
        // Arduino should hang if original code is running
        while (1)
            ;
    }
    // QueueDIAGNotification(SOUND_EFFECT_DIAG_STARTING_NEW_CODE);

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
    //RPU_PushToTimedSolenoidStack(SOL_DROP_TARGET_RESET, 10, CurrentTime, true);

    Audio.SetMusicDuckingGain(12);
    Audio.QueueSound(SOUND_EFFECT_GAME_START, AUDIO_PLAY_TYPE_WAV_TRIGGER, CurrentTime + 1200);
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

void ShowBonusLamps() {
    if (IsSuperSuperBlastOffActive(CurrentTime) || WizardModeActive)
    {
        return;
    }

    if (SkillShotActive || CurrentTime <= SkillShotGracePeroidEnd) {
        // Don't fight the animation
        return;
    }

    byte bonus = Bonus[CurrentPlayer];
    if (bonus > MAX_DISPLAY_BONUS) bonus = MAX_DISPLAY_BONUS;

    if (bonus >= 40) {
        RPU_SetLampState(LAMP_BONUS_400, 1, 0, 0);

        // Flash all other bonus lamps
        RPU_SetLampState(LAMP_BONUS_10, 1, 0, 500);
        RPU_SetLampState(LAMP_BONUS_20, 1, 0, 500);
        RPU_SetLampState(LAMP_BONUS_30, 1, 0, 500);
        RPU_SetLampState(LAMP_BONUS_40, 1, 0, 500);
        RPU_SetLampState(LAMP_BONUS_50, 1, 0, 500);
        RPU_SetLampState(LAMP_BONUS_60, 1, 0, 500);
        RPU_SetLampState(LAMP_BONUS_70, 1, 0, 500);
        RPU_SetLampState(LAMP_BONUS_80, 1, 0, 500);
        RPU_SetLampState(LAMP_BONUS_90, 1, 0, 500);
        RPU_SetLampState(LAMP_BONUS_100, 1, 0, 500);
        RPU_SetLampState(LAMP_BONUS_200, 1, 0, 500);
        RPU_SetLampState(LAMP_BONUS_300, 1, 0, 500);
    } 
    else {
        RPU_SetLampState(LAMP_BONUS_400, 0, 0, 0);

        if (bonus >= 30) {
            RPU_SetLampState(LAMP_BONUS_300, 1, 0, 0);
            bonus -= 30;
        }
        else {
            RPU_SetLampState(LAMP_BONUS_300, 0, 0, 0);
        }

        if (bonus >= 20) {
            RPU_SetLampState(LAMP_BONUS_200, 1, 0, 0);
            bonus -= 20;
        }
        else {
            RPU_SetLampState(LAMP_BONUS_200, 0, 0, 0);
        }

        if (bonus >= 10) {
            RPU_SetLampState(LAMP_BONUS_100, 1, 0, 0);
            bonus -= 10;
        }
        else {
            RPU_SetLampState(LAMP_BONUS_100, 0, 0, 0);
        }

        for (byte count = 1; count < 10; count++) {
            if (bonus == count) {
                RPU_SetLampState(LAMP_BONUS_10 + (count - 1), 1, 0, 100);
            } else {
                RPU_SetLampState(LAMP_BONUS_10 + (count - 1), 0, 0, 100);
            }
        }
    }
}

void ShowLeftSpinnerLamps(void) {
    if (SkillShotActive || CurrentTime <= SkillShotGracePeroidEnd) {
        // Don't fight the animation
        return;
    }

    if (IsSuperSpinnerActive(CurrentTime)) {
        ShowLampAnimation(4, 240, CurrentTime, 23, false, false);
    } else if (WizardModeActive) {
        if (WizardModeProgress.LeftSpinnerSpins >= WIZARD_MODE_LEFT_SPINS_REQUIRED / 4) {
            RPU_SetLampState(LAMP_L_SPINNER_2000, 0, 0, 0);
        } else {
            RPU_SetLampState(LAMP_L_SPINNER_2000, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
        }
        if (WizardModeProgress.LeftSpinnerSpins >= WIZARD_MODE_LEFT_SPINS_REQUIRED / 2) {
            RPU_SetLampState(LAMP_L_SPINNER_1000, 0, 0, 0);
        } else {
            RPU_SetLampState(LAMP_L_SPINNER_1000, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
        }
        if (WizardModeProgress.LeftSpinnerSpins >= WIZARD_MODE_LEFT_SPINS_REQUIRED * (.75)) {
            RPU_SetLampState(LAMP_L_SPINNER_200, 0, 0, 0);
        } else {
            RPU_SetLampState(LAMP_L_SPINNER_200, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
        }
        if (WizardModeProgress.LeftSpinnerSpins >= WIZARD_MODE_LEFT_SPINS_REQUIRED) {
            RPU_SetLampState(LAMP_L_SPINNER_100, 0, 0, 0);
        } else {
            RPU_SetLampState(LAMP_L_SPINNER_100, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
        }
    } else {
        if (NumberOfSpins[CurrentPlayer] >= 0 && NumberOfSpins[CurrentPlayer] < 51) {
            RPU_SetLampState(LAMP_L_SPINNER_100, 1, 0, 0);
            RPU_SetLampState(LAMP_L_SPINNER_200, 0, 0, 0);
            RPU_SetLampState(LAMP_L_SPINNER_1000, 0, 0, 0);
            RPU_SetLampState(LAMP_L_SPINNER_2000, 0, 0, 0);
        }
        if (NumberOfSpins[CurrentPlayer] > 50 && NumberOfSpins[CurrentPlayer] < 101) {
            RPU_SetLampState(LAMP_L_SPINNER_100, 0, 0, 0);
            RPU_SetLampState(LAMP_L_SPINNER_200, 1, 0, 0);
            RPU_SetLampState(LAMP_L_SPINNER_1000, 0, 0, 0);
            RPU_SetLampState(LAMP_L_SPINNER_2000, 0, 0, 0);
        }
        if (NumberOfSpins[CurrentPlayer] > 100 && NumberOfSpins[CurrentPlayer] < 151) {
            RPU_SetLampState(LAMP_L_SPINNER_100, 0, 0, 0);
            RPU_SetLampState(LAMP_L_SPINNER_200, 0, 0, 0);
            RPU_SetLampState(LAMP_L_SPINNER_1000, 1, 0, 0);
            RPU_SetLampState(LAMP_L_SPINNER_2000, 0, 0, 0);
        }
        if (NumberOfSpins[CurrentPlayer] > 150 && NumberOfSpins[CurrentPlayer] < 200) {
            RPU_SetLampState(LAMP_L_SPINNER_100, 0, 0, 0);
            RPU_SetLampState(LAMP_L_SPINNER_200, 0, 0, 0);
            RPU_SetLampState(LAMP_L_SPINNER_1000, 0, 0, 0);
            RPU_SetLampState(LAMP_L_SPINNER_2000, 1, 0, 0);
        }
    }
}

void ShowCenterSpinnerLamps() {
    if (SkillShotActive || CurrentTime <= SkillShotGracePeroidEnd) {
        // Don't fight the animation
        return;
    }

    if (IsSuperSuperBlastOffActive(CurrentTime)) {
        ShowLampAnimation(2, 144, CurrentTime, 23, false, false);
    } else if (WizardModeActive) {
         if (WizardModeProgress.CenterSpinnerSpins >= WIZARD_MODE_CENTER_SPINS_REQUIRED / 2) {
            RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
         } else {
            RPU_SetLampState(LAMP_C_SPINNER_5, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
         }
         if (WizardModeProgress.CenterSpinnerSpins >= WIZARD_MODE_CENTER_SPINS_REQUIRED) {
            RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
         } else {
            RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
         }
    } else {
        if (NumberOfCenterSpins[CurrentPlayer] >= 0 && NumberOfCenterSpins[CurrentPlayer] < 21) {
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_3, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
        }
        if (NumberOfCenterSpins[CurrentPlayer] > 20 && NumberOfCenterSpins[CurrentPlayer] < 41) {
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_3, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
        }
        if (NumberOfCenterSpins[CurrentPlayer] > 40 && NumberOfCenterSpins[CurrentPlayer] < 61) {
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_4, 0, 0, 0);
            RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
        }
        if (NumberOfCenterSpins[CurrentPlayer] > 60 && NumberOfCenterSpins[CurrentPlayer] < 81) {
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_5, 0, 0, 0);
        }
        if (NumberOfCenterSpins[CurrentPlayer] > 80 && NumberOfCenterSpins[CurrentPlayer] < 100) {
            RPU_SetLampState(LAMP_C_SPINNER_1, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_2, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_3, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_4, 1, 0, 100);
            RPU_SetLampState(LAMP_C_SPINNER_5, 1, 0, 100);
        }
    }
}    

void ShowShootAgainLamps() {
    if ((BallFirstSwitchHitTime == 0 && BallSaveNumSeconds) || (BallSaveEndTime && CurrentTime < BallSaveEndTime)) {
        unsigned long msRemaining = 5000;
        if (BallSaveEndTime != 0) msRemaining = BallSaveEndTime - CurrentTime;
        RPU_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, (msRemaining < 5000) ? 100 : 500);
        RPU_SetLampState(LAMP_HEAD_SAME_PLAYER_SHOOTS_AGAIN, 1, 0, (msRemaining < 5000) ? 100 : 500);
    } else {
        RPU_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
        RPU_SetLampState(LAMP_HEAD_SAME_PLAYER_SHOOTS_AGAIN, SamePlayerShootsAgain);
    }
}

void ShowPlayfieldXLamps() {
    if (IsSuperSuperBlastOffActive(CurrentTime))
    {
        return;
    }
    if (SkillShotActive || CurrentTime <= SkillShotGracePeroidEnd) {
        // Don't fight the animation
        return;
    }
    if (PlayfieldMultiplier[CurrentPlayer] <= 1) {
        RPU_SetLampState(LAMP_BONUS_2X, 0, 0, 0);
        RPU_SetLampState(LAMP_BONUS_3X, 0, 0, 0);
        RPU_SetLampState(LAMP_BONUS_5X, 0, 0, 0);
    }
    if (PlayfieldMultiplier[CurrentPlayer] == 2) {
        RPU_SetLampState(LAMP_BONUS_2X, 1, 0, 0);
        RPU_SetLampState(LAMP_BONUS_3X, 0, 0, 0);
        RPU_SetLampState(LAMP_BONUS_5X, 0, 0, 0);
    }
    if (PlayfieldMultiplier[CurrentPlayer] == 3) {
        RPU_SetLampState(LAMP_BONUS_3X, 1, 0, 0);
        RPU_SetLampState(LAMP_BONUS_2X, 0, 0, 0);
        RPU_SetLampState(LAMP_BONUS_5X, 0, 0, 0);
    }
    if (PlayfieldMultiplier[CurrentPlayer] == 5) {
        RPU_SetLampState(LAMP_BONUS_5X, 1, 0, 0);
        RPU_SetLampState(LAMP_BONUS_2X, 0, 0, 0);
        RPU_SetLampState(LAMP_BONUS_3X, 0, 0, 0);
    }
}

void ShowSpaceProgressLamps() {
    if (IsSuperSuperBlastOffActive(CurrentTime))
    {
        return;
    }
    if (PlayerGoalProgress[CurrentPlayer].S_Complete == true) {
        RPU_SetLampState(LAMP_LOWER_S, 1, 0, 0);
    } else {
        RPU_SetLampState(LAMP_LOWER_S, 0, 0, 0);
    }
    if (PlayerGoalProgress[CurrentPlayer].P_Complete == true) {
        RPU_SetLampState(LAMP_LOWER_P, 1, 0, 0);
    } else {
        RPU_SetLampState(LAMP_LOWER_P, 0, 0, 0);
    }
    if (PlayerGoalProgress[CurrentPlayer].A_Complete == true) {
        RPU_SetLampState(LAMP_LOWER_A, 1, 0, 0);
    } else {
        RPU_SetLampState(LAMP_LOWER_A, 0, 0, 0);
    }
    if (PlayerGoalProgress[CurrentPlayer].C_Complete == true) {
        RPU_SetLampState(LAMP_LOWER_C, 1, 0, 0);
    } else {
        RPU_SetLampState(LAMP_LOWER_C, 0, 0, 0);
    }
    if (PlayerGoalProgress[CurrentPlayer].E_Complete == true) {
        RPU_SetLampState(LAMP_LOWER_E, 1, 0, 0);
    } else {
        RPU_SetLampState(LAMP_LOWER_E, 0, 0, 0);
    }
}

void ShowPopBumperLamps() {
    if (IsSuperPopsActive(CurrentTime) || WizardModeActive) {
        return;
    }
    if (SkillShotActive || CurrentTime <= SkillShotGracePeroidEnd) {
        // Don't fight the animation
        return;
    }
    RPU_SetLampState(LAMP_C_POP, 1, 0, 0);
    RPU_SetLampState(LAMP_LR_POP, 1, 0, 0);
}

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
#define DISPLAY_OVERRIDE_ANIMATION_NONE 0
#define DISPLAY_OVERRIDE_ANIMATION_BOUNCE 1
#define DISPLAY_OVERRIDE_ANIMATION_FLUTTER 2
#define DISPLAY_OVERRIDE_ANIMATION_FLYBY 3
#define DISPLAY_OVERRIDE_ANIMATION_CENTER 4
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
            // RPU_SetDisplayBlank(displayNum, 0x00);
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
            // RPU_SetDisplayBlank(displayNum, 0x00);
            RPU_SetDisplay(displayNum, displayScore, false);
            RPU_SetDisplayBlank(displayNum, displayMask);
        }
    } else {
        RPU_SetDisplay(displayNum, displayScore, true, 1);
    }
}

// ShowPlayerScores
// This function is used to show the player scores on the displays.
// Inputs: Display to update (0-3 for a specific display, 0xFF to show all)
//         flashCurrent - if true, the current player's score will flash
//         dashCurrent - if true, the current player's score will dash
//         allScoresShowValue - if non-zero, this value will be shown on all displays
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
            // No override, update scores designated by displayToUpdate
            if (allScoresShowValue == 0) {
                displayScore = CurrentScores[scoreCount];
            } else
                displayScore = allScoresShowValue;

            // If we're updating all displays, or the one currently matching the loop, or if we have to scroll
            if (displayToUpdate == 0xFF || displayToUpdate == scoreCount || displayScore > RPU_OS_MAX_DISPLAY_SCORE) {

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
                            if (((CurrentTime / 250) % 2) == 0)
                                RPU_SetDisplayBlank(scoreCount, 0x00);
                            else
                                RPU_SetDisplay(scoreCount, displayScore, true, 2);
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
        // CurrentScores[CurrentPlayer] += ScoreAdditionAnimation;
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

    if (!FreePlayMode) {
        Credits -= 1;
        RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
        RPU_SetDisplayCredits(Credits, !FreePlayMode);
        RPU_SetCoinLockout(false);
    }
    if (CurrentNumPlayers == 1) Audio.StopAllAudio();
    QueueNotification(SOUND_EFFECT_GAME_START, 9);

    RPU_WriteULToEEProm(RPU_TOTAL_PLAYS_EEPROM_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

    return true;
}

unsigned short ChuteAuditByte[] = {RPU_CHUTE_1_COINS_START_BYTE, RPU_CHUTE_2_COINS_START_BYTE, RPU_CHUTE_3_COINS_START_BYTE};
void AddCoinToAudit(byte chuteNum) {
    if (chuteNum > 2) return;
    unsigned short coinAuditStartByte = ChuteAuditByte[chuteNum];
    RPU_WriteULToEEProm(coinAuditStartByte, RPU_ReadULFromEEProm(coinAuditStartByte) + 1);
}

void AddCredit(boolean playSound = false, byte numToAdd = 1) {
    if (Credits < MaximumCredits) {
        Credits += numToAdd;
        if (Credits > MaximumCredits) Credits = MaximumCredits;
        RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
        if (playSound) {
            // PlaySoundEffect(SOUND_EFFECT_ADD_CREDIT);
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
    if (switchHit == SW_COIN_2)
        chuteNum = 1;
    else if (switchHit == SW_COIN_3)
        chuteNum = 2;
    return chuteNum;
}

boolean AddCoin(byte chuteNum) {
    boolean creditAdded = false;
    if (chuteNum > 2) return false;
    byte cpcSelection = GetCPCSelection(chuteNum);

    // Find the lowest chute num with the same ratio selection
    // and use that ChuteCoinsInProgress counter
    byte chuteNumToUse;
    for (chuteNumToUse = 0; chuteNumToUse <= chuteNum; chuteNumToUse++) {
        if (GetCPCSelection(chuteNumToUse) == cpcSelection) break;
    }

    // PlaySoundEffect(SOUND_EFFECT_COIN_DROP_1+(CurrentTime%3));

    byte cpcCoins = GetCPCCoins(cpcSelection);
    byte cpcCredits = GetCPCCredits(cpcSelection);
    byte coinProgressBefore = ChuteCoinsInProgress[chuteNumToUse];
    ChuteCoinsInProgress[chuteNumToUse] += 1;

    if (ChuteCoinsInProgress[chuteNumToUse] == cpcCoins) {
        if (cpcCredits > cpcCoins)
            AddCredit(cpcCredits - (coinProgressBefore));
        else
            AddCredit(cpcCredits);
        ChuteCoinsInProgress[chuteNumToUse] = 0;
        creditAdded = true;
    } else {
        if (cpcCredits > cpcCoins) {
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
        CurrentScores[CurrentPlayer] += SpecialValue * PlayfieldMultiplier[CurrentPlayer];
    } else {
        AwardExtraBall();
    }
}

boolean AwardExtraBall() {
    if (ExtraBallCollected) return false;
        ExtraBallCollected = true;
    if (TournamentScoring) {
        CurrentScores[CurrentPlayer] += ExtraBallValue * PlayfieldMultiplier[CurrentPlayer];
    } else {
        SamePlayerShootsAgain = true;
        RPU_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
        RPU_SetLampState(LAMP_HEAD_SAME_PLAYER_SHOOTS_AGAIN, SamePlayerShootsAgain);
        QueueNotification(SOUND_EFFECT_EXTRABALL, 9);
    }
    return true;
}

boolean AwardGoalExtraBall() {
    if (GoalExtraBallCollected) return false;
        GoalExtraBallCollected = true;
    if (TournamentScoring) {
        CurrentScores[CurrentPlayer] += ExtraBallValue * PlayfieldMultiplier[CurrentPlayer];
    } else {
        SamePlayerShootsAgain = true;
        RPU_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
        RPU_SetLampState(LAMP_HEAD_SAME_PLAYER_SHOOTS_AGAIN, SamePlayerShootsAgain);
        QueueNotification(SOUND_EFFECT_EXTRABALL, 9);
    }
    return true;
}

void SpinnerToggle() {
    if (CenterSpinnerStatus == CENTER_LEFT_SPINNER_LIT) {
        RPU_SetLampState(LAMP_CR_WHENLIT, 1);
        RPU_SetLampState(LAMP_CL_WHENLIT, 0);
        CenterSpinnerStatus = CENTER_RIGHT_SPINNER_LIT;
    } else {
        RPU_SetLampState(LAMP_CR_WHENLIT, 0);
        RPU_SetLampState(LAMP_CL_WHENLIT, 1);
        CenterSpinnerStatus = CENTER_LEFT_SPINNER_LIT;
    }
}

void SpaceToggle() {
    if (SpaceStatus == SPACE_LIT) {
        RPU_SetLampState(LAMP_TOP_S, 1);
        RPU_SetLampState(LAMP_TOP_P, 1);
        RPU_SetLampState(LAMP_TOP_A, 1);
        RPU_SetLampState(LAMP_TOP_C, 1);
        RPU_SetLampState(LAMP_TOP_E, 1);
        SpaceStatus = SPACE_UNLIT;
    } else {
        RPU_SetLampState(LAMP_TOP_S, 0);
        RPU_SetLampState(LAMP_TOP_P, 0);
        RPU_SetLampState(LAMP_TOP_A, 0);
        RPU_SetLampState(LAMP_TOP_C, 0);
        RPU_SetLampState(LAMP_TOP_E, 0);
        SpaceStatus = SPACE_LIT;
    }
}

void TargetBank() {
    if (RPU_ReadLampState(LAMP_TARGET_1) && RPU_ReadLampState(LAMP_TARGET_2) && RPU_ReadLampState(LAMP_TARGET_3) && RPU_ReadLampState(LAMP_TARGET_4)
        && RPU_ReadLampState(LAMP_TARGET_5)) {
            RPU_SetLampState(LAMP_TARGET_1, 0, 0, 0);
            RPU_SetLampState(LAMP_TARGET_2, 0, 0, 0);
            RPU_SetLampState(LAMP_TARGET_3, 0, 0, 0);
            RPU_SetLampState(LAMP_TARGET_4, 0, 0, 0);
            RPU_SetLampState(LAMP_TARGET_5, 0, 0, 0);
        }
    if ((TargetBankComplete[CurrentPlayer] == 2) && !(RPU_ReadLampState(LAMP_EXTRABALL))) {
            RPU_SetLampState(LAMP_EXTRABALL, 1, 0, 0);
            RPU_SetLampState(LAMP_TARGET_1, 1, 0, 500);
            RPU_SetLampState(LAMP_TARGET_2, 1, 0, 500);
            RPU_SetLampState(LAMP_TARGET_3, 1, 0, 500);
            RPU_SetLampState(LAMP_TARGET_4, 1, 0, 500);
            RPU_SetLampState(LAMP_TARGET_5, 1, 0, 500);
            QueueNotification(SOUND_EFFECT_EXTRABALL_LIT, 9);
            TargetBankComplete[CurrentPlayer] += 1;
    }
    if  (TargetBankComplete[CurrentPlayer] >= 3) {
            RPU_SetLampState(LAMP_TARGET_1, 1, 0, 500);
            RPU_SetLampState(LAMP_TARGET_2, 1, 0, 500);
            RPU_SetLampState(LAMP_TARGET_3, 1, 0, 500);
            RPU_SetLampState(LAMP_TARGET_4, 1, 0, 500);
            RPU_SetLampState(LAMP_TARGET_5, 1, 0, 500);
    }
}

void IncreasePlayfieldMultiplier() {
    if (PlayfieldMultiplier[CurrentPlayer] <= 5) {
        PlayfieldMultiplier[CurrentPlayer] += 1;
    }
    if (PlayfieldMultiplier[CurrentPlayer] == 4) {
        PlayfieldMultiplier[CurrentPlayer] += 1;
    }
    if (PlayfieldMultiplier[CurrentPlayer] == 2) {
        QueueNotification(SOUND_EFFECT_MULTI_2X, 9);
    } else if (PlayfieldMultiplier[CurrentPlayer] == 3) {
        QueueNotification(SOUND_EFFECT_MULTI_3X, 9);
    } else if (PlayfieldMultiplier[CurrentPlayer] == 5) {
        PlayerGoalProgress[CurrentPlayer].E_Complete = true;
        if (CountGoalsCompleted(CurrentPlayer) >= 5){
            QueueNotification(SOUND_EFFECT_PLAYFIELDX_WIZARD, 9);
        } else if (CountGoalsCompleted(CurrentPlayer) == 3){
            QueueNotification(SOUND_EFFECT_PLAYFIELDX_EXTRABALL, 9);
        } else {
            QueueNotification(SOUND_EFFECT_MULTI_GOAL, 9);
        }
    } else if (PlayfieldMultiplier[CurrentPlayer] > 5) {
        // Should not currently be able to happen
        PlaySoundEffect(SOUND_EFFECT_DROPTARGET);
    }
}

#define ADJ_TYPE_LIST 1
#define ADJ_TYPE_MIN_MAX 2
#define ADJ_TYPE_MIN_MAX_DEFAULT 3
#define ADJ_TYPE_SCORE 4
#define ADJ_TYPE_SCORE_WITH_DEFAULT 5
#define ADJ_TYPE_SCORE_NO_DEFAULT 6
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
        if (SoundSettingTimeout && CurrentTime > SoundSettingTimeout) {
            SoundSettingTimeout = 0;
            Audio.StopAllAudio();
        }
    }

    // Any state that's greater than MACHINE_STATE_TEST_DONE is handled by the Base Self-test code
    // Any that's less, is machine specific, so we handle it here.
    if (curState >= MACHINE_STATE_TEST_DONE) {
        byte cpcSelection = 0xFF;
        byte chuteNum = 0xFF;
        if (curState == MACHINE_STATE_ADJUST_CPC_CHUTE_1) chuteNum = 0;
        if (curState == MACHINE_STATE_ADJUST_CPC_CHUTE_2) chuteNum = 1;
        if (curState == MACHINE_STATE_ADJUST_CPC_CHUTE_3) chuteNum = 2;
        if (chuteNum != 0xFF) cpcSelection = GetCPCSelection(chuteNum);
        returnState = RunBaseSelfTest(returnState, curStateChanged, CurrentTime, SW_CREDIT_RESET, SW_SLAM);
        if (chuteNum != 0xFF) {
            if (cpcSelection != GetCPCSelection(chuteNum)) {
                byte newCPC = GetCPCSelection(chuteNum);
                Audio.StopAllAudio();
                Audio.PlaySound(SOUND_EFFECT_SELF_TEST_CPC_START + newCPC, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
            }
        }
    } else {
        byte curSwitch = RPU_PullFirstFromSwitchStack();

        if (curSwitch == SW_SELF_TEST_SWITCH && (CurrentTime - GetLastSelfTestChangedTime()) > 250) {
            SetLastSelfTestChangedTime(CurrentTime);
            if (RPU_GetUpDownSwitchState())
                returnState -= 1;
            else
                returnState += 1;
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
#if (RPU_MPU_ARCHITECTURE < 10)
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
            case MACHINE_STATE_ADJUST_WIZARD_DIFFICULTY:
                CurrentAdjustmentByte = (byte *)&WizardHardMode;
                CurrentAdjustmentStorageByte = EEPROM_WIZARD_HARD_MODE_BYTE;
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
                        if (AdjustmentType == ADJ_TYPE_MIN_MAX)
                            curVal = AdjustmentValues[0];
                        else {
                            if (curVal > 99)
                                curVal = AdjustmentValues[0];
                            else
                                curVal = 99;
                        }
                    }
                } else {
                    if (curVal == AdjustmentValues[0]) {
                        if (AdjustmentType == ADJ_TYPE_MIN_MAX_DEFAULT)
                            curVal = 99;
                        else
                            curVal = AdjustmentValues[1];
                    } else {
                        curVal -= 1;
                    }
                }

                *CurrentAdjustmentByte = curVal;
                if (CurrentAdjustmentStorageByte) EEPROM.write(CurrentAdjustmentStorageByte, curVal);

                if (curState == MACHINE_STATE_ADJUST_SOUND_SELECTOR) {
                    Audio.StopAllAudio();
                    Audio.PlaySound(SOUND_EFFECT_SELF_TEST_AUDIO_OPTIONS_START + curVal, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
                } else if (curState == MACHINE_STATE_ADJUST_MUSIC_VOLUME) {
                    if (SoundSettingTimeout) Audio.StopAllAudio();
                    Audio.PlaySound(SOUND_EFFECT_BACKGROUND1, AUDIO_PLAY_TYPE_WAV_TRIGGER, curVal);
                    Audio.SetMusicVolume(curVal);
                    SoundSettingTimeout = CurrentTime + 5000;
                } else if (curState == MACHINE_STATE_ADJUST_SFX_VOLUME) {
                    if (SoundSettingTimeout) Audio.StopAllAudio();
                    Audio.PlaySound(SOUND_EFFECT_BONUS_COUNT_1k, AUDIO_PLAY_TYPE_WAV_TRIGGER, curVal);
                    Audio.SetSoundFXVolume(curVal);
                    SoundSettingTimeout = CurrentTime + 5000;
                } else if (curState == MACHINE_STATE_ADJUST_CALLOUTS_VOLUME) {
                    if (SoundSettingTimeout) Audio.StopAllAudio();
                    Audio.PlaySound(SOUND_EFFECT_EXTRABALL, AUDIO_PLAY_TYPE_WAV_TRIGGER, curVal);
                    Audio.SetNotificationsVolume(curVal);
                    SoundSettingTimeout = CurrentTime + 3000;
                }

            } else if (CurrentAdjustmentByte && AdjustmentType == ADJ_TYPE_LIST) {
                byte valCount = 0;
                byte curVal = *CurrentAdjustmentByte;
                byte newIndex = 0;
                boolean upDownState = RPU_GetUpDownSwitchState();
                for (valCount = 0; valCount < (NumAdjustmentValues); valCount++) {
                    if (curVal == AdjustmentValues[valCount]) {
                        if (upDownState) {
                            if (valCount < (NumAdjustmentValues - 1)) newIndex = valCount + 1;
                        } else {
                            if (valCount > 0) newIndex = valCount - 1;
                        }
                    }
                }
                *CurrentAdjustmentByte = AdjustmentValues[newIndex];
                if (CurrentAdjustmentStorageByte) EEPROM.write(CurrentAdjustmentStorageByte, AdjustmentValues[newIndex]);
            } else if (CurrentAdjustmentUL && (AdjustmentType == ADJ_TYPE_SCORE_WITH_DEFAULT || AdjustmentType == ADJ_TYPE_SCORE_NO_DEFAULT)) {
                unsigned long curVal = *CurrentAdjustmentUL;

                if (RPU_GetUpDownSwitchState())
                    curVal += 5000;
                else if (curVal >= 5000)
                    curVal -= 5000;
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
        // DecodeDIPSwitchParameters();
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

    if (MusicVolume == 0) return;

    Audio.PlayBackgroundSong(songNum);
}

unsigned long NextSoundEffectTime = 0;

void PlaySoundEffect(unsigned int soundEffectNum) {

    if (MachineState == MACHINE_STATE_INIT_GAMEPLAY) return;

    // Play digital samples on the WAV trigger (numbered same
    // as SOUND_EFFECT_ defines)
    Audio.PlaySound(soundEffectNum, AUDIO_PLAY_TYPE_WAV_TRIGGER);

    // SOUND_EFFECT_ defines can also be translated into
    // commands for the sound card
    switch (soundEffectNum) {
    }
}

void QueueNotification(unsigned int soundEffectNum, byte priority) {
    if (CalloutsVolume == 0) return;
    if (SoundSelector < 3 || SoundSelector == 4 || SoundSelector == 7 || SoundSelector == 9) return;
    if (soundEffectNum < SOUND_EFFECT_BALL_SAVE || soundEffectNum >= (SOUND_EFFECT_BALL_SAVE + NUM_VOICE_NOTIFICATIONS)) return;
    Audio.QueuePrioritizedNotification(soundEffectNum, 0, priority, CurrentTime);
}

////////////////////////////////////////////////////////////////////////////
//
//  Diagnostics Mode
//
////////////////////////////////////////////////////////////////////////////

int RunDiagnosticsMode(int curState, boolean curStateChanged) {

    int returnState = curState;

    if (curStateChanged) {
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
byte InAttractMode = true;
unsigned long ShowLampTimeOffset = 0;
unsigned long AttractModeStartTime = 0;

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
        }

        // If this machine has a saucer, clear it in attract mode

        if (RPU_ReadSingleSwitchState(SW_C_SAUCER)) {
            RPU_PushToSolenoidStack(SOL_C_SAUCER, 16, true);
        }
        if (RPU_ReadSingleSwitchState(SW_R_SAUCER)) {
            RPU_PushToSolenoidStack(SOL_R_SAUCER, 16, true);
        }
    }

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
        // Not sure why this used to turn on skill shot, leaving it commented for now - TJS
        //if (attractPlayfieldPhase == 2)
        //    GameMode = GAME_MODE_SKILL_SHOT;
        //else
        //    GameMode = GAME_MODE_UNSTRUCTURED_PLAY;
        AttractLastLadderBonus = 1;
        AttractLastLadderTime = CurrentTime;
    }

unsigned long animationTime = (CurrentTime - AttractModeStartTime);
    if (animationTime<4600) {
      ShowLampAnimation(0, 96, animationTime, 23, false, false);
    } else if (animationTime<6900) {
      ShowLampAnimation(1, 48, animationTime, 23, false, false);
    } else {
      AttractModeStartTime = CurrentTime;
    }

    byte switchHit;
    while ((switchHit = RPU_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY) {
        if (switchHit == SW_CREDIT_RESET) {
            if (AddPlayer(true)) returnState = MACHINE_STATE_INIT_GAMEPLAY;
        }
        if (switchHit == SW_COIN_1 || switchHit == SW_COIN_2 || switchHit == SW_COIN_3) {
            AddCoinToAudit(SwitchToChuteNum(switchHit));
            AddCoin(SwitchToChuteNum(switchHit));
        }
        if (switchHit == SW_SELF_TEST_SWITCH && (CurrentTime - GetLastSelfTestChangedTime()) > 250) {
#if (RPU_MPU_ARCHITECTURE < 10)
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

byte CountBallsInTrough() {

    byte numBalls = RPU_ReadSingleSwitchState(SW_OUTHOLE);

    return numBalls;
}

void AddToBonus(byte amountToAdd = 1) {
    if (WizardModeActive) {
        return;
    }

    Bonus[CurrentPlayer] += amountToAdd;
    if (Bonus[CurrentPlayer] >= MAX_DISPLAY_BONUS) {
        Bonus[CurrentPlayer] = MAX_DISPLAY_BONUS;
        if (PlayerGoalProgress[CurrentPlayer].C_Complete == false) {
            PlayerGoalProgress[CurrentPlayer].C_Complete = true;
            if (CountGoalsCompleted(CurrentPlayer) >= 5){
                    QueueNotification(SOUND_EFFECT_BONUS_WIZARD, 9);
                } else if (CountGoalsCompleted(CurrentPlayer) == 3){
                    QueueNotification(SOUND_EFFECT_BONUS_EXTRABALL, 9);
                } else {
                    QueueNotification(SOUND_EFFECT_BONUS_ACHIEVED, 9);
                }
            RPU_SetLampState(LAMP_LOWER_C, 1, 0, 0);
        }
    } else {
        BonusChanged = CurrentTime;
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
        if (CurrentTime > (SaucerEjectTime + 1500)) {
            RPU_PushToSolenoidStack(SOL_C_SAUCER, 12, true);
            SaucerEjectTime = CurrentTime;
        }
    }
    if (RPU_ReadSingleSwitchState(SW_R_SAUCER)) {
        if (CurrentTime > (SaucerEjectTime + 2500)) {
            RPU_PushToSolenoidStack(SOL_R_SAUCER, 12, true);
            SaucerEjectTime = CurrentTime;
        }
    }

    if (CountBallsInTrough() == 0) {
        if (CurrentTime > (GameStartNotificationTime + 5000)) {
            GameStartNotificationTime = CurrentTime;
        }
        return MACHINE_STATE_INIT_GAMEPLAY;
    }

    //  MachineLocks = InitializeMachineLocksBasedOnSwitches();
    //  NumberOfBallsLocked = CountBits(MachineLocks & LOCKS_ENGAGED_MASK);

    if (CountBallsInTrough() < (TotalBallsLoaded - NumberOfBallsLocked)) {
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
        NumberOfCenterSpins[count] = 0;
        NumberOfHits[count] = 0;
        Bonus[count] = 0;
        TargetBankComplete[count] = 0;
        PlayerGoalProgress[count].S_Complete = false;
        PlayerGoalProgress[count].P_Complete = false;
        PlayerGoalProgress[count].A_Complete = false;
        PlayerGoalProgress[count].C_Complete = false;
        PlayerGoalProgress[count].E_Complete = false;
        HOLD_SPINNER_PROGRESS[count] = false;
        HOLD_POP_PROGRESS[count] = false;
        HOLD_BLASTOFF_PROGRESS[count] = false;
        HOLD_BONUS[count] = false;
        HOLD_PLAYFIELDX[count] = false;
        PlayfieldMultiplier[count] = 1;
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

void PlayRandomBackgroundSong() {
    if (MusicVolume == 0) return;

    int rand = random()%6+1;
    if (rand == 1) {
        PlayBackgroundSong(SOUND_EFFECT_BACKGROUND1);
    } else if (rand == 2) {
        PlayBackgroundSong(SOUND_EFFECT_BACKGROUND2);
    } else if (rand == 3) {
        PlayBackgroundSong(SOUND_EFFECT_BACKGROUND3);
    } else if (rand == 4) {
        PlayBackgroundSong(SOUND_EFFECT_BACKGROUND4);
    } else if (rand == 5) {
        PlayBackgroundSong(SOUND_EFFECT_BACKGROUND5);
    } else {
        PlayBackgroundSong(SOUND_EFFECT_BACKGROUND6);
    }
}

/* 
* Manage holdover awards on a new ball.
* If ignoreAll is true, clear all holdover awards and don't apply any of them
*/
void NewBallHoldoverAwards(bool ignoreAll = false) {
    if (HOLD_BONUS[CurrentPlayer] == false || ignoreAll) {
        // Bonus was maxed out last ball, so start back at 0
        if (Bonus[CurrentPlayer] >= MAX_DISPLAY_BONUS) {
            Bonus[CurrentPlayer] = 0;
        }

        if (!WizardHardMode) {
            if (Bonus[CurrentPlayer] >= 30) {
                Bonus[CurrentPlayer] = 30;
            } else if (Bonus[CurrentPlayer] >= 20) {
                Bonus[CurrentPlayer] = 20;
            } else if (Bonus[CurrentPlayer] >= 10) {
                Bonus[CurrentPlayer] = 10;
            } else {
                Bonus[CurrentPlayer] = 0;
            }
        } else {
            Bonus[CurrentPlayer] = 0;
        }
    }
    if (HOLD_PLAYFIELDX[CurrentPlayer] == false || ignoreAll) {
        PlayfieldMultiplier[CurrentPlayer] = 1;
    }
    if (HOLD_BLASTOFF_PROGRESS[CurrentPlayer] == false || ignoreAll) {
        NumberOfCenterSpins[CurrentPlayer] = 0;
    }
    if (HOLD_SPINNER_PROGRESS[CurrentPlayer] == false || ignoreAll) {
        NumberOfSpins[CurrentPlayer] = 0;
    }
    if (HOLD_POP_PROGRESS[CurrentPlayer] == false || ignoreAll) {
        NumberOfHits[CurrentPlayer] = 0;
    }

    HOLD_BONUS[CurrentPlayer] = false;
    HOLD_PLAYFIELDX[CurrentPlayer] = false;
    HOLD_BLASTOFF_PROGRESS[CurrentPlayer] = false;
    HOLD_SPINNER_PROGRESS[CurrentPlayer] = false;
    HOLD_POP_PROGRESS[CurrentPlayer] = false;
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
        if (CurrentNumPlayers > 1 && (ballNum != 1 || playerNum != 0) && !SamePlayerShootsAgain && !PreparingWizardMode) {
            PlaySoundEffect(SOUND_EFFECT_RIDER1 + playerNum);
        }
        SamePlayerShootsAgain = false;

        RPU_SetDisplayBallInPlay(CurrentBallInPlay);
        RPU_SetLampState(LAMP_HEAD_TILT, 0);

        if (BallSaveNumSeconds > 0) {
            RPU_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, 500);
            RPU_SetLampState(LAMP_HEAD_SAME_PLAYER_SHOOTS_AGAIN, 1, 0, 500);
        }

        SkillShotCelebrationBlinkEndTime = 0;
        IsAnyModeActive = false;
        BallSaveUsed = false;
        BallTimeInTrough = 0;
        NumTiltWarnings = 0;
        LastTiltWarningTime = 0;
        SpaceStatus = SPACE_LIT;

        // Initialize game-specific start-of-ball lights & variables
        //ExtraBallCollected = false;
        SpecialCollected = false;
        ScoreAdditionAnimation = 0;
        ScoreAdditionAnimationStartTime = 0;
        BonusXAnimationStart = 0;
        //    Bonus[CurrentPlayer] = 0;
        BallSaveEndTime = 0;
        Bonus[CurrentPlayer] = Bonus[CurrentPlayer];
        for (int count = 0; count < 4; count++) {
            //      NumberOfSpins[count] = 1;
            //      NumberOfCenterSpins[count] = 1;
            //      NumberOfHits[count] = 0;
            Goals[count] = 0;
            //TargetBankComplete[count] = 0;
        }
        ResetModes();
        SpinnerToggle();
        TargetBank();

        ShowPlayerScores(0xFF, false, false);

        // Reset gate
        GateOpen = true; // Unpowered gate is open, gate open when true
        GateOpenTime = 0;

        WizardModeProgress = {};

        if (PreparingWizardMode) {
            // Ball was just loaded to start wizard mode
            QueueNotification(SOUND_EFFECT_WIZARD_MODE_INSTRUCT, 9);
            PlayBackgroundSong(SOUND_EFFECT_WIZARD_BG);
            WizardModeActive = true;
            NewBallHoldoverAwards(true);
            DisableBallSaveThisBall = true;
        } else if (WizardModeEnding) {
             // Just came out of wizard mode.
             NewBallHoldoverAwards(true);
             Bonus[CurrentPlayer] = BonusBeforeWizardMode;
             BonusBeforeWizardMode = 0;
             PlayRandomBackgroundSong();
             SkillShotActive = false; // No skill shot after wizard mode
             DisableBallSaveThisBall = true;
        } else {
            // Normal play, not before or after wizard mode
            SkillShotActive = true;
            NewBallHoldoverAwards();
            PlayRandomBackgroundSong();
            DisableBallSaveThisBall = false;
        }

        PreparingWizardMode = false;
        WizardModeEnding = false;

        SkillShotGracePeroidEnd = 0;

        // Reset Drop Targets
        if (!FirstGame) {
            // Only reset the drop targets if this is not the first game, since game start would have already reset them
            //RPU_PushToTimedSolenoidStack(SOL_DROP_TARGET_RESET, 10, CurrentTime, true);
        } else {
            FirstGame = false;
        }

        // Always reset because we don't reset on game start (for now)
        RPU_PushToTimedSolenoidStack(SOL_DROP_TARGET_RESET, 10, CurrentTime, true);

        RPU_PushToTimedSolenoidStack(SOL_OUTHOLE, 16, CurrentTime + 1000);
        NumberOfBallsInPlay = 1;
    }

    // We should only consider the ball initialized when
    // the ball is no longer triggering the SW_OUTHOLE
    if (CountBallsInTrough() == (TotalBallsLoaded - NumberOfBallsLocked)) {
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



byte GameModeStage;
boolean DisplaysNeedRefreshing = false;
unsigned long LastTimePromptPlayed = 0;
unsigned short CurrentBattleLetterPosition = 0xFF;

// This function manages all timers, flags, and lights
int ManageGameMode() {
    int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

    boolean specialAnimationRunning = false;

    // Determine which spinner lights should be on
    ShowLeftSpinnerLamps();
    // Should BlastOff lights be on?
    ShowCenterSpinnerLamps();
    // Determine which PlayfieldX lights should be on
    ShowPlayfieldXLamps();
    ShowPopBumperLamps();
    // Show which goals have been achieved
    ShowSpaceProgressLamps();

    if (WizardModeActive) {
        ShowLampAnimation(6, 48, CurrentTime, 23, false, false);
        ManageWizardMode();
    }

    if (SkillShotActive == true && BallFirstSwitchHitTime != 0) {
        // The switch handler will award the skill shot
        // (when applicable) and this mode will move
        // to unstructured play when any valid switch is
        // recorded
        SetGeneralIlluminationOn(true);
        SkillShotActive = false;
        SkillShotGracePeroidEnd = CurrentTime + SkillShotGracePeriodMs;
    } else if ((SkillShotActive || CurrentTime <= SkillShotGracePeroidEnd) && CurrentTime >= SkillShotCelebrationBlinkEndTime) {
        ShowLampAnimation(3, 960, CurrentTime, 23, false, false);

        // Show a countdown if within the grace period
        if (!SkillShotActive) {
            unsigned long SkillShotTimeLeft = (SkillShotActive) ? 30 : (SkillShotGracePeroidEnd - CurrentTime) / 1000;
            byte displayToUse = (CurrentPlayer == 3) ? 0 : CurrentPlayer + 1; // Use the next available display
            ShowPlayerScores(displayToUse, false, false);
            OverrideScoreDisplay(displayToUse, SkillShotTimeLeft, DISPLAY_OVERRIDE_ANIMATION_BOUNCE);
        }
    }

    if (IsSuperSpinnerActive(CurrentTime)) {
        unsigned long SuperSpinnerTimeLeft = SuperSpinnerRemainingTime(CurrentTime);
        
        byte displayToUse = (CurrentPlayer == 0) ? 1 : 0; // Show spinner time on first available display
        OverrideScoreDisplay(displayToUse, SuperSpinnerTimeLeft / 1000, DISPLAY_OVERRIDE_ANIMATION_BOUNCE);

        IsAnyModeActive = true;
    }

    if (IsSuperPopsActive(CurrentTime)) {
        ShowLampAnimation(5, 24, CurrentTime, 23, false, false);
        unsigned long SuperPopTimeLeft = SuperPopsRemainingTime(CurrentTime);

        byte displayToUse = (CurrentPlayer == 0 || CurrentPlayer == 1) ? 2 : 1; // Show spinner time on first available display
        OverrideScoreDisplay(displayToUse, SuperPopTimeLeft / 1000, DISPLAY_OVERRIDE_ANIMATION_BOUNCE);

        IsAnyModeActive = true;
    } else if (!WizardModeActive) {
        //ShowPopBumperLamps();
    }
    
    if (IsSuperSuperBlastOffActive(CurrentTime)){
        
        unsigned long SuperBlastOffTimeLeft = SuperBlastOffRemainingTime(CurrentTime);

        byte displayToUse = (CurrentPlayer == 3) ? 2 : 3; // Show spinner time on first available display
        OverrideScoreDisplay(displayToUse, SuperBlastOffTimeLeft / 1000, DISPLAY_OVERRIDE_ANIMATION_BOUNCE);

        IsAnyModeActive = true;
    }

    if (!IsSuperSpinnerActive(CurrentTime) && 
        !IsSuperPopsActive(CurrentTime) && 
        !IsSuperSuperBlastOffActive(CurrentTime) && 
        !SkillShotActive && 
        CurrentTime > SkillShotGracePeroidEnd) {
        
            ShowPlayerScores(0xFF, false, false);

        if (IsAnyModeActive == true) {
            // Some hurry up mode was active but now it's over
            IsAnyModeActive = false;
            PlayRandomBackgroundSong();
            RPU_SetDisplayCredits(Credits);
            RPU_SetDisplayBallInPlay(CurrentBallInPlay);
        }
    }

    // If skill shot was hit and we're done blinking, or if the skill shot grace period ended,
    // switch to the normal gameplay SPACE letter togging.
    if ((!SkillShotActive && SkillShotCelebrationBlinkEndTime != 0 && CurrentTime > SkillShotCelebrationBlinkEndTime) ||
         (!SkillShotActive  && SkillShotGracePeroidEnd != 0 && CurrentTime > SkillShotGracePeroidEnd)) {

        SpaceToggle(); // Start the toggle cycle since those lights are no longer needed for Skill Shot
        SkillShotCelebrationBlinkEndTime = 0; // Reset this to 0 so we don't contantly turn off the SPACE lamps, let them toggle
        SkillShotGracePeroidEnd = 0;
    }

    if ((GateOpenTime != 0) && ((CurrentTime - GateOpenTime) > 1000)) {
        RPU_SetDisableGate(false);
        GateOpenTime = 0;
    }

    if ((BallFirstSwitchHitTime == 0) && CountGoalsCompleted(CurrentPlayer)) { // If ball not in play and if any goals have been reached
        for (byte count = 0; count < 4; count++) {
            if (count != CurrentPlayer) {
                OverrideScoreDisplay(count, CountGoalsCompleted(CurrentPlayer), false); // Show achieved goals
            }
        }
        GoalsDisplayToggle = true;
    } else if ((BallFirstSwitchHitTime > 0) && GoalsDisplayToggle) {
        ShowPlayerScores(0xFF, false, false); //  Reset all score displays
        GoalsDisplayToggle = false;
    }

    if (!specialAnimationRunning && NumTiltWarnings <= MaxTiltWarnings) {
        //    ShowTopSpaceLamps();
        ShowBonusLamps();

        if (!WizardModeActive && !WizardModeEnding) {
            ShowShootAgainLamps();
        }
            //ShowPopBumperLamps();
    }

    // If the player has completed three goals, Light Extra Ball at Right Target
    if (CountGoalsCompleted(CurrentPlayer) == 3 && !GoalExtraBallCollected && !RPU_ReadLampState(LAMP_EXTRABALL)) {
        RPU_SetLampState(LAMP_EXTRABALL, 1, 0, 0);
        //QueueNotification(SOUND_EFFECT_EXTRABALL_LIT, 8);
    }
    else if (CountGoalsCompleted(CurrentPlayer) >= 5 && !PreparingWizardMode)
    {
        // Kill the flippers and lights to let the ball drain and start wizard mode
        PreparingWizardMode = true;
        BallSaveEndTime = 0;
        RPU_TurnOffAllLamps();
        RPU_SetDisableFlippers(true);
        RPU_SetDisableGate(true);
        RPU_DisableSolenoidStack();
        ResetModes();

        // If bonus was maxed this ball, reset it so they don't get a free letter after wizard.
        if (Bonus[CurrentPlayer] >= 40) {
            Bonus[CurrentPlayer] = 0;
        }
        BonusBeforeWizardMode = Bonus[CurrentPlayer]; // Bonus will get cleared on the new ball, so save it off

        //QueueNotification(SOUND_EFFECT_WIZARD_MODE_START, 9);
        PlayerGoalProgress[CurrentPlayer].S_Complete = false;
        PlayerGoalProgress[CurrentPlayer].P_Complete = false;
        PlayerGoalProgress[CurrentPlayer].A_Complete = false;
        PlayerGoalProgress[CurrentPlayer].C_Complete = false;
        PlayerGoalProgress[CurrentPlayer].E_Complete = false;
        
        // Handle the edge case where the saucer hit was the last thing needed to enter wizard - 
        // We just cleared the solenoid stack so need to re-trigger the kickout
        if (RPU_ReadSingleSwitchState(SW_C_SAUCER)) {
            RPU_PushToTimedSolenoidStack(SOL_C_SAUCER, 16, CurrentTime + 3000, true);
        }
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
                if (count == countdownDisplay)
                    OverrideScoreDisplay(count, scoreToShow, DISPLAY_OVERRIDE_ANIMATION_NONE);
                else if (count != CurrentPlayer)
                    OverrideScoreDisplay(count, DISPLAY_OVERRIDE_BLANK_SCORE, DISPLAY_OVERRIDE_ANIMATION_NONE);
            }
        } else {
            byte countdownDisplay = (1 + CurrentPlayer) % 4;
            unsigned long remainingScore = 0;
            if ((CurrentTime - ScoreAdditionAnimationStartTime) < 5000) {
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
                if (count == countdownDisplay)
                    OverrideScoreDisplay(count, ScoreAdditionAnimation - remainingScore, DISPLAY_OVERRIDE_ANIMATION_NONE);
                else if (count != CurrentPlayer)
                    OverrideScoreDisplay(count, DISPLAY_OVERRIDE_BLANK_SCORE, DISPLAY_OVERRIDE_ANIMATION_NONE);
                else
                    OverrideScoreDisplay(count, CurrentScores[CurrentPlayer] + remainingScore, DISPLAY_OVERRIDE_ANIMATION_NONE);
            }
        }
        if (ScoreAdditionAnimationStartTime) {
            ShowPlayerScores(CurrentPlayer, false, false);
        } else {
            ShowPlayerScores(0xFF, false, false);
        }
    } else {
        ShowPlayerScores(CurrentPlayer, (BallFirstSwitchHitTime == 0) ? true : false, (BallFirstSwitchHitTime > 0 && ((CurrentTime - LastTimeScoreChanged) > 2000)) ? true : false);
    }

    // Check to see if ball is in the outhole
    if (CountBallsInTrough() > (TotalBallsLoaded - (NumberOfBallsInPlay + NumberOfBallsLocked))) {

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
                    if (BallSaveEndTime && CurrentTime < (BallSaveEndTime + BALL_SAVE_GRACE_PERIOD)) {
                        RPU_PushToTimedSolenoidStack(SOL_OUTHOLE, 16, CurrentTime + 100);
                        QueueNotification(SOUND_EFFECT_BALL_SAVE, 9);
                        RPU_SetLampState(LAMP_SHOOT_AGAIN, 0);
                        BallTimeInTrough = CurrentTime;
                        returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

                        // Only 1 ball save if one ball in play
                        if (NumberOfBallsInPlay == 1) {
                            BallSaveEndTime = CurrentTime + 1000;
                        } else {
                            if (CurrentTime > BallSaveEndTime) BallSaveEndTime += 1000;
                        }

                    } else {

                        NumberOfBallsInPlay -= 1;
                        if (NumberOfBallsInPlay == 0) {
                            ShowPlayerScores(0xFF, false, false);
                            Audio.StopAllAudio();

                            if (!PreparingWizardMode && !WizardModeActive && !WizardModeEnding) {
                                returnState = MACHINE_STATE_COUNTDOWN_BONUS;
                            } else {
                                returnState = MACHINE_STATE_BALL_OVER;
                            }
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

void ManageWizardMode() {
    // Pops lights
    if (WizardModeProgress.TopPopsHit) {
        RPU_SetLampState(LAMP_LR_POP, 0, 0, 0);
    } else {
        RPU_SetLampState(LAMP_LR_POP, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
    }
    if (WizardModeProgress.BottomPopHit) {
        RPU_SetLampState(LAMP_C_POP, 0, 0, 0);
    } else {
        RPU_SetLampState(LAMP_C_POP, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
    }

    // Center Saucer Lights
    if (WizardModeProgress.CenterSaucerHit) {
        RPU_SetLampState(LAMP_TOP_S, 0, 0, 0);
        RPU_SetLampState(LAMP_TOP_P, 0, 0, 0);
        RPU_SetLampState(LAMP_TOP_A, 0, 0, 0);
        RPU_SetLampState(LAMP_TOP_C, 0, 0, 0);
        RPU_SetLampState(LAMP_TOP_E, 0, 0, 0);
    } else {
        RPU_SetLampState(LAMP_TOP_S, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
        RPU_SetLampState(LAMP_TOP_P, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
        RPU_SetLampState(LAMP_TOP_A, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
        RPU_SetLampState(LAMP_TOP_C, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
        RPU_SetLampState(LAMP_TOP_E, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
    }
    
    // L Spinner lights are managed in a separate function

    // C Spinner lights are managed in a separate function

    // Left target bank
    if (WizardModeProgress.Target1Hit) {
        RPU_SetLampState(LAMP_TARGET_1, 0, 0, 0);
    } else {
        RPU_SetLampState(LAMP_TARGET_1, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
    }
    if (WizardModeProgress.Target2Hit) {
        RPU_SetLampState(LAMP_TARGET_2, 0, 0, 0);
    } else {
        RPU_SetLampState(LAMP_TARGET_2, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
    }
    if (WizardModeProgress.Target3Hit) {
        RPU_SetLampState(LAMP_TARGET_3, 0, 0, 0);
    } else {
        RPU_SetLampState(LAMP_TARGET_3, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
    }
    if (WizardModeProgress.Target4Hit) {
        RPU_SetLampState(LAMP_TARGET_4, 0, 0, 0);
    } else {
        RPU_SetLampState(LAMP_TARGET_4, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
    }
    if (WizardModeProgress.Target5Hit) {
        RPU_SetLampState(LAMP_TARGET_5, 0, 0, 0);
    } else {
        RPU_SetLampState(LAMP_TARGET_5, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
    }

    // Inline targets
    if (WizardModeProgress.InlineTargetsCompleted) {
        RPU_SetLampState(LAMP_DROP_SPECIAL, 0, 0, 0);
    } else {
        RPU_SetLampState(LAMP_DROP_SPECIAL, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
    }

    // Right target
    if (WizardModeProgress.RightTargetHit) {
        RPU_SetLampState(LAMP_TARGET_SPECIAL, 0, 0, 0);
    } else {
        RPU_SetLampState(LAMP_TARGET_SPECIAL, 1, 0, WIZARD_MODE_GOAL_BLINK_PERIOD_MS);
    }
}

bool AreWizardModeGoalsCompleted() {
    if ((WizardModeProgress.BottomPopHit == true) &&
        (WizardModeProgress.TopPopsHit == true) &&
        (WizardModeProgress.CenterSaucerHit == true) &&
        (WizardModeProgress.CenterSpinnerSpins >= WIZARD_MODE_CENTER_SPINS_REQUIRED) &&
        (WizardModeProgress.InlineTargetsCompleted == true) &&
        (WizardModeProgress.LeftSpinnerSpins >= WIZARD_MODE_LEFT_SPINS_REQUIRED) &&
        (WizardModeProgress.RightTargetHit) && 
        (WizardModeProgress.Target1Hit) && 
        (WizardModeProgress.Target2Hit) && 
        (WizardModeProgress.Target3Hit) && 
        (WizardModeProgress.Target4Hit) && 
        (WizardModeProgress.Target5Hit)) {
        return true;
    } else {
        return false;
    }
}

int CountDownDelayTimes[] = {175, 130, 105, 90, 80, 70, 60, 40, 30, 20};

unsigned long CountdownStartTime = 0;
unsigned long LastCountdownReportTime = 0;
unsigned long BonusCountDownEndTime = 0;
byte DecrementingBonusCounter;
byte IncrementingBonusXCounter;
byte TotalBonus = 0;
byte TotalBonusX = 0;
boolean CountdownBonusHurryUp = false;

int CountdownBonus(boolean curStateChanged) {
    // If this is the first time through the countdown loop
    if (curStateChanged) {

        CountdownStartTime = CurrentTime;
        LastCountdownReportTime = CurrentTime;
        ShowBonusLamps();
        IncrementingBonusXCounter = 1;
        DecrementingBonusCounter = Bonus[CurrentPlayer];
        TotalBonus = Bonus[CurrentPlayer];
        TotalBonusX = BonusX[CurrentPlayer];
        CountdownBonusHurryUp = false;

        BonusCountDownEndTime = 0xFFFFFFFF;
        // Some sound cards have a special index
        // for a "sound" that will turn off
        // the current background drone or currently
        // playing sound
        //    PlaySoundEffect(SOUND_EFFECT_STOP_BACKGROUND);
    }

    unsigned long countdownDelayTime = (unsigned long)(CountDownDelayTimes[IncrementingBonusXCounter - 1]);
    if (CountdownBonusHurryUp && countdownDelayTime > ((unsigned long)CountDownDelayTimes[9])) countdownDelayTime = CountDownDelayTimes[9];

    if ((CurrentTime - LastCountdownReportTime) > countdownDelayTime) {

        if (DecrementingBonusCounter) {

            // Only give sound & score if this isn't a tilt
            if (NumTiltWarnings <= MaxTiltWarnings) {
                PlaySoundEffect(SOUND_EFFECT_BONUS_COUNT_1k);
                CurrentScores[CurrentPlayer] += 1000;
            }

            DecrementingBonusCounter -= 1;
            Bonus[CurrentPlayer] = DecrementingBonusCounter;
            ShowBonusLamps();

        } else if (BonusCountDownEndTime == 0xFFFFFFFF) {
            IncrementingBonusXCounter += 1;
            if (BonusX[CurrentPlayer] > 1) {
                DecrementingBonusCounter = TotalBonus;
                Bonus[CurrentPlayer] = TotalBonus;
                ShowBonusLamps();
                BonusX[CurrentPlayer] -= 1;
                if (BonusX[CurrentPlayer] == 9) BonusX[CurrentPlayer] = 8;
            } else {
                BonusX[CurrentPlayer] = TotalBonusX;
                Bonus[CurrentPlayer] = TotalBonus;
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
            if ((CurrentNumPlayers > (NumMatchSpins - 40)) && ((CurrentScores[NumMatchSpins - 40] / 10) % 10) == MatchDigit) {
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
            if (RPU_OS_NUM_DIGITS == 7) {
                upperMask = 0x1F;
                lowerMask = 0x60;
            }
            if ((CurrentTime / 200) % 2) {
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
#if (RPU_MPU_ARCHITECTURE < 10)
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

// A function to handle only a small subset of switched during the "drain" phase to start wizard mode.
// In this state the ball is still "in play" but we're just waiting for it to drain so we only care about
// switches that need to be used to keep it from getting stuck.
void HandleSwitchesMinimal(byte switchHit) {
    switch (switchHit) {  
    case SW_C_SAUCER:
        RPU_PushToSolenoidStack(SOL_C_SAUCER, 16, true);
        break;
    case SW_R_SAUCER:
        if (CurrentTime > WizardModeEndTime + 3500) {
            RPU_PushToSolenoidStack(SOL_R_SAUCER, 16, true);
        }
        break;
    default:
        break;
    }   
}

void HandleGamePlaySwitches(byte switchHit) {
    if (PreparingWizardMode || WizardModeEnding)
    {
        HandleSwitchesMinimal(switchHit);
        return;
    }

    switch (switchHit) {

    case SW_LEFT_SLING:
    case SW_RIGHT_SLING:
        CurrentScores[CurrentPlayer] += 150 * PlayfieldMultiplier[CurrentPlayer];
        PlaySoundEffect(SOUND_EFFECT_SLINGSHOT);
        LastSwitchHitTime = CurrentTime;

        if (!WizardModeActive) {
            SpinnerToggle();

            if (!SkillShotActive && CurrentTime > SkillShotGracePeroidEnd) {
                // Only do the toggle if skill shot is NOT active - don't want to overwrite those lights
                SpaceToggle(); 
            }
        }

        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_L_POP_BUMPER:
    case SW_C_POP_BUMPER:
    case SW_R_POP_BUMPER:
        if (WIZARD_TEST_MODE && !WizardModeActive) {
            if (PlayerGoalProgress[CurrentPlayer].S_Complete == false) { PlayerGoalProgress[CurrentPlayer].S_Complete = true; }
            else if (PlayerGoalProgress[CurrentPlayer].P_Complete == false) {  PlayerGoalProgress[CurrentPlayer].P_Complete = true; }
            else if (PlayerGoalProgress[CurrentPlayer].A_Complete == false) {  PlayerGoalProgress[CurrentPlayer].A_Complete = true; }
            else if (PlayerGoalProgress[CurrentPlayer].C_Complete == false) {  PlayerGoalProgress[CurrentPlayer].C_Complete = true; }
            else if (PlayerGoalProgress[CurrentPlayer].E_Complete == false) {  PlayerGoalProgress[CurrentPlayer].E_Complete = true; }
        }

        if (IsSuperPopsActive(CurrentTime)) {
            PlaySoundEffect(SOUND_EFFECT_POPBUMPER);
            CurrentScores[CurrentPlayer] += (SCORE_POPFRENZY)*PlayfieldMultiplier[CurrentPlayer];
        } else {
            // Super pops are not active
            if (WizardModeActive) {
                if (switchHit == SW_L_POP_BUMPER || switchHit == SW_R_POP_BUMPER) {
                    WizardModeProgress.TopPopsHit = true;
                }
                else {
                    // Center bumper
                    WizardModeProgress.BottomPopHit = true;
                }
            } else {
                NumberOfHits[CurrentPlayer] += 1;
            }
            if (NumberOfHits[CurrentPlayer] >= 25) {
                NumberOfHits[CurrentPlayer] = 0;
                RPU_SetDisplayCredits(Credits);
                PlayerGoalProgress[CurrentPlayer].P_Complete = true;
                PlayBackgroundSong(SOUND_EFFECT_HURRY_UP);
                StartSuperPops(CurrentTime);
                if (CountGoalsCompleted(CurrentPlayer) >= 5) {
                    QueueNotification(SOUND_EFFECT_SUPERPOPS_WIZARD, 9);
                } else if (CountGoalsCompleted(CurrentPlayer) == 3) {
                    QueueNotification(SOUND_EFFECT_SUPERPOPS_EXTRABALL, 9);
                } else {
                    QueueNotification(SOUND_EFFECT_SUPERPOP_GOAL, 9);
                }
                
            } else if (!WizardModeActive) {
                RPU_SetDisplayCredits(NumberOfHits[CurrentPlayer]);
            }
            CurrentScores[CurrentPlayer] += 100 * PlayfieldMultiplier[CurrentPlayer];
            PlaySoundEffect(SOUND_EFFECT_POPBUMPER);
        }
        if (!WizardModeActive) {
            if (!SkillShotActive && CurrentTime > SkillShotGracePeroidEnd) {
                // Only do the toggle if skill shot is NOT active - don't want to overwrite those lights
                SpaceToggle(); 
            }
        }
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_DROP_1:
        CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
        if (!WizardModeActive) {
            AddToBonus(1);
            SpinnerToggle();
            if (!RPU_ReadLampState(LAMP_5000)){
                RPU_SetLampState(LAMP_5000, 1, 0, 0);
            }
            PlaySoundEffect(SOUND_EFFECT_DROPTARGET);
        } else {
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET1);
        }
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;
    case SW_DROP_2:
        CurrentScores[CurrentPlayer] += 2000 * PlayfieldMultiplier[CurrentPlayer];
        if (!WizardModeActive) {
            AddToBonus(1);
            SpinnerToggle();
            if (!RPU_ReadLampState(LAMP_OPENGATE)){
                QueueNotification(SOUND_EFFECT_GATE_LIT, 9);
                RPU_SetLampState(LAMP_OPENGATE, 1, 0, 0);
            }    
            PlaySoundEffect(SOUND_EFFECT_DROPTARGET);
        } else {
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET1);
        }
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;
    case SW_DROP_3:
        CurrentScores[CurrentPlayer] += 3000 * PlayfieldMultiplier[CurrentPlayer];
        if (!WizardModeActive) {
            AddToBonus(1);
            SpinnerToggle();
            if (!RPU_ReadLampState(LAMP_L_OUTLANE)){
                QueueNotification(SOUND_EFFECT_L_OUTLANE_LIT, 9);
                RPU_SetLampState(LAMP_L_OUTLANE, 1, 0, 0);
            }
            PlaySoundEffect(SOUND_EFFECT_DROPTARGET);
        } else {
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET1);
        }
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;
    case SW_DROP_4:
        CurrentScores[CurrentPlayer] += 4000 * PlayfieldMultiplier[CurrentPlayer];
        if (!WizardModeActive) {
            AddToBonus(1);
            RPU_SetLampState(LAMP_DROP_TARGET, 1, 0, 500);
            SpinnerToggle();
            PlaySoundEffect(SOUND_EFFECT_BOING);
        } else {
            WizardModeProgress.InlineTargetsCompleted = true;
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET1);
        }
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_L_SPINNER:
        if (IsSuperSpinnerActive(CurrentTime)) {
            PlaySoundEffect(SOUND_EFFECT_SPINNERFRENZY);
            CurrentScores[CurrentPlayer] += (SCORE_SPINNERFRENZY)*PlayfieldMultiplier[CurrentPlayer];
        } else {
            if (WizardModeActive) {
                WizardModeProgress.LeftSpinnerSpins += 1;
                if (WizardModeProgress.LeftSpinnerSpins > WIZARD_MODE_LEFT_SPINS_REQUIRED) {
                    WizardModeProgress.LeftSpinnerSpins = WIZARD_MODE_LEFT_SPINS_REQUIRED;
                }
            } else {
                NumberOfSpins[CurrentPlayer] += 1;
            }
            if (NumberOfSpins[CurrentPlayer] >= 200) {
                NumberOfSpins[CurrentPlayer] = 1;
                StartSuperSpinner(CurrentTime);
                RPU_SetLampState(LAMP_LOWER_S, 1, 0, 0);
                PlayerGoalProgress[CurrentPlayer].S_Complete = true;
                RPU_SetDisplayCredits(Credits);
                PlayBackgroundSong(SOUND_EFFECT_HURRY_UP);
                if (CountGoalsCompleted(CurrentPlayer) >= 5){
                    QueueNotification(SOUND_EFFECT_SUPERSPINNER_WIZARD, 9);
                } else if (CountGoalsCompleted(CurrentPlayer) == 3){
                    QueueNotification(SOUND_EFFECT_SUPERSPINNER_EXTRABALL, 9);
                } else {
                    QueueNotification(SOUND_EFFECT_SUPERSPINNER_GOAL, 9);
                }
                
                
            } else if (!WizardModeActive) {
                RPU_SetDisplayCredits(NumberOfSpins[CurrentPlayer]);
            }
            if (NumberOfSpins[CurrentPlayer] > 0 && NumberOfSpins[CurrentPlayer] < 51) {
                CurrentScores[CurrentPlayer] += (SCORE_SPINNER1)*PlayfieldMultiplier[CurrentPlayer];
                PlaySoundEffect(SOUND_EFFECT_SPINNER100);
            }
            if (NumberOfSpins[CurrentPlayer] > 50 && NumberOfSpins[CurrentPlayer] < 101) {
                CurrentScores[CurrentPlayer] += (SCORE_SPINNER2)*PlayfieldMultiplier[CurrentPlayer];
                PlaySoundEffect(SOUND_EFFECT_SPINNER200);
            }
            if (NumberOfSpins[CurrentPlayer] > 100 && NumberOfSpins[CurrentPlayer] < 151) {
                CurrentScores[CurrentPlayer] += (SCORE_SPINNER3)*PlayfieldMultiplier[CurrentPlayer];
                PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
            }
            if (NumberOfSpins[CurrentPlayer] > 150 && NumberOfSpins[CurrentPlayer] < 200) {
                CurrentScores[CurrentPlayer] += (SCORE_SPINNER4)*PlayfieldMultiplier[CurrentPlayer];
                PlaySoundEffect(SOUND_EFFECT_SPINNER2000);
            }
        }

        if (WizardModeActive) {
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET2);
            CurrentScores[CurrentPlayer] += (SCORE_SPINNER1)*PlayfieldMultiplier[CurrentPlayer];
        }

        LastTimeSpinnerHit = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_CL_SPINNER:
        if (IsSuperSuperBlastOffActive(CurrentTime)) {
            PlaySoundEffect(SOUND_EFFECT_SPINNERCENTER);
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1)*PlayfieldMultiplier[CurrentPlayer];
        } else {
            if (CenterSpinnerStatus == CENTER_LEFT_SPINNER_LIT && !WizardModeActive) { 
                NumberOfCenterSpins[CurrentPlayer] += 1;
                if (NumberOfCenterSpins[CurrentPlayer] > 0 && NumberOfCenterSpins[CurrentPlayer] < 21) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER100);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 20 && NumberOfCenterSpins[CurrentPlayer] < 41) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER2)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER100);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 40 && NumberOfCenterSpins[CurrentPlayer] < 61) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER3)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER200);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 60 && NumberOfCenterSpins[CurrentPlayer] < 81) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER4)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 80 && NumberOfCenterSpins[CurrentPlayer] < 100) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER5)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
                }
            } else if (!WizardModeActive) {
                if (NumberOfCenterSpins[CurrentPlayer] > 0) {
                    NumberOfCenterSpins[CurrentPlayer] -= 1;
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 0 && NumberOfCenterSpins[CurrentPlayer] < 21) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER100);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 20 && NumberOfCenterSpins[CurrentPlayer] < 41) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER2)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER100);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 40 && NumberOfCenterSpins[CurrentPlayer] < 61) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER3)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER200);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 60 && NumberOfCenterSpins[CurrentPlayer] < 81) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER4)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 80 && NumberOfCenterSpins[CurrentPlayer] < 100) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER5)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
                }
            }

            if (NumberOfCenterSpins[CurrentPlayer] >= 100) {
                NumberOfCenterSpins[CurrentPlayer] = 1;
                StartSuperBlastOff(CurrentTime);
                PlayBackgroundSong(SOUND_EFFECT_ALARM);
                QueueNotification(SOUND_EFFECT_FIRE_ROCKET, 9);
                CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1)*PlayfieldMultiplier[CurrentPlayer];
                RPU_SetDisplayBallInPlay(CurrentBallInPlay);
            } else if (NumberOfCenterSpins[CurrentPlayer] < 1) {
                NumberOfCenterSpins[CurrentPlayer] = 1;
            } else if (!WizardModeActive) {
                RPU_SetDisplayBallInPlay( NumberOfCenterSpins[CurrentPlayer]);
            }
        }

        if (WizardModeActive) {
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET2);
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1)*PlayfieldMultiplier[CurrentPlayer];
            WizardModeProgress.CenterSpinnerSpins += 1;
            if (WizardModeProgress.CenterSpinnerSpins > WIZARD_MODE_CENTER_SPINS_REQUIRED) {
                WizardModeProgress.CenterSpinnerSpins = WIZARD_MODE_CENTER_SPINS_REQUIRED;
            }
        }

        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_CR_SPINNER:
        if (IsSuperSuperBlastOffActive(CurrentTime)) {
            PlaySoundEffect(SOUND_EFFECT_SPINNERCENTER);
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1)*PlayfieldMultiplier[CurrentPlayer];
        } else {
            if (RPU_ReadLampState(LAMP_CR_WHENLIT) && !WizardModeActive) {
                NumberOfCenterSpins[CurrentPlayer] += 1;
                if (NumberOfCenterSpins[CurrentPlayer] > 0 && NumberOfCenterSpins[CurrentPlayer] < 21) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER100);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 20 && NumberOfCenterSpins[CurrentPlayer] < 41) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER2)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER100);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 40 && NumberOfCenterSpins[CurrentPlayer] < 61) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER3)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER200);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 60 && NumberOfCenterSpins[CurrentPlayer] < 81) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER4)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 80 && NumberOfCenterSpins[CurrentPlayer] < 101) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER5)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
                }
            } else if (RPU_ReadLampState(LAMP_CL_WHENLIT) && !WizardModeActive) {
                if (NumberOfCenterSpins[CurrentPlayer] > 0) {
                    NumberOfCenterSpins[CurrentPlayer] -= 1;
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 0 && NumberOfCenterSpins[CurrentPlayer] < 21) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER100);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 20 && NumberOfCenterSpins[CurrentPlayer] < 41) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER2)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER100);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 40 && NumberOfCenterSpins[CurrentPlayer] < 61) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER3)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER200);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 60 && NumberOfCenterSpins[CurrentPlayer] < 81) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER4)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
                }
                if (NumberOfCenterSpins[CurrentPlayer] > 80 && NumberOfCenterSpins[CurrentPlayer] < 101) {
                    CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER5)*PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_SPINNER1000);
                }
            }
            if (NumberOfCenterSpins[CurrentPlayer] >= 100) {
                NumberOfCenterSpins[CurrentPlayer] = 1;
                StartSuperBlastOff(CurrentTime);
                PlayBackgroundSong(SOUND_EFFECT_ALARM);
                CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1)*PlayfieldMultiplier[CurrentPlayer];
                RPU_SetDisplayBallInPlay(CurrentBallInPlay);
            } else if (NumberOfCenterSpins[CurrentPlayer] < 1) {
                NumberOfCenterSpins[CurrentPlayer] = 1;
            } else if (!WizardModeActive) {
                RPU_SetDisplayBallInPlay( NumberOfCenterSpins[CurrentPlayer]);
            }
        }
        if (WizardModeActive) {
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET2);
            CurrentScores[CurrentPlayer] += (SCORE_C_SPINNER1)*PlayfieldMultiplier[CurrentPlayer];
            WizardModeProgress.CenterSpinnerSpins += 1;;
            if (WizardModeProgress.CenterSpinnerSpins > WIZARD_MODE_CENTER_SPINS_REQUIRED) {
                WizardModeProgress.CenterSpinnerSpins = WIZARD_MODE_CENTER_SPINS_REQUIRED;
            }
        }

        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;

        break;

    case SW_C_SAUCER:
        if ((SkillShotActive == true || CurrentTime <= SkillShotGracePeroidEnd) && CurrentTime > SkillShotCelebrationBlinkEndTime) {
            unsigned int kickoutWaitTime = 3000;

            if (RPU_ReadLampState(LAMP_TOP_S)) {
                QueueNotification(SOUND_EFFECT_SPINNER_HELD, 9);
                HOLD_SPINNER_PROGRESS[CurrentPlayer] = true;
                RPU_SetLampState(LAMP_TOP_S, 1, 0, 250);
                CurrentScores[CurrentPlayer] += SCORE_SKILL_SHOT;
            } else if (RPU_ReadLampState(LAMP_TOP_P)) {
                QueueNotification(SOUND_EFFECT_POP_HELD, 9);
                HOLD_POP_PROGRESS[CurrentPlayer] = true;
                RPU_SetLampState(LAMP_TOP_P, 1, 0, 250);
                CurrentScores[CurrentPlayer] += SCORE_SKILL_SHOT;
            } else if (RPU_ReadLampState(LAMP_TOP_A)) {
                QueueNotification(SOUND_EFFECT_BLASTOFF_HELD, 9);
                HOLD_BLASTOFF_PROGRESS[CurrentPlayer] = true;
                RPU_SetLampState(LAMP_TOP_A, 1, 0, 250);
                CurrentScores[CurrentPlayer] += SCORE_SKILL_SHOT;
            } else if (RPU_ReadLampState(LAMP_TOP_C)) {
                QueueNotification(SOUND_EFFECT_BONUS_HELD, 9);
                HOLD_BONUS[CurrentPlayer] = true;
                RPU_SetLampState(LAMP_TOP_C, 1, 0, 250);
                CurrentScores[CurrentPlayer] += SCORE_SKILL_SHOT;
            } else if (RPU_ReadLampState(LAMP_TOP_E)) {
                QueueNotification(SOUND_EFFECT_PLAYFIELDX_HELD, 9);
                HOLD_PLAYFIELDX[CurrentPlayer] = true;
                RPU_SetLampState(LAMP_TOP_E, 1, 0, 250);
                CurrentScores[CurrentPlayer] += SCORE_SKILL_SHOT;
            } else {
                kickoutWaitTime = 2000;
                // Missed skill shot, only award base saucer score
                QueueNotification(SOUND_EFFECT_SKILLSHOT_MISSED, 9);
                CurrentScores[CurrentPlayer] += 1000;
            }

            SkillShotActive = false;
            SkillShotGracePeroidEnd = 0;
            SkillShotCelebrationBlinkEndTime = CurrentTime + kickoutWaitTime;
            RPU_PushToTimedSolenoidStack(SOL_C_SAUCER, 16, CurrentTime + kickoutWaitTime, true);

        } else if (IsSuperSuperBlastOffActive(CurrentTime)) {
            StopSuperBlastOff();
            SuperBlastOffCollectedHoldTime = CurrentTime + 3200;
            // Super Blast off was achieved, mark goal complete
            RPU_SetLampState(LAMP_LOWER_A, 1, 0, 0);
            PlayerGoalProgress[CurrentPlayer].A_Complete = true;
            if (CountGoalsCompleted(CurrentPlayer) >= 5){
                    QueueNotification(SOUND_EFFECT_BLASTOFF_WIZARD, 9);
                } else if (CountGoalsCompleted(CurrentPlayer) == 3){
                    QueueNotification(SOUND_EFFECT_BLASTOFF_EXTRABALL, 9);
                } else {
                    QueueNotification(SOUND_EFFECT_BLASTOFF_GOAL, 9);
                }
            RPU_PushToTimedSolenoidStack(SOL_C_SAUCER, 16, CurrentTime + 3000, true);

        } else if (!WizardModeActive) {
            // Regular hit during unstructured play
            if (CurrentTime >= SuperBlastOffCollectedHoldTime && CurrentTime >= SkillShotCelebrationBlinkEndTime) {
                if (RPU_ReadLampState(LAMP_TOP_S) && RPU_ReadLampState(LAMP_TOP_P) && RPU_ReadLampState(LAMP_TOP_A) && RPU_ReadLampState(LAMP_TOP_C) &&
                RPU_ReadLampState(LAMP_TOP_E)){
                    CurrentScores[CurrentPlayer] += 5000 * PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_ROLL_OVER);
                    RPU_PushToTimedSolenoidStack(SOL_C_SAUCER, 16, CurrentTime + 500, false);
                } else {
                    CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
                    PlaySoundEffect(SOUND_EFFECT_ROLL_OVER);
                    RPU_PushToTimedSolenoidStack(SOL_C_SAUCER, 16, CurrentTime + 500, false);
                }
            }
            AddToBonus(1);
        }
        else {
            //Wizard mode is active
            WizardModeProgress.CenterSaucerHit = true;
            CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
            RPU_PushToTimedSolenoidStack(SOL_C_SAUCER, 16, CurrentTime + 2000, false);
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET1);
        }
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_R_SAUCER:
        if (!WizardModeActive){
            CurrentScores[CurrentPlayer] += 25000;
            IncreasePlayfieldMultiplier();
            RPU_SetLampState(LAMP_DROP_TARGET, 0, 0, 0);
            RPU_PushToTimedSolenoidStack(SOL_R_SAUCER, 10, CurrentTime + 4000, true);
            RPU_PushToTimedSolenoidStack(SOL_DROP_TARGET_RESET, 10, CurrentTime + 1500, true);
        } else if (WizardModeActive){
            RPU_PushToTimedSolenoidStack(SOL_R_SAUCER, 10, CurrentTime + 2000, true);
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET1); //TODO "More Targets Required to Save Station" sound needed

            if (AreWizardModeGoalsCompleted()) {
                // Wizard Mode fully Completed
                CurrentScores[CurrentPlayer] += WIZARD_MODE_COMPLETED_AWARD;
                RPU_PushToTimedSolenoidStack(SOL_R_SAUCER, 10, CurrentTime + 3000, true);
                RPU_PushToTimedSolenoidStack(SOL_DROP_TARGET_RESET, 10, CurrentTime + 1500, true);
                QueueNotification(SOUND_EFFECT_WIZARD_MODE_COMPLETE, 9);

                RPU_TurnOffAllLamps();
                RPU_SetDisableFlippers(true);
                RPU_SetDisableGate(true);
                RPU_DisableSolenoidStack();
                WizardModeActive = false;
                WizardModeEnding = true;
                WizardModeEndTime = CurrentTime;
            }
        }
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_R_TARGET:
        if (!WizardModeActive) {
            if (RPU_ReadLampState(LAMP_OPENGATE) && !RPU_ReadLampState(LAMP_5000) && !RPU_ReadLampState(LAMP_EXTRABALL)) {
                CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
                QueueNotification(SOUND_EFFECT_GATEOPEN, 9);
                RPU_SetDisableGate(true);
                GateOpen = false;
                RPU_SetLampState(LAMP_R_OUTLANE, 1, 0, 500);
                RPU_SetLampState(LAMP_OPENGATE, 0, 0, 0);
            } else if (RPU_ReadLampState(LAMP_OPENGATE) && RPU_ReadLampState(LAMP_5000) && !RPU_ReadLampState(LAMP_EXTRABALL)) {
                CurrentScores[CurrentPlayer] += 5000 * PlayfieldMultiplier[CurrentPlayer];
                QueueNotification(SOUND_EFFECT_GATEOPEN, 9);
                RPU_SetDisableGate(true);
                GateOpen = false;
                RPU_SetLampState(LAMP_R_OUTLANE, 1, 0, 500);
                RPU_SetLampState(LAMP_OPENGATE, 0, 0, 0);
            } else if (RPU_ReadLampState(LAMP_EXTRABALL) && !RPU_ReadLampState(LAMP_5000) && !RPU_ReadLampState(LAMP_OPENGATE)) {
                CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
                RPU_SetLampState(LAMP_EXTRABALL, 0, 0, 0);
                if (CountGoalsCompleted(CurrentPlayer) == 3 && !GoalExtraBallCollected){
                    AwardGoalExtraBall();
                    GoalExtraBallCollected = true;
                } else if (TargetBankComplete[CurrentPlayer] >= 2){
                    AwardExtraBall();
                    ExtraBallCollected = true;
                }
            } else if (RPU_ReadLampState(LAMP_EXTRABALL) && RPU_ReadLampState(LAMP_5000) && !RPU_ReadLampState(LAMP_OPENGATE)) {
                CurrentScores[CurrentPlayer] += 5000 * PlayfieldMultiplier[CurrentPlayer];
                RPU_SetLampState(LAMP_EXTRABALL, 0, 0, 0);
                if (CountGoalsCompleted(CurrentPlayer) == 3 && !GoalExtraBallCollected){
                    AwardGoalExtraBall();
                    GoalExtraBallCollected = true;
                } else if (TargetBankComplete[CurrentPlayer] >= 2){
                    AwardExtraBall();
                    ExtraBallCollected = true;
                }
            } else if (RPU_ReadLampState(LAMP_EXTRABALL) && RPU_ReadLampState(LAMP_OPENGATE) && !RPU_ReadLampState(LAMP_5000)) {
                CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
                QueueNotification(SOUND_EFFECT_GATEOPEN, 9);
                RPU_SetDisableGate(true);
                GateOpen = false;
                RPU_SetLampState(LAMP_R_OUTLANE, 1, 0, 500);
                RPU_SetLampState(LAMP_OPENGATE, 0, 0, 0);
                RPU_SetLampState(LAMP_EXTRABALL, 0, 0, 0);
                if (CountGoalsCompleted(CurrentPlayer) == 3 && !GoalExtraBallCollected){
                    AwardGoalExtraBall();
                    GoalExtraBallCollected = true;
                } else if (TargetBankComplete[CurrentPlayer] >= 2){
                    AwardExtraBall();
                    ExtraBallCollected = true;
                }
            } else if (RPU_ReadLampState(LAMP_EXTRABALL) && RPU_ReadLampState(LAMP_OPENGATE) && RPU_ReadLampState(LAMP_5000)) {
                CurrentScores[CurrentPlayer] += 5000 * PlayfieldMultiplier[CurrentPlayer];
                QueueNotification(SOUND_EFFECT_GATEOPEN, 9);
                RPU_SetDisableGate(true);
                GateOpen = false;
                RPU_SetLampState(LAMP_R_OUTLANE, 1, 0, 500);
                RPU_SetLampState(LAMP_EXTRABALL, 0, 0, 0);
                RPU_SetLampState(LAMP_OPENGATE, 0, 0, 0);
                if (CountGoalsCompleted(CurrentPlayer) == 3 && !GoalExtraBallCollected){
                    AwardGoalExtraBall();
                    GoalExtraBallCollected = true;
                } else if (TargetBankComplete[CurrentPlayer] >= 2){
                    AwardExtraBall();
                    ExtraBallCollected = true;
                }
            } else if (!RPU_ReadLampState(LAMP_EXTRABALL) && !RPU_ReadLampState(LAMP_OPENGATE) && RPU_ReadLampState(LAMP_5000)) {
                CurrentScores[CurrentPlayer] += 5000 * PlayfieldMultiplier[CurrentPlayer];
                PlaySoundEffect(SOUND_EFFECT_SWITCHHIT);
            } else
                CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
                AddToBonus(1);
                PlaySoundEffect(SOUND_EFFECT_SWITCHHIT);
        }
        else {
            // Wizard mode is active
            CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
            WizardModeProgress.RightTargetHit = true;
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET1);
        }
        

        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_TARGET1:
        if (!WizardModeActive) {
            if (RPU_ReadLampState(LAMP_TARGET_2) && RPU_ReadLampState(LAMP_TARGET_3) && RPU_ReadLampState(LAMP_TARGET_4) && RPU_ReadLampState(LAMP_TARGET_5)) {
                TargetBankComplete[CurrentPlayer] += 1;
            }
            AddToBonus(1);
            RPU_SetLampState(LAMP_TARGET_1, 1, 0, 0);
            SpinnerToggle();
            TargetBank();
            PlaySoundEffect(SOUND_EFFECT_SWITCHHIT);
        } else {
            WizardModeProgress.Target1Hit = true;
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET1);
        }
        CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_TARGET2:
        if (!WizardModeActive) {
            if (RPU_ReadLampState(LAMP_TARGET_1) && RPU_ReadLampState(LAMP_TARGET_3) && RPU_ReadLampState(LAMP_TARGET_4) && RPU_ReadLampState(LAMP_TARGET_5)) {
                TargetBankComplete[CurrentPlayer] += 1;
            }
            AddToBonus(1);
            RPU_SetLampState(LAMP_TARGET_2, 1, 0, 0);
            SpinnerToggle();
            TargetBank();
            PlaySoundEffect(SOUND_EFFECT_SWITCHHIT);
        } else {
            WizardModeProgress.Target2Hit = true;
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET1);
        }
        CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_TARGET3:
        if (!WizardModeActive) {
            if (RPU_ReadLampState(LAMP_TARGET_1) && RPU_ReadLampState(LAMP_TARGET_2) && RPU_ReadLampState(LAMP_TARGET_4) && RPU_ReadLampState(LAMP_TARGET_5)) {
                TargetBankComplete[CurrentPlayer] += 1;
            }
            AddToBonus(1);
            RPU_SetLampState(LAMP_TARGET_3, 1, 0, 0);
            SpinnerToggle();
            TargetBank();
            PlaySoundEffect(SOUND_EFFECT_SWITCHHIT);
        } else {
            WizardModeProgress.Target3Hit = true;
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET1);
        }
        CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_TARGET4:
        if (!WizardModeActive) {
            if (RPU_ReadLampState(LAMP_TARGET_1) && RPU_ReadLampState(LAMP_TARGET_2) && RPU_ReadLampState(LAMP_TARGET_3) && RPU_ReadLampState(LAMP_TARGET_5)) {
                TargetBankComplete[CurrentPlayer] += 1;
            }
            AddToBonus(1);
            RPU_SetLampState(LAMP_TARGET_4, 1, 0, 0);
            SpinnerToggle();
            TargetBank();
            PlaySoundEffect(SOUND_EFFECT_SWITCHHIT);
        } else {
            WizardModeProgress.Target4Hit = true;
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET1);
        }
        CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_TARGET5:
        if (!WizardModeActive) {
            if (RPU_ReadLampState(LAMP_TARGET_1) && RPU_ReadLampState(LAMP_TARGET_2) && RPU_ReadLampState(LAMP_TARGET_3) && RPU_ReadLampState(LAMP_TARGET_4)) {
                TargetBankComplete[CurrentPlayer] += 1;
            }
            AddToBonus(1);
            RPU_SetLampState(LAMP_TARGET_5, 1, 0, 0);
            SpinnerToggle();
            TargetBank();
            PlaySoundEffect(SOUND_EFFECT_SWITCHHIT);
        } else {
            WizardModeProgress.Target5Hit = true;
            PlaySoundEffect(SOUND_EFFECT_WIZARDTARGET1);
        }
        CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_R_INLANE:
        CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier[CurrentPlayer];
        PlaySoundEffect(SOUND_EFFECT_ROLL_OVER);
        AddToBonus(1);
        LastSwitchHitTime = CurrentTime;
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
        break;

    case SW_R_OUTLANE:
        if (GateOpen == false) {
            QueueNotification(SOUND_EFFECT_GATE_CLOSED, 9);
            CurrentScores[CurrentPlayer] += 100;
            RPU_SetLampState(LAMP_OPENGATE, 0, 0, 0);
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
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
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
            QueueNotification(SOUND_EFFECT_SHOOTAGAIN, 9);
            returnState = MACHINE_STATE_INIT_NEW_BALL;
        } else if (PreparingWizardMode) {
            returnState = MACHINE_STATE_INIT_NEW_BALL;
        } else if (WizardModeEnding) {
            QueueNotification(SOUND_EFFECT_WIZARD_MODE_END, 9);
            returnState = MACHINE_STATE_INIT_NEW_BALL;
            //Keep WizardModeEnding as true so next ball knows we came out of wiz mode
        } else if (WizardModeActive) {
            // If we are here it means the player drained before completing wizard mode
            QueueNotification(SOUND_EFFECT_WIZARD_MODE_FAILED, 9);
            returnState = MACHINE_STATE_INIT_NEW_BALL;
            WizardModeActive = false;
            WizardModeEnding = true;
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
            } else
                returnState = MACHINE_STATE_INIT_NEW_BALL;
        }
    } else if (curState == MACHINE_STATE_MATCH_MODE) {
        returnState = ShowMatchSequence(curStateChanged);
    }

    byte switchHit;
    unsigned long lastBallFirstSwitchHitTime = BallFirstSwitchHitTime;

    while ((switchHit = RPU_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY) {
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
                if (TimeRequiredToResetGame != 99 && (CurrentTime - CreditResetPressStarted) >= ((unsigned long)TimeRequiredToResetGame * 1000)) {
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

    if (lastBallFirstSwitchHitTime == 0 && BallFirstSwitchHitTime != 0 && !DisableBallSaveThisBall) {
        BallSaveEndTime = BallFirstSwitchHitTime + ((unsigned long)BallSaveNumSeconds) * 1000;
    }
    if (CurrentTime > (BallSaveEndTime + BALL_SAVE_GRACE_PERIOD)) {
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

unsigned long CountGoalsCompleted(unsigned char player) {
    unsigned long result = 0;

    if (PlayerGoalProgress[player].S_Complete) result += 1;
    if (PlayerGoalProgress[player].P_Complete) result += 1;
    if (PlayerGoalProgress[player].A_Complete) result += 1;
    if (PlayerGoalProgress[player].C_Complete) result += 1;
    if (PlayerGoalProgress[player].E_Complete) result += 1;

    return result;
}

#if (RPU_MPU_ARCHITECTURE >= 10)
unsigned long LastLEDUpdateTime = 0;
byte LEDPhase = 0;
#endif
// unsigned long NumLoops = 0;
// unsigned long LastLoopReportTime = 0;

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

#if (RPU_MPU_ARCHITECTURE >= 10)
    if (LastLEDUpdateTime == 0 || (CurrentTime - LastLEDUpdateTime) > 250) {
        LastLEDUpdateTime = CurrentTime;
        RPU_SetBoardLEDs((LEDPhase % 8) == 1 || (LEDPhase % 8) == 3, (LEDPhase % 8) == 5 || (LEDPhase % 8) == 7);
        LEDPhase += 1;
    }
#endif
}

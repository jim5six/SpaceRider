// Here, a define should be made for every lamp on the machine.
// For demonstration, here are some example lamps that are typical
// for different architectures. This section should be deleted
// when implementing a particular machine.

#if (RPU_MPU_ARCHITECTURE<10)

// Space Rider Lamp definitions
#define LAMP_BONUS_10                       0
#define LAMP_BONUS_20                       1
#define LAMP_BONUS_30                       2
#define LAMP_BONUS_40                       3
#define LAMP_BONUS_50                       4
#define LAMP_BONUS_60                       5
#define LAMP_BONUS_70                       6
#define LAMP_BONUS_80                       7
#define LAMP_BONUS_90                       8
#define LAMP_5000                           9
#define LAMP_CR_WHENLIT                     10
#define LAMP_CL_WHENLIT                     11
#define LAMP_C_SPINNER_1                    12
#define LAMP_C_SPINNER_2                    13
#define LAMP_C_SPINNER_3                    14
#define LAMP_C_SPINNER_4                    15
#define LAMP_C_SPINNER_5                    16
#define LAMP_OPENGATE                       17
#define LAMP_R_OUTLANE                      18
#define LAMP_DROP_TARGET                    19
#define LAMP_L_SPINNER_100                  20
#define LAMP_L_SPINNER_200                  21
#define LAMP_L_SPINNER_1000                 22
#define LAMP_L_SPINNER_2000                 23
#define LAMP_TOP_S                          24
#define LAMP_TOP_P                          25
#define LAMP_TOP_A                          26
#define LAMP_TOP_C                          27
#define LAMP_TOP_E                          28
#define LAMP_TARGET_SPECIAL                 29
#define LAMP_L_OUTLANE                      30
#define LAMP_DROP_SPECIAL                   31
#define LAMP_TARGET_5                       32
#define LAMP_TARGET_4                       33
#define LAMP_TARGET_3                       34
#define LAMP_TARGET_2                       35
#define LAMP_TARGET_1                       36
#define LAMP_C_POP                          37
#define LAMP_LR_POP                         38
#define LAMP_EXTRABALL                      39
#define LAMP_LOWER_E                        40
#define LAMP_HEAD_MATCH                     41
#define LAMP_SHOOT_AGAIN                    42
#define LAMP_HEAD_SAME_PLAYER_SHOOTS_AGAIN  42
#define LAMP_APRON_CREDIT                   43
#define LAMP_BONUS_2X                       44
#define LAMP_BONUS_3X                       45
#define LAMP_BONUS_5X                       46
#define LAMP_BONUS_SPECIAL                  47
#define LAMP_HEAD_BIP                       48
#define LAMP_HEAD_HIGH_SCORE                49
#define LAMP_HEAD_TILT                      50
#define LAMP_HEAD_GAME_OVER                 51
#define LAMP_BONUS_100                      52
#define LAMP_BONUS_200                      53
#define LAMP_BONUS_300                      54
#define LAMP_BONUS_400                      55
#define LAMP_LOWER_S                        56
#define LAMP_LOWER_P                        57
#define LAMP_LOWER_A                        58
#define LAMP_LOWER_C                        59
#endif

// Space Rider Switches
#if (RPU_MPU_ARCHITECTURE<10)
#define SW_DROP_4                   0
#define SW_DROP_3                   1
#define SW_DROP_2                   2
#define SW_DROP_1                   3
#define SW_CREDIT_RESET             5
// Different definitions for Tilt so I can have the same
// code for different architectures
#define SW_TILT                     6
#define SW_PLUMB_TILT               6
#define SW_ROLL_TILT                6
#define SW_OUTHOLE                  7
#define SW_COIN_3                   8
#define SW_COIN_1                   9
#define SW_COIN_2                   10
#define SW_SLAM                     15
#define SW_PLAYFIELD_TILT           15
#define SW_L_SPINNER                16
#define SW_R_INLANE                 21
#define SW_R_OUTLANE                22
#define SW_C_SAUCER                 23
#define SW_CR_SPINNER               24
#define SW_TARGET1                  25
#define SW_TARGET2                  26
#define SW_TARGET3                  27
#define SW_TARGET4                  28
#define SW_TARGET5                  29
#define SW_L_OUTLANE                30
#define SW_R_SAUCER                 31
#define SW_CL_SPINNER               32
#define SW_RUBBER                   33
#define SW_R_TARGET                 34
#define SW_RIGHT_SLING              35
#define SW_LEFT_SLING               36
#define SW_C_POP_BUMPER             37
#define SW_L_POP_BUMPER             39
#define SW_R_POP_BUMPER             38
#endif

// Space Rider Solinoids
#define SOL_KNOCKER                 5
#define SOL_OUTHOLE                 6
#define SOL_R_POP_BUMPER            7
#define SOL_L_POP_BUMPER            8
#define SOL_C_POP_BUMPER            9
#define SOL_LEFT_SLING              10
#define SOL_RIGHT_SLING             11
#define SOL_C_SAUCER                12
#define SOL_R_SAUCER                13
#define SOL_DROP_TARGET_RESET       14

// Define Scoring
#define SCORE_SPINNER0              50
#define SCORE_SPINNER1              100
#define SCORE_SPINNER2              200
#define SCORE_SPINNER3              1000
#define SCORE_SPINNER4              2000
#define SCORE_C_SPINNER0            50
#define SCORE_C_SPINNER1            100
#define SCORE_C_SPINNER2            250
#define SCORE_C_SPINNER3            500
#define SCORE_C_SPINNER4            750
#define SCORE_C_SPINNER5            1000
#define SCORE_SPINNERFRENZY         10000
#define SCORE_POPFRENZY             5000
#define SCORE_BLASTOFF_COLLECT      50000
#define SCORE_SKILL_SHOT            25000
#define EXTRA_BALL_TOURN            50000
#define EXTRA_BALL_AGAIN            25000
#define SPECIAL_TOURN               50000
#define WIZARD_MODE_COMPLETED_AWARD 250000
#define WIZARD_MODE_LEFT_SPINS_REQUIRED 20
#define WIZARD_MODE_CENTER_SPINS_REQUIRED 20
#define WIZARD_MODE_GOAL_BLINK_PERIOD_MS 500

#if (RPU_MPU_ARCHITECTURE<10)
// Machines with a -17, -35, 100, and 200 architecture
// almost always have software based switch-triggered solenoids. 
// For those, you can define an array of solenoids and the switches
// that will trigger them:

#define NUM_SWITCHES_WITH_TRIGGERS          5 // total number of solenoid/switch pairs
#define NUM_PRIORITY_SWITCHES_WITH_TRIGGERS 5 // This number should match the define above

struct PlayfieldAndCabinetSwitch SolenoidAssociatedSwitches[] = {
  { SW_RIGHT_SLING, SOL_RIGHT_SLING, 4},
  { SW_LEFT_SLING, SOL_LEFT_SLING, 4},
  { SW_L_POP_BUMPER, SOL_L_POP_BUMPER, 4},
  { SW_C_POP_BUMPER, SOL_C_POP_BUMPER, 4},
  { SW_R_POP_BUMPER, SOL_R_POP_BUMPER, 4}
};
#endif

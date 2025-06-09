#ifndef TREVOR_LAMP_ANIMATIONS_H
#define TREVOR_LAMP_ANIMATIONS_H

#include "RPU.h"
#include "RPU_Config.h"

#define MAX_ANIMATION_STEPS (32)

#define MASK_NONE (0)
#define MASK_LAMP_BONUS_10 (0b1 << 0)
#define MASK_LAMP_BONUS_20 (0b1 << 1)
#define MASK_LAMP_BONUS_30 (0b1 << 2)
#define MASK_LAMP_BONUS_40 (0b1 << 3)
#define MASK_LAMP_BONUS_50 (0b1 << 4)
#define MASK_LAMP_BONUS_60 (0b1 << 5)
#define MASK_LAMP_BONUS_70 (0b1 << 6)
#define MASK_LAMP_BONUS_80 (0b1 << 7)
#define MASK_LAMP_BONUS_90 (0b1 << 8)
#define MASK_LAMP_5000 (0b1 << 9)
#define MASK_LAMP_CR_WHENLIT (0b1 << 10)
#define MASK_LAMP_CL_WHENLIT (0b1 << 11)
#define MASK_LAMP_C_SPINNER_1 (0b1 << 12)
#define MASK_LAMP_C_SPINNER_2 (0b1 << 13)
#define MASK_LAMP_C_SPINNER_3 (0b1 << 14)
#define MASK_LAMP_C_SPINNER_4 (0b1 << 15)
#define MASK_LAMP_C_SPINNER_5 (0b1 << 16)
#define MASK_LAMP_OPENGATE (0b1 << 17)
#define MASK_LAMP_R_OUTLANE (0b1 << 18)
#define MASK_LAMP_DROP_TARGET (0b1 << 19)
#define MASK_LAMP_L_SPINNER_100 (0b1 << 20)
#define MASK_LAMP_L_SPINNER_200 (0b1 << 21)
#define MASK_LAMP_L_SPINNER_1000 (0b1 << 22)
#define MASK_LAMP_L_SPINNER_2000 (0b1 << 23)
#define MASK_LAMP_TOP_S (0b1 << 24)
#define MASK_LAMP_TOP_P (0b1 << 25)
#define MASK_LAMP_TOP_A (0b1 << 26)
#define MASK_LAMP_TOP_C (0b1 << 27)
#define MASK_LAMP_TOP_E (0b1 << 28)
#define MASK_LAMP_TARGET_SPECIAL (0b1 << 29)
#define MASK_LAMP_L_OUTLANE (0b1 << 30)
#define MASK_LAMP_DROP_SPECIAL (0b1 << 31)
#define MASK_LAMP_TARGET_5 (0b1 << 32)
#define MASK_LAMP_TARGET_4 (0b1 << 33)
#define MASK_LAMP_TARGET_3 (0b1 << 34)
#define MASK_LAMP_TARGET_2 (0b1 << 35)
#define MASK_LAMP_TARGET_1 (0b1 << 36)
#define MASK_LAMP_C_POP (0b1 << 37)
#define MASK_LAMP_LR_POP (0b1 << 38)
#define MASK_LAMP_EXTRABALL (0b1 << 39)
#define MASK_LAMP_LOWER_E (0b1 << 40)
#define MASK_LAMP_HEAD_MATCH (0b1 << 41)
#define MASK_LAMP_SHOOT_AGAIN (0b1 << 42)
#define MASK_LAMP_HEAD_SAME_PLAYER_SHOOTS_AGAIN (0b1 << 42)
#define MASK_LAMP_APRON_CREDIT (0b1 << 43)
#define MASK_LAMP_BONUS_2X (0b1 << 44)
#define MASK_LAMP_BONUS_3X (0b1 << 45)
#define MASK_LAMP_BONUS_5X (0b1 << 46)
#define MASK_LAMP_BONUS_SPECIAL (0b1 << 47)
#define MASK_LAMP_HEAD_BIP (0b1 << 48)
#define MASK_LAMP_HEAD_HIGH_SCORE (0b1 << 49)
#define MASK_LAMP_HEAD_TILT (0b1 << 50)
#define MASK_LAMP_HEAD_GAME_OVER (0b1 << 51)
#define MASK_LAMP_BONUS_100 (0b1 << 52)
#define MASK_LAMP_BONUS_200 (0b1 << 53)
#define MASK_LAMP_BONUS_300 (0b1 << 54)
#define MASK_LAMP_BONUS_400 (0b1 << 55)
#define MASK_LAMP_LOWER_S (0b1 << 56)
#define MASK_LAMP_LOWER_P (0b1 << 57)
#define MASK_LAMP_LOWER_A (0b1 << 58)
#define MASK_LAMP_LOWER_C (0b1 << 59)
#define LAMP_MASK_COUNT (60)

// This file can define a series of animations, stored
// with each lamp as a bit in the following array.
// Lamp 0 = the first bit of the first byte, so "{0x01," below.
// The animations can be played with either of the helper functions
// at the bottom of this file.

enum AnimationType {
    ANIMATION_TOP_SPACE_ROTATE, // Top space rotate animation
    ANIMATION_SPINNER_ROLLING,  // Spinner rolling animation
    ANIMATION_COUNT             // Total number of animations
};

struct LampAnimation {
    AnimationType type; // Type of animation
    unsigned int numSteps;
    unsigned long long sequence[MAX_ANIMATION_STEPS]; // Sequence of lamp states
};

void ShowLampAnimation2(AnimationType animation, unsigned long holdTimePerStep, unsigned long currentTime, unsigned long turnOffStepDelay);

#endif

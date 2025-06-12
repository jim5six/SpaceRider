// Definition for the TrevorLampAnimations module

#include "TrevorLampAnimations.h"

LampAnimation Animations[ANIMATION_COUNT] = {
    {
        ANIMATION_TOP_SPACE_ROTATE,
        5,
        {
            MASK_LAMP_TOP_S,
            MASK_LAMP_TOP_P,
            MASK_LAMP_TOP_A,
            MASK_LAMP_TOP_C,
            MASK_LAMP_TOP_E
        }
    },
    {
        ANIMATION_SPINNER_ROLLING,
        5,
        {
            MASK_LAMP_L_SPINNER_100,
            MASK_LAMP_L_SPINNER_200,
            MASK_LAMP_L_SPINNER_1000,
            MASK_LAMP_L_SPINNER_2000,
            MASK_NONE
        }
    }
};

void ShowLampAnimation2(AnimationType animation, unsigned long holdTimePerStep, unsigned long currentTime, unsigned long turnOffStepDelay)
{
    bool valid_animation = false;
    LampAnimation anim_to_play;

    for (int i = 0; i < ANIMATION_COUNT; i++) 
    {
        if (Animations[i].type == animation) 
        {
            valid_animation = true;
            anim_to_play = Animations[i];
            break;
        }
    }

    if (valid_animation)
    {
        unsigned int turn_on_step_num = (currentTime / holdTimePerStep) % anim_to_play.numSteps;
        
        // TODO: Should step delay actually be reversed (numSteps - delay)?
        turnOffStepDelay = turnOffStepDelay % anim_to_play.numSteps;
        unsigned int turn_off_step_num = ((currentTime / holdTimePerStep) + turnOffStepDelay) % anim_to_play.numSteps;
    
        for (int lamp_num = 0; lamp_num < LAMP_MASK_COUNT; lamp_num++)
        {
            if (anim_to_play.sequence[turn_on_step_num] & (1 << lamp_num))
            {
                RPU_SetLampState(lamp_num, 1);
            }
            if (anim_to_play.sequence[turn_off_step_num] & (1 << lamp_num))
            {
                RPU_SetLampState(lamp_num, 0);
            }
        }
    }
}
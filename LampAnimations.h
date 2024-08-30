#ifndef LAMP_ANIMATIONS_H

#include "RPU_Config.h"
#include "RPU.h"

// This file can define a series of animations, stored 
// with each lamp as a bit in the following array.
// Lamp 0 = the first bit of the first byte, so "{0x01," below.
// The animations can be played with either of the helper functions
// at the bottom of this file.

// Lamp animation arrays
#define NUM_LAMP_ANIMATIONS       4
#define LAMP_ANIMATION_STEPS      16
#define NUM_LAMP_ANIMATION_BYTES  8
byte LampAnimations[NUM_LAMP_ANIMATIONS][LAMP_ANIMATION_STEPS][NUM_LAMP_ANIMATION_BYTES] = {
  //UP
{
 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 
 {0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00},
 {0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00},
 {0xFF, 0x01, 0x04, 0x40, 0x00, 0xF0, 0xF0, 0x00},
 {0xFF, 0x03, 0x04, 0x40, 0x10, 0xF1, 0xF0, 0x0F},
 {0xFF, 0x13, 0x06, 0x40, 0x18, 0xF1, 0xF0, 0x0F},
 {0xFF, 0x23, 0x06, 0x40, 0x9C, 0xF1, 0xF0, 0x0F},
 {0xFF, 0x63, 0x0A, 0x60, 0x9E, 0xF1, 0xF0, 0x0F},
 {0xFF, 0xFF, 0x0A, 0x60, 0x9F, 0xF1, 0xF0, 0x0F},
 {0xFF, 0xFF, 0x1B, 0xE0, 0x9F, 0xF1, 0xF0, 0x0F},
 {0xFF, 0xFF, 0x7B, 0xE0, 0x9F, 0xF1, 0xF0, 0x0F},
 {0xFF, 0xFF, 0xFB, 0xE0, 0x9F, 0xF1, 0xF0, 0x0F},
 {0xFF, 0xFF, 0xFB, 0xE0, 0xEF, 0xF1, 0xF0, 0x0F},
 {0xFF, 0xFF, 0xFB, 0xFF, 0xEF, 0xF1, 0xF0, 0x0F},
},
  //DOWN
{
 {0xFF, 0xFF, 0xFB, 0xFF, 0xEF, 0xF1, 0xF0, 0x0F},
 {0xFF, 0xFF, 0xFB, 0xE0, 0xEF, 0xF1, 0xF0, 0x0F},
 {0xFF, 0xFF, 0xFB, 0xE0, 0x9F, 0xF1, 0xF0, 0x0F},
 {0xFF, 0xFF, 0x7B, 0xE0, 0x9F, 0xF1, 0xF0, 0x0F},
 {0xFF, 0xFF, 0x1B, 0xE0, 0x9F, 0xF1, 0xF0, 0x0F},
 {0xFF, 0xFF, 0x0A, 0x60, 0x9F, 0xF1, 0xF0, 0x0F},
 {0xFF, 0x63, 0x0A, 0x60, 0x9E, 0xF1, 0xF0, 0x0F},
 {0xFF, 0x23, 0x06, 0x40, 0x9C, 0xF1, 0xF0, 0x0F},
 {0xFF, 0x13, 0x06, 0x40, 0x18, 0xF1, 0xF0, 0x0F},
 {0xFF, 0x03, 0x04, 0x40, 0x10, 0xF1, 0xF0, 0x0F},
 {0xFF, 0x01, 0x04, 0x40, 0x00, 0xF0, 0xF0, 0x00},
 {0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00},
 {0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00},
 {0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
},
  // Top Space Rotate
{
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00},
 },
   //Spinner attract
{
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00},
 },
 };



void ShowLampAnimation(byte animationNum, unsigned long divisor, unsigned long baseTime, byte subOffset, boolean dim, boolean reverse = false, byte keepLampOn = 99) {
  if (animationNum>=NUM_LAMP_ANIMATIONS) return;
  
  byte currentStep = (baseTime / divisor) % LAMP_ANIMATION_STEPS;
  if (reverse) currentStep = (LAMP_ANIMATION_STEPS - 1) - currentStep;

  byte curBitmask;
  byte *currentLampOffsetByte = LampAnimations[animationNum][(currentStep + subOffset) % LAMP_ANIMATION_STEPS];
  byte *currentLampByte = LampAnimations[animationNum][currentStep];

  byte lampNum = 0;
  for (int byteNum = 0; byteNum < NUM_LAMP_ANIMATION_BYTES; byteNum++) {
    curBitmask = 0x01;
    
    for (byte bitNum = 0; bitNum < 8; bitNum++) {

      byte lampOn = false;
      lampOn = (*currentLampByte) & curBitmask;

      // if there's a subOffset, turn off lights at that offset
      if (subOffset) {
        byte lampOff = true;
        lampOff = (*currentLampOffsetByte) & curBitmask;
        if (lampOff && lampNum != keepLampOn && !lampOn) RPU_SetLampState(lampNum, 0);
      }

      if (lampOn) RPU_SetLampState(lampNum, 1, dim);

      curBitmask *= 2;
      lampNum += 1;
    }
    currentLampByte += 1;
    currentLampOffsetByte += 1;
  }
}


void ShowLampAnimationSingleStep(byte animationNum, byte currentStep, byte *lampsToAvoid = NULL) {
  if (animationNum>=NUM_LAMP_ANIMATIONS) return;
  
  byte lampNum = 0;
  byte *currentLampByte = LampAnimations[animationNum][currentStep];
  byte *currentAvoidByte = lampsToAvoid;
  byte curBitmask;
  
  for (int byteNum = 0; byteNum < NUM_LAMP_ANIMATION_BYTES; byteNum++) {
    curBitmask = 0x01;
    for (byte bitNum = 0; bitNum < 8; bitNum++) {

      boolean avoidLamp = false;
      if (currentAvoidByte!=NULL) {
        if ((*currentAvoidByte) & curBitmask) avoidLamp = true;
      }

      if (!avoidLamp /*&& (*currentLampByte)&curBitmask*/) RPU_SetLampState(lampNum, (*currentLampByte)&curBitmask);

      lampNum += 1;
      curBitmask *= 2;
    }
    currentLampByte += 1;
    if (currentAvoidByte!=NULL) currentAvoidByte += 1;
  }
}




#define LAMP_ANIMATIONS_H
#endif

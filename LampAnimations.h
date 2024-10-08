#ifndef LAMP_ANIMATIONS_H

#include "RPU_Config.h"
#include "RPU.h"

// This file can define a series of animations, stored 
// with each lamp as a bit in the following array.
// Lamp 0 = the first bit of the first byte, so "{0x01," below.
// The animations can be played with either of the helper functions
// at the bottom of this file.

// Lamp animation arrays
#define NUM_LAMP_ANIMATIONS       5
#define LAMP_ANIMATION_STEPS      16
#define NUM_LAMP_ANIMATION_BYTES  8
byte LampAnimations[NUM_LAMP_ANIMATIONS][LAMP_ANIMATION_STEPS][NUM_LAMP_ANIMATION_BYTES] = {
  //UP 0
{
 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0xC0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00},
 {0x00, 0x00, 0x04, 0x40, 0x00, 0xF0, 0xE0, 0x00},
 {0x00, 0x02, 0x00, 0x00, 0x10, 0x01, 0x00, 0x0F},
 {0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x00, 0x00},
 {0x00, 0x30, 0x00, 0x20, 0x80, 0x00, 0x00, 0x00},
 {0x00, 0xC0, 0x08, 0x00, 0x02, 0x00, 0x00, 0x00},
 {0x00, 0x0C, 0x01, 0x80, 0x01, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x80, 0x00, 0x20, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00},
},
  //Spiral 1
{
 {0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00},
 {0x00, 0x00, 0x00, 0x40, 0x00, 0x04, 0xE0, 0x00},
 {0x00, 0x00, 0xF0, 0x00, 0x1F, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x1F, 0x60, 0x00, 0x00, 0x00},
 {0x00, 0x02, 0x0E, 0xA0, 0x80, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0xF1, 0x00, 0x0F},
 {0x00, 0xFC, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
 },
  // Search right 2
{
 {0x01, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00},
 {0x4B, 0x00, 0x00, 0x40, 0x10, 0x04, 0x00, 0x00},
 {0x4B, 0x00, 0x00, 0x40, 0x18, 0x14, 0x00, 0x00},
 {0x4B, 0x00, 0x00, 0x40, 0x1C, 0x14, 0x00, 0x00},
 {0x4B, 0x00, 0x00, 0x40, 0x1E, 0x14, 0x00, 0x00},
 {0x4B, 0x00, 0x00, 0x40, 0x1F, 0x14, 0x00, 0x01},
 {0xCB, 0x00, 0xF0, 0x40, 0x1F, 0x34, 0x20, 0x01},
 {0xCB, 0x08, 0xF0, 0x41, 0x1F, 0x34, 0x20, 0x03},
 {0xDB, 0xF8, 0xF1, 0x47, 0x3F, 0x34, 0x60, 0x07},
 {0xDB, 0xFC, 0xF1, 0x4F, 0x3F, 0x34, 0x60, 0x0F},
 {0xDB, 0xFD, 0xF1, 0x5F, 0x3F, 0x74, 0xE0, 0x0F},
 {0xDB, 0xFD, 0xF9, 0x7F, 0x3F, 0x75, 0xE0, 0x0F},
 {0xDB, 0xFD, 0xF9, 0x7F, 0xBF, 0x75, 0xE0, 0x0F},
 {0xDB, 0xFD, 0xFB, 0x7F, 0xBF, 0xF5, 0xE0, 0x0F},
 {0xFF, 0xFF, 0xFB, 0x7F, 0xBF, 0xF5, 0xF0, 0x0F},
 {0xFF, 0xFF, 0xFF, 0x7F, 0xBF, 0xF5, 0xF0, 0x0F},
},
  // Top Space Rotate 3
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
   //Spinner rolling 4
{
 {0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
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

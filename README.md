# Space Rider (Harlem Globe trotters Gieger retheme)
Early solid state pinball implementation
ToDo list:
* Change skill shot to 30 second timer - TJS 7/18 Implemented
* New callout added for when a goal is achieved and its the the third (extraball) or the last goal (wizard mode start). So we need to identify the three possible machine states when a goal is achieved. Regular, Third, and Final Jim 7/18 - Complete testing now

* I found a scenario where the center saucer did not eject ball. I think if Blast off was the final goal to complete. Need to find again - TJS 7/19 Think it's fixed
* Bonus needs to reset when collected. This is a problem if the Bonus is the final goal to get to wizard mode as its not collected so when you drop out of wizard mode bonus is still lit at 40 and counts toward the wizard mode again. - TJS 7/19 Think it's fixed
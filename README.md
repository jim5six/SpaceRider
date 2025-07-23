# Space Rider (Harlem Globe trotters Gieger retheme)
Early solid state pinball implementation
ToDo list:
* 7/21 - Skill shot does not stop lampanimation until saucer kicks ball out. No celebration happens. Also artifact lights are remaining after skill shot is over(This is new because I added lights to the skill shot lampanimation to better indicate what goal counts would be saved).

* 7/23 - No default blastoff light after skill shot - TJS need to replace turning off all lights with individual turn-on logic
* 7/23 - ball save on after wizard mode (returning to game). OK not really... The lights blink but once first switch hit is made its off. I guess its accounting for a switch hit is required to consider the ball is on the playfield, so this is fine i think...
* 7/23  - said skill shot missed and spinner count held at the same time on S skill shot
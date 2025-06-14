# Space Rider (Harlem Globe trotters Gieger retheme)
Early solid state pinball implementation
ToDo list:
* Fix display countdown timers (can they be assigned to individual displays so they dont fight each other?) Jim - 6/12 The Blast-off timer is still not showing on displays
* Add hurry up background sound while timers are running. Jim- 6/12 Blast-off still an issue. I cannot find what is causing the wrong sound to be played, this one is strange)
* If all goals are commpleted in a game ( (S)Super spinner, (P)Pop bumper Frenzy, (A)Blast off collect, (C)Max bonus (E)5x playfield). Drop out of game, kill coils and drain. Start Wizard mode where everything is 10X playfield for 45 seconds or a minute. ends on drain or time expire and resets SPACE letters and lights the DRop_Target Special lamp, hitting the right scoop wins free game or special value. This needs a wizard mode background song, "Start Wizard Mode" call out, and "End Wizard Mode" call out. If any 3 goals are completed light the extra ball at Right target, has light and sound call out is already there.
* Figure out what to do with the standup targets complete. Jim - 6/12 if target bank is complete(all lit), right inlane resets(all unlit). If you light the targets second time light extra ball at right target, has call out already.  
* Something to give an extra ball. Jim - 6/12 Any 3 goals completed, and complete target bank twice(original HGT code)
* Only reset drop targets if you have to
* Skill shot SPACE letter holds on any switch hit. Jim - 6/12 Below would fix this if we asing it a state at the start of unstructured play or end of skill shot mode.
* What to do with the SPACE letters after skill shot phase? Jim - 6/12 create a space toggle that lites the SPACE letters ON/OFF and assign it to pops and slings (coil fires). If center scoop hit when lit 5000 points if scoop hit unlit 1000 points.
* Hurry up music ends randomly if you start multiple modes


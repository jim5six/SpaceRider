# Space Rider (Harlem Globe trotters Gieger retheme)
Early solid state pinball implementation
ToDo list:
* If all goals are commpleted in a game ( (S)Super spinner, (P)Pop bumper Frenzy, (A)Blast off collect, (C)Max bonus (E)5x playfield). Drop out of game, kill coils and drain. Start Wizard mode. Turn on all the shot lights blinking fast, Stand-up targets, top space letters, pop bumpers, all the left spinner lights, top 2 center spinner lights, Right target Special, and DRop targets special light. The player has to hit every shot to turn out the lights. 5 Stand-up Targets one hit each, pops one hit each, Left Spinner 10 spins each light(40 spins), Either Center spinner 10 spins each light (20 spins), All 4 Drop Targets, Right Stand-up Target once. Hit Right Saucer to score big jackpot of 250,000 points)and ends the Wizard Mode. Also ends on drain or time expire (1 minute?) and returns the game back to the ball in play with everything reset. LampAnimation for Wizard Mode is #6, (08) Wizard mode background song, (335) Start Wizard Mode call out, (333) Wizard Complete call out (Jackpot collected), (334) Wizard Mode Failed call out (drain or time expire).
* Made easier - 100 center spins. 25 pop hits. Added more shots to add bonus. 
* Bonus should not countdown and award points for start of wizard mode - TJS 7/15 done, needs testing
* Should not build bonus during wizard mode, so should not have bonus to countdown, but it does. looks like its holdover from the last ball.
* Remove skillshot when exiting wizard mode - TJS 7/15 done, needs testing
* Bonus should be where it was when you entered wizard mode when you exit wizard mode - TJS 7/15 done, needs testing
* The skillshot celebration blink is gone... - TJS 7/15 done, needs testing
* POPs and Spinners not working in wizard mode. Its not just a visible thing as I cannot "win" wizard mode even if I am sure i hit everything i should. left and right target lights, center saucer lights, and drop target 4 lights are working as they should so I think they are working and counting (hopefully).
* Left spinner lamps during wizard mode are not woking the 200 light anyway never is on. The game doesn't understand "3/4" I think. I changed to .75 and it works now. - TJS yeah 3/4 was actually evaluating to zero, I tried to be clever there and forgot how programming works
* Lock in permanent 100, 200 300 bonus per player
* Change skill shot to 30 second timer

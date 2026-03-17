<!--
This is replacement code for Geiger Space Rider, a German retheme of Harlem Globetrotters.
It is provided WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

RPU framework by Dick Hamill
Space Rider design by Jim Martin
Software by Trevor Seyfried and Jim Martin
Sounds can be found here: https://drive.google.com/file/d/14YrbskdDLZMBf4jz1WutmT2DQ0Eio7wl/view?usp=sharing
in a zip file. Unzip and load files on the SD card.
-->



#  Custom Ruleset — Geiger Space Rider
Version: **1.0.1**

## Primary Objective
Complete all five **S-P-A-C-E** goals to activate **Station Shutdown Wizard Mode**.  
Once in wizard mode, hit lit shots to shut down the station and score the Wizard Mode jackpot.

---

## S-P-A-C-E Goals

### S — Super Spinner
- Spin the **left spinner** **200 times**.
- Completion activates **Super Spinner** for **30 seconds**.
- Completing Super Spinner awards the **S** letter.

---

### P — Super Pops
- Achieve **25 pop bumper hits**.
- Completion starts **Super Pops** for **30 seconds**.
- Completing Super Pops awards the **P** letter.

---

### A — Super Blast Off
- Spin the **lit center spinner** **100 times** to fuel the rocket.
- When fully fueled:
  - **Super Blast Off** is qualified.
- Hit the **center saucer** to score the **Blast Off Jackpot**.
- Successful Blast Off awards the **A** letter.

---

### C — Bonus Max
- Build bonus to the maximum value of **40,000**.
- Reaching max bonus awards the **C** letter.

---

### E — Playfield Multiplier Max
- Increase the playfield multiplier to **5×**.
- Reaching 5× awards the **E** letter.

---

## Wizard Mode — Station Shutdown

- Station Shutdown Wizard Mode begins immediately after all five **S-P-A-C-E** goals are completed.
- All station lights are on at the start of the mode.
- Each **S-P-A-C-E** objective must be completed **again**.
- Completing an objective during wizard mode turns off its corresponding station light.
- Wizard mode ends when all station lights are turned off.

---

## Skill Shot

- Available for **30 seconds** after plunge.
- Shoot the **center saucer** while one of the **upper SPACE letters** is lit to earn a **holdover award** for the next ball.

### Skill Shot Holdover Awards
- **S** — Hold spinner count
- **P** — Hold pop bumper count
- **A** — Hold Super Blast Off progress
- **C** — Hold bonus value
- **E** — Hold Playfield Multiplier

- The skill shot can be missed if the saucer is hit when no letter is lit.

---

## Playfield Multiplier

- Increased by hitting the **right saucer**.
- Multiplier progression:
  - 2× → 3× → 5×
- Multiplier resets at the start of the next ball unless held via skill shot.

---

## Bonus System

- Bonus is built from various playfield switches.
- Super bonus hold thresholds at:
  - 10,000
  - 20,000
  - 30,000
- If bonus reaches the maximum **40,000**, all super bonus holds are lost.

---

## Inline Drop Targets

- **First target**: Lights right target for **5,000 points**.
- **Second target**: Lights **OPEN GATE** at the right target.
- **Third target**: Lights left outlane at the **25,000 points**.

---

## Extra Balls

- Complete the **left target bank twice** to light **EXTRA BALL** at the right target.
- Complete **three S-P-A-C-E goals** to light **EXTRA BALL** at the right target.

---

# Stall Ball
This code comes with a built-in **Stall Ball** mode that can be enabled from the operator menu!

## Enabling Stall Ball

- Enable Stall Ball play by setting the #31 operator setting (Stall Ball Enabled) to **1**.
- When Stall Ball Enaled is 1, starting a new game will enter Stall Ball mode.

## Stall Ball Rules

- Every time the ball lands in a saucer, the game will deliver a custom callout telling the current player to rotate out.
- Only one player scoreboard is eabled and will start at 200 and count down switch hits.
- If the ball drains, the current player is out and the ball will automatically reload.
- After 200 switch hits, the inline drop targets will reset. Player 1 score shows how many switch hits remain.
- Hitting the Second Drop Target will open the right gate for a ball save.
- Hitting the Fourth Drop Target will award an "Extra Ball". (we use this to bring back players that have been eliminated in team stall ball).  
- Stall Ball mode will go on forever until the game is turned off, or Stall Ball Enabled is set to **0**.

# Notes

- The #30 operator setting (Hard Mode) is currently unused.
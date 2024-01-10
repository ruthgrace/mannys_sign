# mannys_sign

## developer setup

1. install PlatformIO for VSCode: https://platformio.org/install/ide?install=vscode
2. Install FastLED library: Download code here as zip: https://github.com/FastLED/FastLED/releases , unzip and paste the FastLED-3.6.0 folder into the lib folder of this project. The version of the FastLED library I'm using is 3.6.0
3. Install WiFi library: Download code here as zip https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi , unzip and paste the WiFi library folder (https://github.com/espressif/arduino-esp32/tree/master/libraries) into the lib folder of this project. The version of the WiFi library I'm using is 2.0.0

### build / upload

find the bug face shaped platformio icon on the left side of your VS Code. Under Project Tasks > hiletgo-esp32 > General. click Build

### upload

1. plug board into computer (usb to usb micro)
2. Looking at the board with the Espressif right side up on the chip, press and hold the button on the bottom right, beside the power jack on the board. Keep holding down while you do step 3
3. find the bug face shaped platformio icon on the left side of your VS Code. Under Project Tasks > hiletgo-esp32 > General. click Upload

## hardware

I'm using an ESP32 from HiLetGo: https://www.amazon.com/gp/product/B0718T232Z/
And a breakout board from KeeYees: https://www.amazon.com/dp/B09PYW1HC9

### installation

#### steps

1. make sure that wifi password is updated correctly in code
2. plug just the power panel in, monitor with laptop, ensure that wifi connection is successful (you can do this in the restaurant before installing the sign outside)
3. Check if box needs to be weather proofed/caulked on the back. Do it first if so.
4. power panel goes behind ELECTION COUNTDOWN panel. Plug in ELECTION COUNTDOWN PANEL first. Add clips to attach next panel. Place and plug other panels in one by one (adding clips each time), leaving gaps between panels to pull the power cords through and plug them into the power panel. When the last panel is plugged in, close remaining gaps by attaching clips between panels
5. weather proof the box - use acrylic panels and caulking to cover the box

#### panel plugs
There are 3 power supplies. The top powers 3 panels, the middle powers 4 panels, and the bottom one powers 2 panels. From top to bottom this is the ideal arrangement of panels being plugged in to distribute power:

First power supply | Pin name in code

* 88 (seconds) | S1
* 88 (minutes) | S2
* hours | S3

Second power supply | Pin name in code

* days | S4
* seconds | S5
* 88 (hours) | S6
* ELECTION COUNTDOWN | S7

Third power supply | Pin name in code

* 888 | S8
* minutes | S9

### Miscellaneous build notes
* use syringe with isopropyl alcohol to remove hot glue
* there are 3d printed clips holding down the aluminum backed LEDs. Some are 1mm and some are 2mm, required because of wood warping.
* Screws:
  * MCx6mm for the wooden board where you don't want it to go through the plexiglass
  * MCx10mm for the power board (thicker wood piece, no plexiglass)
  * MCx16mm to go through the plexiglass and wooden board, into 3d printed nuts

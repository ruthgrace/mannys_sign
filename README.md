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
There are 3 power supplies. The top powers 3 panels, the middle powers 4 panels, and the bottom one powers 2 panels. From top to bottom this is the ideal arrangement of panels being plugged in to distribute power:

First power supply

* 88
* 88
* hours

Second power supply

* days
* seconds
* 88
* ELECTION COUNTDOWN

Third power supply

* 888
* minutes

### Miscellaneous build notes
* use syringe with isopropyl alcohol to remove hot glue
* there are 3d printed clips holding down the aluminum backed LEDs. Some are 1mm and some are 2mm, required because of wood warping.
* Screws:
  * MCx6mm for the wooden board where you don't want it to go through the plexiglass
  * MCx10mm for the power board (thicker wood piece, no plexiglass)
  * MCx16mm to go through the plexiglass and wooden board, into 3d printed nuts

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

# mannys_sign

## developer setup

1. install arduino if you don't already have it. I am using Arduino IDE 2.1.0
2. Install FastLED library: in Arduino IDE, Tools > Manage Libraries > search for FastLED and select Install for "FastLED by Daniel Garcia". I'm installing version 3.6.0.
3. Install WiFi library: Download from https://www.arduino.cc/reference/en/libraries/wifi/ - I am using version 1.2.7. Leave the file zipped and add it to the project directory (I've added it to the .gitignore). To install: Sketch > Include Library > Add .ZIP Library

## hardware

I'm using an NodeMCU ESP-12E from HiLetGo: https://www.amazon.com/gp/product/B010O1G1ES/

To hook it up to the Arduino IDE, I plugged the microcontroller into my windows laptop (running Windows 11), and told the Arduino IDE that the device plugged into the USB port (Com 3 in my case) was a Node MCU 1.0 (ESP-12E)

# ArduOpenDSKY
Software for the Arduino Based Open DKSY  https://opendsky.com

OpenDSKY is a trademark of S&T GeoTronics LLC

This is designed to run on OpenDSKy hardware, it allows multiple programs to be
run at the same time, though only one program can be in the foreground and thus
in control of what is displayed on the 7 segment displays.


## Currently Supported Modules
The following parts of the OpenDSKY are currently supported
- Keyboard (physical, and via serial input)
- Inertial Measurement Unit (IMU)
- Global Positioning System (GPS)
- Bulbs (Both RGB and RGBW neopixels)
- Real Time Clock (RTC)


## Not Currently Supported Modules
- Sound
- 7 Segment Displays ( though simulation of the 7 Segment display is available via the serial port)

# Development environment
This project has been developed with PlatformIO  - see https://platformio.org/get-started for more information on installing platformIO.

# Required libraries
The following libraries are required:

- TinyGPSPlus by Mikal Hart, this should be available from here https://github.com/mikalhart/TinyGPSPlus
- digitalWriteFast by Watterott electronic, this should be available from here https://github.com/watterott/Arduino-Libs/tree/master/digitalWriteFast
- I2Cdevlib-MPU6050 library by Jeff Rowberg, this should be available from here https://github.com/jrowberg/i2cdevlib
- Adafruit Neopixel  by Adafruit, this should be available from here https://github.com/adafruit/Adafruit_NeoPixel
- TimerOne by Stoyko Dimitrov, this should be available from here https://github.com/PaulStoffregen/TimerOne
- RTC by Michael Miller, this should be available from here https://github.com/Makuna/Rtc
- LedControl by Eberhard Fahle, this should be available from here https://github.com/wayoda/LedControl
- Thread by Ivan Seidel, this should be available from here https://github.com/ivanseidel/ArduinoThread

All of the above libraries can be installed by the platformIO library manager.

## Calling up a program
Items in the program table are called up using at least a verb, and optionally a noun and some data. To try out your hardware, try pressing
**Verb 3 5 Enter** on the OpenDSKY keyboard, this should then start the bulb and 7 segment display test.


## Adding your own programs
1. Add a new entry to the ProgramTable in Program.cpp . As a minimum this should at least contain a VProgramNumber (verb number) and a program. In the example below the V37N36SetAlarmProg program will called when verb 37 and noun 36 are entered, it can accept data via the V37N36GiveData function, and can provide data for the 7 segment display via the TimeGetDisplayData function. By default the program is not running, and anything that is NULL means that this program does not support that feature.
```
{ 37,  36, &V37N36SetAlarmProg, NULL,  NULL, &V37N36GiveData, &TimeGetDisplayData, NOT_RUNNING}, /* Set Alarm Program */}
```
2. Create the functions you have just added to the program table. For examples, see TimeProgram.cpp

# Useful links
- This contains some really useful information on the verbs nouns and programs in the real Apollo DSKY https://www.ibiblio.org/apollo/ForDummies.html
- Open DSKY hardware page  https://opendsky.com

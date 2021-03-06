# ArduOpenDSKY
Software for the Arduino Based Open DKSY  

OpenDSKY is a trademark of S&T GeoTronics LLC  https://opendsky.com

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
- Sound
- 7 Segment Displays 

For help with board bringup see the wiki link at the top of the github page.

# Development environment
This project has been developed with PlatformIO  - see https://platformio.org/get-started for more information on installing platformIO.
If you want to use the arduino development environment instead, then rename the src folder to ArduOpenDSKY and it will be possible to open
the .ino file and compile/download with the Arduino IDE.

# Required libraries
The following libraries for arduino are required:

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

## Supported Programs

Name |Description|Verb|Noun|Data Entry
------|-----------|----|----|------
IMU Gyro| Shows to IMU Gyro Data on the 7 segment Display|16|29|Not Used
IMU Accelerometer | Shows to IMU Accelerometer Data on the 7 segment Display|16|30|Not Used
Current time | Shows the current time from the RTC on the 7 segment Display|16|36|Not Used
GPS Position | Shows the openDSKy Latitude (in degrees) Longitude (in degrees) and Altitude (in feet) on the 7 segment Display|16|43| Not Used
Time To/From Launch  | Shows the time to or from launch on the 7 segment Display|16|65| Not Used
Launch parameters  | Shows an approximation of accel, velocity and altitude during launch on the 7 segment Display|16|62| Not Used
Play Sound | Plays the specified sound | 21 | 98 | Sound to play (0 to 2999) **Enter**
Load Time Manually | Manually set the RTC time via the keypad | 25 | 36| Hour (0 to 23) **Enter** Minute (0 to 59) **Enter** Second (0 to 59) **Enter**
Load Time From GPS | Sets the RTC time from the GPS, the data entry phase allows an offset from GPS to be used, to help with daylight saving time / timezones | 25 |26 | Offset (-24 to 24) **Enter**
Bring To Foreground| Brings the program specified in the data entry phase to the Foreground | 30 | Not Used | Verb Number **Enter**  Noun Number **Enter**
Reset| Resets the program running in the foreground | 32 | Not Used | Not Used
Terminate| Terminates the program running in the foreground | 34 | Not Used | Not Used
Bulb Test| Tests all of the Neopixels and 7 Segments| 35 | Not Used | Not Used
Launch **Coming Soon**| Initiates the launch program, plays a sound at 10 seconds, counts down and launche s, then simulates the velocity, acceleration and altitude|37|02| Seconds until lift off ** Enter** 
Set Alarm Program| Set an alarm time, and a verb noun to call when the alarm goes off|37|36| day (0-31 0 is today) **Enter** Hour (0 to 23) **Enter** Minute (0 to 59) **Enter** Second (0 to 59) **Enter** Verb **Enter** Noun **Enter** Data **Enter** Repeat **Enter**

## Adding your own programs
1. Add a new entry to the ProgramTable in Program.cpp . As a minimum this should at least contain a VProgramNumber (verb number) and a program. In the example below the V37N36SetAlarmProg program will called when verb 37 and noun 36 are entered, it can accept data via the V37N36GiveData function, and can provide data for the 7 segment display via the TimeGetDisplayData function. By default the program is not running, and anything that is NULL means that this program does not support that feature.
```
{ 37,  36, &V37N36SetAlarmProg, &V37N36GiveData, &TimeGetDisplayData, NOT_RUNNING}, /* Set Alarm Program */}
```
The noun number is optional. The program function is called when the verb and noun are entered. If the program should be called repeatedly, then you will need to setup a thread and a callback function inside the program function. The set data function is used to give numerical data to the function. The Get Disp Data Function Should return display data for the 7 segment display driver. Run State is used internally by the Program module to track the state of all of the current programs.


2. Create the functions you have just added to the program table. For examples, see TimeProgram.cpp, here is an example that creates a thread that calls the P16ThreadCallback function every 333ms. It then tells the program handler that it is running in the foreground.

```
ProgramRunStateEnum V16N36ShowRTCTime(ProgramCallStateEnum call)
{
		switch(call)
		{

		case RESET_PROGRAM:
		case START_PROGRAM:
		case BRING_PROGRAM_TO_FOREGROUND:
				/* Check if we need to create a thread and callback so as to be
				   able to periodically update the displayed time */
				if(P16Thread == NULL)
				{
						/* Create a new thread and set it to update three times a second,
						   you could get away with only updating the data once a second, but
						   doing it more often makes it appear smoother to the user. */
						P16Thread = new Thread();
						P16Thread->onRun(P16ThreadCallback);
						P16Thread->setInterval(333);
						controll.add(P16Thread);
						return FOREGROUND;
				}

				break;

		/* this program only runs in FOREGROUND */
		case PAUSE_PROGRAM:
		case UNPAUSE_PROGRAM:
		case PUSH_PROGRAM_TO_BACKGROUND:
		case STOP_PROGRAM:
		case NUM_CALL_STATES:
		default:
				/* If the thread exists, delete it */
				if(P16Thread != NULL)
				{
						controll.remove(P16Thread);
						delete[] P16Thread;
						P16Thread = NULL;
				}
				return NOT_RUNNING;
				break;

		}
		return NOT_RUNNING;
}
```

# Useful links
- This contains some really useful information on the verbs nouns and programs in the real Apollo DSKY https://www.ibiblio.org/apollo/ForDummies.html
- Open DSKY hardware page  https://opendsky.com
- NASA have made some sound files available https://www.nasa.gov/connect/sounds/index.html

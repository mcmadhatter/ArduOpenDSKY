/*
 * ArduinoOpenDSKY.ino
 *
 * This file is part of ArduOpenDSKY - the open source software for
 * the OpenDSKY.
 *
 * Main function and frame loop handler. Do not add anything else to loop
 *
 * Copyright 2018 Jeffrey Marten Gillmor
 *
 * OpenDSKY is a trademark of S&T GeoTronics LLC  https://opendsky.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * (GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* Arduino libraries */
#include <Arduino.h>
#include "Thread.h"
#include "ThreadController.h"
#include <TimerOne.h>
#include <Wire.h>
#include <helper_3dmath.h>

/* Open DSKY headers */
#include "main.h"
#include "Neopixels.h"
#include "IMU.h"
#include "SevenSegment.h"
#include "RTC.h"
#include "Keyboard.h"
#include "Program.h"
#include "GPS.h"
#include "Sound.h"
#include "Launch.h"


/* ThreadController that will controll all threads */
ThreadController controll = ThreadController();

/* Per hardware diver setup */
void setup()
{
		Wire.begin();
		Serial.begin(9600); /* If you don't have a GPS chip, consider making this 115200 */
		NeoPixelSetup();
		SevenSegmentSetup();
		IMUSetup();
		RTCSetup();
		KeyboardSetup();
		GPSSetup();
		SoundSetup();

		/* Program Setup functions */
		LaunchSetup();
}


/* Don't add anything else to the loop, instead use a thread */
void loop()
{

		controll.run();
		ProgramTask(); /* Program Task shedules handles all the programs, it also acts as the
	                      idle task, nothing else should be added to the loop(); */
}

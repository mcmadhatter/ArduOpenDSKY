/*
 * Launch.cpp
 *
 * This file is part of ArduOpenDSKY - the open source software for
 * the OpenDSKY.
 *
 * Software Copyright 2018 Jeffrey Marten Gillmor
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

 #include <Wire.h>
 #include <Thread.h>
 #include <ThreadController.h>
 #include <TimerOne.h>


/* Open DSKY headers */
 #include "SevenSegment.h"
 #include "Neopixels.h"
 #include "Program.h"
 #include "Sound.h"
 #include "RTC.h"
 #include "GPS.h"
#include "Launch.h"
 #include "DiagnosticPrograms.h"
 #include "main.h"

static SevenSegmentDisplayStruct LaunchDisplayData;
static RtcDateTime LaunchTime;

Thread* V37N02Thread = NULL;
Thread* V16N65Thread = NULL;

SevenSegmentDisplayStruct* LaunchGetDisplayData(void)
{
		return &LaunchDisplayData;
}


/**
 * Whilst P02 is running, this shows the countdown, after launch it shows an
 * exceptionally crude (and poor) approximation of the speed, acceleration and
 * height of the rocket first stage burn.
 */
void V37N02ThreadCallback(void)
{
		static uint32_t lastMillis = millis();
		static float velocity = 0.0;
		static float accel = 0.0;
		static float accelFactor = 0.06;
		static float height = 0.0;


		RtcDateTime ActionTime = LaunchTime;
		RtcDateTime NowTime = RTCDateTimeReturnNow();
		ActionTime -= 10;


		if(LaunchTime.TotalSeconds() > NowTime.TotalSeconds())
		{
				Serial.println("Count Down");
				RtcDateTime ClockTime = RtcDateTime( LaunchTime.TotalSeconds() - NowTime.TotalSeconds());
				LaunchDisplayData.R1 = (int32_t)ClockTime.Hour() * -1;
				LaunchDisplayData.R2 = (int32_t)ClockTime.Minute()* -1;
				LaunchDisplayData.R3 = ((int32_t)ClockTime.Second() * -100 ) - ((100 - millis())%100);
				lastMillis = millis();
				velocity = 0.0;
				accel = 0.0;
				LaunchDisplayData.Verb = 75;
				LaunchDisplayData.Noun = 00;
				LaunchDisplayData.Prog = 02;

				if((LaunchTime.TotalSeconds() - 6) == NowTime.TotalSeconds() )
				{

						PlayTrack(COUNTDOWN);
						Serial.println("T -6");


				}
		}
		else if(LaunchTime.TotalSeconds() == NowTime.TotalSeconds())
		{
				velocity = 0.0;
				accel = 4.0;
				Serial.println("Lift Off");
		}
		else
		{

				velocity += accel/5.0;
				accel += accelFactor;
				height += velocity/121.52;


				if((NowTime.TotalSeconds() - LaunchTime.TotalSeconds()) < 5)
				{
						accelFactor = 1.8;
				}
				else if((NowTime.TotalSeconds() - LaunchTime.TotalSeconds()) < 80)
				{
						accelFactor = 0.06;
				}
				else if((NowTime.TotalSeconds() - LaunchTime.TotalSeconds()) < 125)
				{
						accelFactor = 0.18;
				}
				else if((NowTime.TotalSeconds() - LaunchTime.TotalSeconds()) == 125)
				{
						accel = 93.0;
				}
				else if((NowTime.TotalSeconds() - LaunchTime.TotalSeconds()) >= 160)
				{
						accel = 0;
						accelFactor = 0;
				}
				else
				{
						/* do nothing */
				}

				LaunchDisplayData.R1 = (int32_t)velocity;
				LaunchDisplayData.R2 = (int32_t)accel;
				LaunchDisplayData.R3 = (int32_t)height;
				LaunchDisplayData.Prog = 11;
				LaunchDisplayData.Verb = 06;
				LaunchDisplayData.Noun = 62;

		}
		LaunchDisplayData.R1DigitShowMask = 0x3F;
		LaunchDisplayData.R2DigitShowMask = 0x3F;
		LaunchDisplayData.R3DigitShowMask = 0x3F;
}

/**
 * Give the time in seconds until lift off
 * @param  dataIdx not used;
 * @param  data    time in seconds until lift off
 * @return         FOREGROUND always;
 */
ProgramRunStateEnum V37N02GiveData(uint8_t dataIdx,int32_t data)
{
		LaunchTime =  RTCDateTimeReturnNow();
		LaunchTime += (uint32_t)(data);

		Serial.print("Now: ");
		Serial.print(RTCDateTimeReturnNow().Hour());
		Serial.print(":");
		Serial.print(RTCDateTimeReturnNow().Minute());
		Serial.print(":");
		Serial.println(RTCDateTimeReturnNow().Second());

		Serial.print("Launch time: ");
		Serial.print(LaunchTime.Hour());
		Serial.print(":");
		Serial.print(LaunchTime.Minute());
		Serial.print(":");
		Serial.println(LaunchTime.Second());
		return FOREGROUND;
}


ProgramRunStateEnum V37N02Launch(ProgramCallStateEnum call)
{
		switch(call)
		{
		case START_PROGRAM:
		case RESET_PROGRAM:
		case BRING_PROGRAM_TO_FOREGROUND:
				/* Clear out the display data ready for input */
				LaunchDisplayData.R1 = 0;
				LaunchDisplayData.R2 = 0;
				LaunchDisplayData.R3 = 0;
				LaunchDisplayData.Verb = 75;
				LaunchDisplayData.Noun = 00;
				LaunchDisplayData.Prog = 02;
				LaunchDisplayData.R1DigitShowMask = 0x3F;
				LaunchDisplayData.R2DigitShowMask = 0x3F;
				LaunchDisplayData.R3DigitShowMask = 0x3F;


				/* Check if the thread already exists before we try to create it. */
				if(V37N02Thread == NULL)
				{
						V37N02Thread = new Thread();
						V37N02Thread->onRun(V37N02ThreadCallback);
						V37N02Thread->setInterval(200);
						controll.add(V37N02Thread);
				}

				return FOREGROUND;
				break;

		case PAUSE_PROGRAM:

				if(V37N02Thread != NULL)
				{
						V37N02Thread->onRun(ThreadDoNothing);
				}
				return BACKGROUND;
				break;


		case PUSH_PROGRAM_TO_BACKGROUND:

				return BACKGROUND;
				break;


		case UNPAUSE_PROGRAM:
				if(V37N02Thread != NULL)
				{

						V37N02Thread->onRun(V37N02ThreadCallback);
				}
				return FOREGROUND;
				break;

		case STOP_PROGRAM:
		case NUM_CALL_STATES:
		default:

				if(V37N02Thread != NULL)
				{
						controll.remove(V37N02Thread);
						delete[] V37N02Thread;
						V37N02Thread = NULL;
				}
				return NOT_RUNNING;
				break;
		}

		return NOT_RUNNING;
}

void V16N65ThreadCallback(void)
{
		static uint32_t lastMillis = 0;
		uint32_t currMillis = (uint32_t)millis();

		RtcDateTime ActionTime = LaunchTime;
		RtcDateTime NowTime = RTCDateTimeReturnNow();

		if(ActionTime.TotalSeconds() <= NowTime.TotalSeconds())
		{
				RtcDateTime ClockTime = RtcDateTime( NowTime.TotalSeconds() - ActionTime.TotalSeconds());
				LaunchDisplayData.R1 = ClockTime.Hour() * -1;
				LaunchDisplayData.R2 = ClockTime.Minute()* -1;
				LaunchDisplayData.R3 = (ClockTime.Second() * -100 ) + (((currMillis - lastMillis)/10)%100);


		}
		else
		{
				RtcDateTime ClockTime = RtcDateTime( NowTime.TotalSeconds() - ActionTime.TotalSeconds());
				LaunchDisplayData.R1 = ClockTime.Hour();
				LaunchDisplayData.R2 = ClockTime.Minute();
				LaunchDisplayData.R3 = (ClockTime.Second() * 100 ) + (((currMillis - lastMillis)/10)%100);

		}
		lastMillis = currMillis;
}


ProgramRunStateEnum V16N65Launch(ProgramCallStateEnum call)
{
		switch(call)
		{
		case START_PROGRAM:
		case RESET_PROGRAM:
		case BRING_PROGRAM_TO_FOREGROUND:
		case UNPAUSE_PROGRAM:


				/* Clear out the display data ready for input */
				LaunchDisplayData.R1 = 0;
				LaunchDisplayData.R2 = 0;
				LaunchDisplayData.R3 = 0;
				LaunchDisplayData.Verb = 16;
				LaunchDisplayData.Noun = 65;
				LaunchDisplayData.R1DigitShowMask = 0x3F;
				LaunchDisplayData.R2DigitShowMask = 0x3F;
				LaunchDisplayData.R3DigitShowMask = 0x3F;

				/* Check if the thread already exists before we try to create it. */
				if(V16N65Thread == NULL)
				{
						V16N65Thread = new Thread();
						V16N65Thread->onRun(V16N65ThreadCallback);
						V16N65Thread->setInterval(100);
						controll.add(V16N65Thread);
				}

				return FOREGROUND;
				break;


		case PAUSE_PROGRAM:
		case PUSH_PROGRAM_TO_BACKGROUND:
		case STOP_PROGRAM:
		case NUM_CALL_STATES:
		default:
				if(V16N65Thread != NULL)
				{
						controll.remove(V16N65Thread);
						delete[] V16N65Thread;
						V16N65Thread = NULL;
						return NOT_RUNNING;
				}
				break;
		}

		return NOT_RUNNING;
}

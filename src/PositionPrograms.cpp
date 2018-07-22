/*
 * PositionPrograms.cpp
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

 #include <Wire.h>
 #include <Thread.h>
 #include <ThreadController.h>
 #include <TimerOne.h>
 #include <math.h>

/* Open DSKY headers */
 #include "Neopixels.h"
 #include "Program.h"
 #include "GPS.h"
 #include <helper_3dmath.h>
 #include "IMU.h"
 #include "PositionPrograms.h"
 #include "main.h"


Thread* V16N43Thread = NULL;
Thread* V16N29Thread = NULL;
Thread* V16N30Thread = NULL;
static SevenSegmentDisplayStruct PositionProgramDisplayData;
static uint8_t ActiveProgram = 0; /* Used to track which program is updating the
                                     display data */

/**
 * V16N43ThreadCallback
 *
 * Shows the GPS latitude longitude and altitude information. As there are no
 * decimal places, use a blank digit as a decimal.
 */
void V16N43ThreadCallback(void)
{

		if(ActiveProgram != 1)
		{
				return;
		}
		double lat = GPSGetLatitude();
		double lon = GPSGetLongitude();
		double alt = GPSGetAltitude();

		int32_t latdec = static_cast<int32_t>(lat);
		int32_t londec = static_cast<int32_t>(lon);
		int32_t altdec = static_cast<int32_t>(alt);
		int32_t minorDigits;

		/* no decimal place, so use a blank instead of decimal. This means we need
		   to work out how many digits can be shown */
		if(abs(latdec) > 99)
		{
				PositionProgramDisplayData.R1DigitShowMask = 0x3D;
				latdec *= 100;
				lat*=10.0;
				minorDigits  = static_cast<int32_t>(lat);
				latdec += (minorDigits%10);

		}
		else if(abs(latdec) > 9)
		{
				PositionProgramDisplayData.R1DigitShowMask = 0x3B;
				latdec *= 1000;
				lat*=100.0;
				minorDigits  = static_cast<int32_t>(lat);
				latdec += (minorDigits%100);
		}
		else
		{
				PositionProgramDisplayData.R1DigitShowMask = 0x37;
				latdec *= 10000;
				lat*=1000.0;
				minorDigits  = static_cast<int32_t>(lat);
				latdec += (minorDigits%1000);
		}

		if(abs(londec) > 99)
		{
				PositionProgramDisplayData.R1DigitShowMask = 0x3D;
				londec *= 100;
				lon*=10.0;
				minorDigits  = static_cast<int32_t>(lon);
				londec += (minorDigits%10);

		}
		else if(abs(londec) > 9)
		{
				PositionProgramDisplayData.R1DigitShowMask = 0x3B;
				londec *= 1000;
				lon*=100.0;
				minorDigits  = static_cast<int32_t>(lon);
				londec += (minorDigits%100);
		}
		else
		{
				PositionProgramDisplayData.R1DigitShowMask = 0x37;
				londec *= 10000;
				lon*=1000.0;
				minorDigits  = static_cast<int32_t>(lon);
				londec += (minorDigits%1000);
		}

		PositionProgramDisplayData.R1 = latdec;
		PositionProgramDisplayData.R2 = londec;
		PositionProgramDisplayData.R3 = (altdec%10000);


		PositionProgramDisplayData.Verb = 16;
		PositionProgramDisplayData.Noun = 43;
		PositionProgramDisplayData.Prog = 0;


		NeoPixelSetState(VERB_PIXEL,GREEN_COLOUR,PIXEL_ON);
		NeoPixelSetState(NOUN_PIXEL,GREEN_COLOUR,PIXEL_ON);
		NeoPixelSetState(TRACKER_PIXEL,WARM_WHITE_COLOUR,PIXEL_ON);
		NeoPixelSetState(COMP_ACTY_PIXEL,WARM_WHITE_COLOUR,PIXEL_SLOW_FLASH);
}


/**
 * PositionGetDisplayData
 *
 * Returns a pointer to a structure of type SevenSegmentDisplayStruct that
 * contains the 7 Segment display data for whichever program in this module
 * is running in the FOREGROUND.
 * @return  SevenSegmentDisplayStruct pointer.
 */
SevenSegmentDisplayStruct* PositionGetDisplayData(void)
{
		return &PositionProgramDisplayData;
}

/**
 * V16N43ShowGPSPosition
 *
 * Verb 16 Noun 34 shows the GPS position informaiton
 * @param  call - a ProgramCallStateEnum this should be START_PROGRAM
 * @return - a ProgramRunStateEnum with whether the program is running in the
 *           foreground, background or not at all.
 */
ProgramRunStateEnum V16N43ShowGPSPosition(ProgramCallStateEnum call)
{

		switch(call)
		{
		case START_PROGRAM:
		case BRING_PROGRAM_TO_FOREGROUND:
		case RESET_PROGRAM:
				/* Check if the thread already exists before we try to create it. */
				if(V16N43Thread == NULL)
				{
						V16N43Thread = new Thread();
						V16N43Thread->onRun(V16N43ThreadCallback);
						V16N43Thread->setInterval(50s0);
						controll.add(V16N43Thread);
						ActiveProgram = 1;
						return FOREGROUND;
				}
				break;


		case PUSH_PROGRAM_TO_BACKGROUND:
				ActiveProgram = 0;
				return BACKGROUND;
				break;

		case PAUSE_PROGRAM:
				/* pausing causes a switch over to the do nothing thread */
				V16N43Thread->onRun(ThreadDoNothing);
				ActiveProgram = 0;
				return NOT_RUNNING;
				break;

		case UNPAUSE_PROGRAM:
				/* unpausing changes  back to the V35ThreadCallback thread */
				V16N43Thread->onRun(V16N43ThreadCallback);
				ActiveProgram = 1;
				return FOREGROUND;
				break;

		case STOP_PROGRAM:
		case NUM_CALL_STATES:
		default:
				ActiveProgram = 0;
				NeoPixelSetState(TRACKER_PIXEL,WARM_WHITE_COLOUR,PIXEL_OFF);
				controll.remove(V16N43Thread);
				delete[] V16N43Thread;
				V16N43Thread = NULL;
				return NOT_RUNNING;
				break;
		}

		return NOT_RUNNING;
}

/**
 * V16N29ThreadCallback
 *
 * Shows the IMU Gyro data.
 */
void V16N29ThreadCallback(void)
{
		VectorInt16 rawData = GetGyroRaw();
		if(ActiveProgram != 2)
		{
				return;
		}

		PositionProgramDisplayData.R1 = rawData.x;
		PositionProgramDisplayData.R2 = rawData.y;
		PositionProgramDisplayData.R3 = rawData.z;


		PositionProgramDisplayData.Verb = 16;
		PositionProgramDisplayData.Noun = 30;
		PositionProgramDisplayData.Prog = 0;
		PositionProgramDisplayData.R1DigitShowMask = 0x3F;
		PositionProgramDisplayData.R1DigitShowMask = 0x3F;
		PositionProgramDisplayData.R1DigitShowMask = 0x3F;

}


/**
 * V16N29ShowIMUGyro
 *
 * Verb 16 Noun 29 shows the IMU Gyro information
 * @param  call - a ProgramCallStateEnum this should be START_PROGRAM
 * @return - a ProgramRunStateEnum with whether the program is running in the
 *           foreground, background or not at all.
 */
ProgramRunStateEnum V16N29ShowIMUGyro(ProgramCallStateEnum call)
{
		switch(call)
		{
		case START_PROGRAM:
		case BRING_PROGRAM_TO_FOREGROUND:
		case RESET_PROGRAM:
				/* Check if the thread already exists before we try to create it. */
				if(V16N29Thread == NULL)
				{
						V16N29Thread = new Thread();
						V16N29Thread->onRun(V16N29ThreadCallback);
						V16N29Thread->setInterval(100);
						controll.add(V16N29Thread);
						ActiveProgram = 2;
						return FOREGROUND;
				}
				break;


		case PUSH_PROGRAM_TO_BACKGROUND:
				return BACKGROUND;
				ActiveProgram = 0;
				break;

		case PAUSE_PROGRAM:
				/* pausing causes a switch over to the do nothing thread */
				V16N29Thread->onRun(ThreadDoNothing);
				ActiveProgram = 0;
				return NOT_RUNNING;
				break;

		case UNPAUSE_PROGRAM:
				/* unpausing changes  back to the V35ThreadCallback thread */
				V16N29Thread->onRun(V16N29ThreadCallback);
				ActiveProgram = 2;
				return FOREGROUND;
				break;

		case STOP_PROGRAM:
		case NUM_CALL_STATES:
		default:
				NeoPixelSetState(TRACKER_PIXEL,WARM_WHITE_COLOUR,PIXEL_OFF);
				controll.remove(V16N29Thread);
				delete[] V16N29Thread;
				V16N29Thread = NULL;
				ActiveProgram = 0;
				return NOT_RUNNING;
				break;
		}

		return NOT_RUNNING;
}


/**
 * V16N29ThreadCallback
 *
 * Shows the IMU Accelorometer data.
 */
void V16N30ThreadCallback(void)
{
		VectorInt16 rawData = GetAcelRaw();
		if(ActiveProgram != 3)
		{
				return;
		}

		PositionProgramDisplayData.R1 = rawData.x;
		PositionProgramDisplayData.R2 = rawData.y;
		PositionProgramDisplayData.R3 = rawData.z;


		PositionProgramDisplayData.Verb = 16;
		PositionProgramDisplayData.Noun = 30;
		PositionProgramDisplayData.Prog = 0;
		PositionProgramDisplayData.R1DigitShowMask = 0x3F;
		PositionProgramDisplayData.R1DigitShowMask = 0x3F;
		PositionProgramDisplayData.R1DigitShowMask = 0x3F;

		NeoPixelSetState(VERB_PIXEL,GREEN_COLOUR,PIXEL_ON);
		NeoPixelSetState(NOUN_PIXEL,GREEN_COLOUR,PIXEL_ON);
}


/**
 * V16N30ShowIMUAcel
 *
 * Verb 16 Noun 30 shows the IMU Accelorometer information
 * @param  call - a ProgramCallStateEnum this should be START_PROGRAM
 * @return - a ProgramRunStateEnum with whether the program is running in the
 *           foreground, background or not at all.
 */
ProgramRunStateEnum V16N30ShowIMUAcel(ProgramCallStateEnum call)
{
		switch(call)
		{
		case START_PROGRAM:
		case BRING_PROGRAM_TO_FOREGROUND:
		case RESET_PROGRAM:
				/* Check if the thread already exists before we try to create it. */
				if(V16N30Thread == NULL)
				{
						V16N30Thread = new Thread();
						V16N30Thread->onRun(V16N30ThreadCallback);
						V16N30Thread->setInterval(100);
						controll.add(V16N30Thread);
						ActiveProgram = 3;
						return FOREGROUND;
				}
				break;


		case PUSH_PROGRAM_TO_BACKGROUND:
				ActiveProgram = 0;
				return BACKGROUND;
				break;

		case PAUSE_PROGRAM:
				/* pausing causes a switch over to the do nothing thread */
				V16N30Thread->onRun(ThreadDoNothing);
				ActiveProgram = 0;
				return NOT_RUNNING;
				break;

		case UNPAUSE_PROGRAM:
				/* unpausing changes  back to the V35ThreadCallback thread */
				V16N30Thread->onRun(V16N30ThreadCallback);
				ActiveProgram = 3;
				return FOREGROUND;
				break;

		case STOP_PROGRAM:
		case NUM_CALL_STATES:
		default:
				ActiveProgram = 0;
				NeoPixelSetState(TRACKER_PIXEL,WARM_WHITE_COLOUR,PIXEL_OFF);
				controll.remove(V16N30Thread);
				delete[] V16N30Thread;
				V16N30Thread = NULL;
				return NOT_RUNNING;
				break;
		}

		return NOT_RUNNING;
}

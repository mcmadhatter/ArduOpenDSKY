/*
 * TimeProgram.cpp
 *
 * This file is part of ArduOpenDSKY - the open source software for
 * the OpenDSKY.
 *
 *  Copyright 2018 Jeffrey Marten Gillmor
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
#include "RTC.h"
#include "GPS.h"
#include "DiagnosticPrograms.h"
#include "main.h"


#define HOURS_IDX 0U
#define MINUTES_IDX 1U
#define SECONDS_IDX 2U

static SevenSegmentDisplayStruct TimeProgramDisplayData;
/* Thread handler */
Thread* P16Thread = NULL;


/**
 * TimeGetDisplayData
 *
 * Returns a pointer to a structure of type SevenSegmentDisplayStruct that
 * contains the 7 Segment display data for whichever program in this module
 * is running in the FOREGROUND.
 * @return  SevenSegmentDisplayStruct pointer.
 */
SevenSegmentDisplayStruct* TimeGetDisplayData(void)
{
		return &TimeProgramDisplayData;
}


/**
 * P16ThreadCallback
 *
 * The main callback for the thread that shows the time when Verb 16 Noun 36
 * is requested. This gets and displays the current time from the RTC
 */
void P16ThreadCallback(void)
{
		uint8_t hours;
		uint8_t minutes;
		uint8_t seconds;
		/* We only show the time, not the date , so just give pointers for the
		   time data */
		RtcGetDateTime(NULL, NULL, NULL, &hours, &minutes,  &seconds);

#ifdef DEBUG
		Serial.print("H");
		Serial.print(hours);
		Serial.print("M");
		Serial.print(minutes);
		Serial.print("S");
		Serial.println(seconds);
#endif

		TimeProgramDisplayData.R1 = (int32_t)hours;
		TimeProgramDisplayData.R2 = (int32_t)minutes;
		TimeProgramDisplayData.R3 = (int32_t)seconds;


		TimeProgramDisplayData.Verb = 16;
		TimeProgramDisplayData.Noun = 36;
		TimeProgramDisplayData.Prog = 0;
		TimeProgramDisplayData.R1DigitShowMask = 0x3;
		TimeProgramDisplayData.R1DigitShowMask = 0x3;
		TimeProgramDisplayData.R1DigitShowMask = 0x3;
}


/**
 * V25N36GiveData
 *
 * This function allows other programs (or keyboard input) to give the data
 * needed to be able to set the time on the RTC manually. As it is primarily
 * concerned with using data entered via the keyboard in the form of
 * HH <enter> MM <enter> SS <enter> it will need to be called three times to
 * set the clock. When dataIdx is 0, the hours will be set using the value in
 * data. When dataIdx is 1, the minutes will be set, and when dataIdx is 2, the
 * seconds will be set, the program will stop, and the V16 N36 program will be
 * started so as to show the current time.
 * @param  dataIdx  0 for hours, 1 for minutes, 2 for seconds
 * @param  data    the time value for the particular unit.
 * @return         FOREGROUND if the program should continue to run as more data
 *                 is needed, NOT_RUNNING otherwise.
 */
ProgramRunStateEnum V25N36GiveData(uint8_t dataIdx,int32_t data)
{
		/* Set the hour first */
		if((dataIdx == HOURS_IDX) && (data >= 0) && (data <= LARGEST_HOUR_IN_A_DAY))
		{
				/* Set the RTC value, then display it */
				RtcSetHour((uint8_t)(data));
				TimeProgramDisplayData.R1 = data;
				return FOREGROUND;
		}
		/* Set the minutes second */
		if((dataIdx == MINUTES_IDX) && (data >= 0) && (data <= LARGEST_MINUTE_IN_AN_HOUR))
		{
				/* Set the RTC value, then display it */
				RtcSetMinute((uint8_t)(data));
				TimeProgramDisplayData.R2 = data;
				return FOREGROUND;
		}
		/* Set the seconds third */
		if((dataIdx == SECONDS_IDX) && (data >= 0) && (data <= LARGEST_SECOND_IN_A_MINUTE))
		{
				/* Set the RTC value, then display it */
				RtcSetSecond((uint8_t)(data));
				TimeProgramDisplayData.R3 = data;
				/* RTC time has finished being entered so now call the program that
				   shos the current time. */
				SetProgram(16, 36, START_PROGRAM);
				return NOT_RUNNING;
		}
		return NOT_RUNNING;
}


/**
 * V25N36LoadManualTime
 *
 * This program V25 N36 allows the user to manually enter the time. Once called,
 * it sets up display registers to show zero digits, and waits until the
 * V25N36GiveData function is called to start inputting data. THis program can
 * only run in the foreground, anything else will terminate the program.
 * @param  call - a ProgramCallStateEnum this should be START_PROGRAM
 * @return - a ProgramRunStateEnum with whether the program is running in the
 *           foreground, background or not at all.
 */
ProgramRunStateEnum V25N36LoadManualTime(ProgramCallStateEnum call)
{

		switch(call)
		{
		case START_PROGRAM:
		case BRING_PROGRAM_TO_FOREGROUND:
		case RESET_PROGRAM:
				/* Clear out the display data ready for input */
				TimeProgramDisplayData.R1 = 0;
				TimeProgramDisplayData.R2 = 0;
				TimeProgramDisplayData.R3 = 9;
				TimeProgramDisplayData.Verb = 25;
				TimeProgramDisplayData.Noun = 36;
				TimeProgramDisplayData.Prog = 0;
				TimeProgramDisplayData.R1DigitShowMask = 0x3;
				TimeProgramDisplayData.R1DigitShowMask = 0x3;
				TimeProgramDisplayData.R1DigitShowMask = 0x3;
				return FOREGROUND;
				break;

		case PAUSE_PROGRAM:
		case UNPAUSE_PROGRAM:
		case STOP_PROGRAM:
		case PUSH_PROGRAM_TO_BACKGROUND:
		case NUM_CALL_STATES:
		default:
				/* These call states ar enot supported */
				return NOT_RUNNING;
				break;
		}

		return NOT_RUNNING;
}


/**
 * V26N36GiveData
 *
 * This function instructs the RTC to be set using the GPS time + and offset
 * of the number of hours in the data variable.
 * @param  dataIdx - Should be 0
 * @param  data    the offset in hours vs the GOS time
 * @return         FOREGROUND if the program should continue to run as more data
 *                 is needed, NOT_RUNNING otherwise.
 */
ProgramRunStateEnum V26N36GiveData(uint8_t dataIdx,int32_t data)
{
		/* Set the hour first */
		if((dataIdx == HOURS_IDX) && (data <= LARGEST_HOUR_IN_A_DAY) && (data >= (-1* LARGEST_HOUR_IN_A_DAY)))
		{
				/* Set the RTC value, then display it */
				uint8_t Hour = GPSGetHour();
				/* negative numbers mean you are near a day boundary and the GPS is actually in the
				   following day vs the requested RTC value (e.g RTC needs to be 11PM, and GPS is at 1AM)
				     take care of this wrapping. */
				if(abs(data) > Hour)
				{
						RtcSetHour((uint8_t)(24-(int8_t)data + (int8_t)Hour));
				}
				else
				{
						RtcSetHour((uint8_t)(data + Hour));
				}
				RtcSetMinute(GPSGetMinute());
				RtcSetSecond(GPSGetSecond());
				/* Show the time on the sevne segment diplays */
				SetProgram(16, 36, START_PROGRAM);
				return NOT_RUNNING;
		}
		return NOT_RUNNING;
}

/**
 * V26N36LoadGPSTime
 *
 * This program V26 N36 allows the user to set the RTC via the GPS module,
 * Once called, it sets up display registers to show zero digits, and waits
 * until the P26GiveData function is called to start inputting data. This
 * program can only run in the foreground, anything else will terminate the
 * program.
 * @param  call - a ProgramCallStateEnum this should be START_PROGRAM
 * @return - a ProgramRunStateEnum with whether the program is running in the
 *           foreground, background or not at all.
 */
ProgramRunStateEnum V26N36LoadGPSTime(ProgramCallStateEnum call)
{
		switch(call)
		{
		case START_PROGRAM:
		case BRING_PROGRAM_TO_FOREGROUND:
		case RESET_PROGRAM:
				/* Clear out the display data ready for input */
				TimeProgramDisplayData.R1 = 0;
				TimeProgramDisplayData.R2 = 0;
				TimeProgramDisplayData.R3 = 9;
				TimeProgramDisplayData.Verb = 25;
				TimeProgramDisplayData.Noun = 36;
				TimeProgramDisplayData.Prog = 0;
				TimeProgramDisplayData.R1DigitShowMask = 0x3;
				TimeProgramDisplayData.R1DigitShowMask = 0x3;
				TimeProgramDisplayData.R1DigitShowMask = 0x3;
				return FOREGROUND;
				break;

		case PAUSE_PROGRAM:
		case UNPAUSE_PROGRAM:
		case STOP_PROGRAM:
		case PUSH_PROGRAM_TO_BACKGROUND:
		case NUM_CALL_STATES:
		default:
				/* These call states ar enot supported */
				return NOT_RUNNING;
				break;
		}

		return NOT_RUNNING;
}


/**
 * V16N36ShowRTCTime
 *
 * This Program V16 N36 will show the current time from the RTC on the 7
 * segment displays. It takes the time from the RTC rather than the GPS, as the
 * GPS will not take into account daylight saving time.
 * @param  call - a ProgramCallStateEnum this should be START_PROGRAM
 * @return - a ProgramRunStateEnum with whether the program is running in the
 *           foreground, background or not at all.
 */
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
				/* If the thred exists, delete it */
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

/**
 * V37N36SetAlarmProg
 *
 * Starts up the V37N36 prgram which allows you to set an alarm that calls
 * another program at the set time.
 *
 * @param  call - a ProgramCallStateEnum this should be START_PROGRAM
 * @return - a ProgramRunStateEnum with whether the program is running in the
 *           foreground, background or not at all.
 */
ProgramRunStateEnum V37N36SetAlarmProg(ProgramCallStateEnum call)
{
		switch(call)
		{
		case START_PROGRAM:
		case BRING_PROGRAM_TO_FOREGROUND:
		case RESET_PROGRAM:
				/* Clear out the display data ready for input */
				TimeProgramDisplayData.R1 = 0;
				TimeProgramDisplayData.R2 = 0;
				TimeProgramDisplayData.R3 = 9;
				TimeProgramDisplayData.Verb = 37;
				TimeProgramDisplayData.Noun = 36;
				TimeProgramDisplayData.Prog = 0;
				TimeProgramDisplayData.R1DigitShowMask = 0x3;
				TimeProgramDisplayData.R1DigitShowMask = 0x3;
				TimeProgramDisplayData.R1DigitShowMask = 0x3;
				return FOREGROUND;
				break;

		case PAUSE_PROGRAM:
		case UNPAUSE_PROGRAM:
		case STOP_PROGRAM:
		case PUSH_PROGRAM_TO_BACKGROUND:
		case NUM_CALL_STATES:
		default:
				/* These call states ar enot supported */
				return NOT_RUNNING;
				break;
		}

		return NOT_RUNNING;
}


/**
 * V37N36GiveData
 *
 * Allows the V37N36 program to set an alarm. if any value is
 * outside of range, then the defualt is to use the current time value.
 * data is processed in the order day,  hour,  minute, second,verb,noun, repeat
 * @param  DataIdx Index of the data to set
 * @param  data    data to set
 * @return         FOREGROUND if not all data has been set, NOT_RUNNING otherwise.
 */
ProgramRunStateEnum V37N36GiveData(uint8_t DataIdx,int32_t data)
{
		static uint8_t day;
		static uint8_t hour;
		static uint8_t minute;
		static uint8_t second;
		static uint16_t verb = 0;
		static uint16_t noun = 0;
		static uint8_t repeat = 0;
		switch(DataIdx)
		{
		case 0:
				/* Get the current date time (we can use it fo rany ignored values )*/
				RtcGetDateTime(NULL, NULL, &day, &hour, &minute, &second);
				if((data < 31) && (data > 0))
				{
						day = (uint8_t)data;
				}
				break;

		case 1:
				if((data < 24) && (data >= 0))
				{
						hour = (uint8_t)data;
				}
				break;
		case 2:
				if((data < 60) && (data >= 0))
				{
						minute = (uint8_t)data;
				}
				break;
		case 3:
				if((data < 60) && (data >= 0))
				{
						second = (uint8_t)data;
				}
				break;
		case 4:
				verb = (uint8_t)data;
				break;
		case 5:
				noun = (uint8_t)data;
				break;
		case 6:
				if((data <= 1) && (data >= 0))
				{
						repeat = (uint8_t)data;

				}
	#ifdef DEBUG
				Serial.print ("Set Program V");
				Serial.print(verb);
				Serial.print("N");
				Serial.print(noun);
				Serial.print(" to run on" );

				Serial.print(day);
				Serial.print(":");
				Serial.print(hour);
				Serial.print(":");
				Serial.print(minute);
				Serial.print(":");
				Serial.print(second);

				if(repeat)
				{
						Serial.println(" on repeat");
				}
				else
				{
						Serial.println(" once");
				}
#endif
				RtcSetAlarmProgram( day,  hour,  minute,  second,  verb,  noun,   repeat);
				return NOT_RUNNING;
				break;
		default:
				break;

		}
		return FOREGROUND;
}

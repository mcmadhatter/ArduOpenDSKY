/*
 * RTC.cpp
 *
 * This file is part of ArduOpenDSKY - the open source software for
 * the OpenDSKY.
 *
 * Driver for the RTC module, and alarm clock handler
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
#include <RtcDS3231.h>
#include <Thread.h>
#include <ThreadController.h>
#include <TimerOne.h>

#include "Program.h"
#include "RTC.h"
#include "main.h"


Thread* rtcThread = new Thread();
RtcDS3231<TwoWire> Rtc(Wire);


static void PrintDateTime(const RtcDateTime& dt);

typedef struct ALARM_PROGRAM_STRUCT
{
		int32_t ProgData;
		int16_t Verb;
		int16_t Noun;
		uint8_t Repeat;
		uint8_t Day;
		uint8_t Hour;
		uint8_t Minute;
		uint8_t Second;


}AlarmProgramStruct;


AlarmProgramStruct alarmProgram;


/**
 * RTCThreadCallback
 *
 * Callback handler for alarm processing
 */
void RTCThreadCallback()
{


		DS3231AlarmFlag flag = Rtc.LatchAlarmsTriggeredFlags();

		if (flag & DS3231AlarmFlag_Alarm1)
		{
				/*Call the program handler */
				(void)SetProgram(alarmProgram.Verb, alarmProgram.Noun, BRING_PROGRAM_TO_FOREGROUND);
				(void)GiveNumbersToProgram(alarmProgram.ProgData);
		}


}

/**
 * RTCSetup
 *
 * Main setup function for the RTC driver module, this configures the RTC and
 * creates the RTC callback thread that periodically gets new date time data
 * from the RTC
 */
void RTCSetup(void)
{


		Rtc.Begin();

		RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

		if (!Rtc.IsDateTimeValid())
		{
				Serial.println("RTC lost confidence in the DateTime!");
				Rtc.SetDateTime(compiled);
		}

		if (!Rtc.GetIsRunning())
		{
				Serial.println("RTC was not actively running, starting now");
				Rtc.SetIsRunning(true);
		}

		RtcDateTime now = Rtc.GetDateTime();
		if (now < compiled)
		{
				Serial.println("RTC is older than compile time!  (Updating DateTime)");
				Rtc.SetDateTime(compiled);
		}


		Rtc.Enable32kHzPin(false);


		rtcThread->onRun(RTCThreadCallback);
		rtcThread->setInterval(500);
		controll.add(rtcThread);
}


/**
 * RtcSetDateTime
 *
 * Set the RTC date time
 * @param year   Year to set
 * @param month  Month to set
 * @param day    Day to set
 * @param hour   Hour to set
 * @param minute Minute to set
 * @param second Second to set
 */
void RtcSetDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
		if((month <= 12) && (day <= 31) && (hour = 23) && (minute <= 59) && (second <= 59))
		{
				RtcDateTime dt = new RtcDateTime(year, month, day, hour, minute, second);
				Rtc.SetDateTime(dt);
		}
}


/**
 * RtcGetDateTime
 *
 * Get the current date time, each of the parts should be given as a pointer
 * which this function can then fill with data, any NULL pointers will be
 * ignored.
 *
 * @param year   Pointer to fill with year number
 * @param month  Pointer to fill with month number
 * @param day    Pointer to fill with day number
 * @param hour   Pointer to fill with hour number
 * @param minute Pointer to fill with minute number
 * @param second Pointer to fill with second number
 */
void RtcGetDateTime(uint16_t* year, uint8_t* month, uint8_t* day, uint8_t* hour, uint8_t* minute, uint8_t* second)
{
		RtcDateTime now = Rtc.GetDateTime();

		if(year != NULL)
		{
				*year = now.Year();
		}
		if(month != NULL)
		{
				*month = now.Month();
		}
		if(day != NULL)
		{
				*day = now.Day();
		}
		if(hour != NULL)
		{
				*hour = now.Hour();
		}
		if(minute != NULL)
		{
				*minute = now.Minute();
		}
		if(second != NULL)
		{
				*second = now.Second();
		}
}


/**
 * RtcSetHour
 *
 * Set the current Hour on the RTC
 * @param day  Hour number to set.
 */
void RtcSetHour(uint8_t hour)
{
		RtcDateTime now = Rtc.GetDateTime();
		RtcSetDateTime(now.Year(), now.Month(),now.Day(), hour, now.Minute(), now.Second());
}


/**
 * RtcSetMinute
 *
 * Set the current Minute on the RTC
 * @param day  Minute number to set.
 */
void RtcSetMinute(uint8_t minute)
{
		RtcDateTime now = Rtc.GetDateTime();
		RtcSetDateTime(now.Year(), now.Month(),now.Day(), now.Hour(), minute, now.Second());
}


/**
 * RtcSetSecond
 *
 * Set the current Second on the RTC
 * @param day  Second number to set.
 */
void RtcSetSecond(uint8_t second)
{
		RtcDateTime now = Rtc.GetDateTime();
		RtcSetDateTime(now.Year(), now.Month(),now.Day(), now.Hour(), now.Minute(), second);
}


/**
 * RtcSetYear
 *
 * Set the current Year on the RTC
 * @param day  Year number to set.
 */
void RtcSetYear(uint16_t year)
{
		RtcDateTime now = Rtc.GetDateTime();
		RtcSetDateTime(year, now.Month(),now.Day(), now.Hour(), now.Minute(), now.Second());
}


/**
 * RtcSetMonth
 *
 * Set the current Month on the RTC
 * @param day  Month number to set.
 */
void RtcSetMonth(uint8_t month)
{
		RtcDateTime now = Rtc.GetDateTime();
		RtcSetDateTime(now.Year(), month,now.Day(), now.Hour(), now.Minute(), now.Second());
}

/**
 * RtcSetDay
 *
 * Set the current Day on the RTC
 * @param day  Day number to set.
 */
void RtcSetDay(uint8_t day)
{
		RtcDateTime now = Rtc.GetDateTime();
		RtcSetDateTime(now.Year(), now.Month(),day, now.Hour(), now.Minute(), now.Second());
}


/**
 * RtcSetAlarmProgram
 *
 * Sets up an alarm that calls the specified verb noun program at the specified
 * time. It is also possible to set the alarm to repeat if you want.
 * @param day    day of the week to set the alarm for, or ALARM_IGNORE to ignore
 * @param hour   hour of the alarm, or ALARM_IGNORE to ignore
 * @param minute minute of the alar, or ALARM_IGNORE to ignore
 * @param second second of the alarm, or ALARM_IGNORE to ignore
 * @param verb   verb of the program to call when the alarm goes off
 * @param noun   noun of the program to call when the alarm goes off
 * @param progData data to give to the program
 * @param repeat 1 to repeat, 0 otherwise
 */
void RtcSetAlarmProgram(uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint16_t verb, uint16_t noun, int32_t progData, uint8_t repeat)
{
		/* get the current date/time */
		RtcDateTime now = Rtc.GetDateTime();
		alarmProgram.Repeat = repeat;
		if(day == ALARM_IGNORE)
		{
				alarmProgram.Day = now.Day();
		}
		else
		{
				alarmProgram.Day = day;
		}
		if(hour == ALARM_IGNORE)
		{
				alarmProgram.Hour = now.Hour();
		}
		else
		{
				alarmProgram.Hour = hour;
		}
		if(minute == ALARM_IGNORE)
		{
				alarmProgram.Minute = now.Minute();
		}
		else
		{
				alarmProgram.Minute = minute;
		}
		if(second == ALARM_IGNORE)
		{
				alarmProgram.Second = now.Second();
		}

		else
		{
				alarmProgram.Second = second;
		}
		alarmProgram.Noun = noun;
		alarmProgram.Verb = verb;
		alarmProgram.ProgData = progData;

		DS3231AlarmOne alarm1(
				alarmProgram.Day,
				alarmProgram.Hour,
				alarmProgram.Minute,
				alarmProgram.Second,
				alarmProgram.Repeat);

		Rtc.SetAlarmOne(alarm1);

		Rtc.LatchAlarmsTriggeredFlags();


}

/**
 * PrintDateTime
 *
 * Prints the date and time given in the input variable to the serial port
 * @param dt the datetime structure to print.
 */
static void PrintDateTime(const RtcDateTime& dt)
{
		char datestring[20];

		sprintf_P(datestring,

		          PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
		          dt.Month(),
		          dt.Day(),
		          dt.Year(),
		          dt.Hour(),
		          dt.Minute(),
		          dt.Second() );
		Serial.print(datestring);
}

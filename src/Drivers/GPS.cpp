/*
 * GPS.cpp
 *
 * Driver for regularly getting the gps data.
 *
 * This driver requires the following libraries to be installed:
 *
 * TinyGPSPlus by Mikal Hart , this should be available from here
 * https://github.com/mikalhart/TinyGPSPlus
 *
 * digitalWriteFast by Watterott electronic, this should be available from here
 * https://github.com/watterott/Arduino-Libs/tree/master/digitalWriteFast
 *
 * This file is part of ArduOpenDSKY - the open source software for
 * the OpenDSKY.
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
#include <TinyGPS++.h>
#include <digitalWriteFast.h>

/* Open DSKY headers */
#include "../main.h"
#include "../Drivers/GPS.h"

TinyGPSPlus gps;

Thread* GPSThread = new Thread();


typedef struct GPS_DATA_STRUCT
{
		uint8_t Second;
		uint8_t Minute;
		uint8_t Hour;
		uint8_t Day;
		uint8_t Month;
		uint16_t Year;
		double Latitude;
		double Longitude;
		double Altitude;
		double Speed;
		double Course;
}GPSDataStruct;


typedef enum GPS_READ_STATE_ENUM
{
		GPS_IDLE,
		GPS_READ_STARTED,
		GPS_PROCESS_DATA

}GPSReadStateEnum;


static GPSReadStateEnum GPSReadState = GPS_IDLE;
static GPSDataStruct GPSData;

/* #define DEBUG */ /* Use this if you want to see serial port output */

#ifdef DEBUG
static void displayInfo(void);
#endif

/**
 * GPSSetup
 *
 * Sets up the GPS driver,
 * Configures the relay that switches the Serial RX line in and out.
 * Sets up the GPS driver thread and registers the callback
 */
void GPSSetup(void)
{
		/* pinModeFast library is much faster than the standard arduino libs */
		pinModeFast(7, OUTPUT);
		GPSThread->onRun(GPSThreadCallback);
		GPSThread->setInterval(20);
		controll.add(GPSThread);
}

/**
 * GPSThreadCallback
 *
 * This is the main thread for the GPS Driver, it has three states:
 * GPS_IDLE - sets up the serial reciever and thread timing
 * GPS_READ_STARTED - Reads in the GPSData
 * GPS_PROCESS_DATA - processes the reecieved data (if valid) and then sets up
 *                    a longer wait until the next read cycle, before switching
 *                    out the serial line relay.
 */
void GPSThreadCallback(void)
{
		switch(GPSReadState)
		{
		case GPS_IDLE:
		default:
				/* connect the GPS RX line via the relay */
				digitalWriteFast(7, HIGH);
				/* Redcude the interval until the thread callback is called again */
				GPSThread->setInterval(15);
				GPSReadState = GPS_READ_STARTED;
				break;

		case GPS_READ_STARTED:
				/* If serial data is availble, then process it until a valid GPS
				   packet is found, or we run out of serial data */
				while((Serial.available()) && (GPSReadState == GPS_READ_STARTED))
				{
						/* If the encode has finished, then start processing the data
						     on the next callback */
						if(gps.encode(Serial.read()))
						{
								GPSReadState = GPS_PROCESS_DATA;
						}
				}

				break;

		case GPS_PROCESS_DATA:
				/* release the serial line  via the relay*/
				digitalWriteFast(7, LOW);

				/* wait a while until we try to get fresh data */
				GPSThread->setInterval(250);
				GPSReadState = GPS_IDLE;

				/* Process the actual data */
				GPSData.Second = gps.time.second();
				GPSData.Minute = gps.time.minute();
				GPSData.Hour = gps.time.hour();
				GPSData.Day = gps.date.day();
				GPSData.Month = gps.date.month();
				GPSData.Year = gps.date.year();
				GPSData.Latitude = gps.location.lat();
				GPSData.Longitude = gps.location.lng();
				GPSData.Altitude = gps.altitude.feet();
				GPSData.Speed= gps.speed.mph();
				GPSData.Course= gps.course.deg();

#ifdef DEBUG
				displayInfo();
#endif

				break;
		}
}


/**
 * GPSGetHour
 *
 * @return  the gps hour as a uint8_t
 */
uint8_t GPSGetHour(void)
{
		return GPSData.Hour;
}


/**
 * GPSGetMinute
 *
 * @return  the gps minute as a uint8_t
 */
uint8_t GPSGetMinute(void)
{
		return GPSData.Minute;
}


/**
 * GPSGetSecond
 *
 * @return  the gps second as a uint8_t
 */
uint8_t GPSGetSecond(void)
{
		return GPSData.Second;
}


/**
 * GPSGetDay
 *
 * @return  the gps day of the month as a uint8_t
 */
uint8_t GPSGetDay(void)
{
		return GPSData.Day;
}


/**
 * GPSGetMonth
 *
 * @return  the gps month of the year as a uint8_t
 */
uint8_t GPSGetMonth(void)
{
		return GPSData.Month;
}

/**
 * GPSGetYear
 *
 * @return  the gps year as a uint16_t
 */
uint16_t GPSGetYear(void)
{
		return GPSData.Year;
}

/**
 * GPSGetLatitude
 *
 * @return  the gps Latitude in degrees as a double
 */
double GPSGetLatitude(void)
{
		return GPSData.Latitude;
}


/**
 * GPSGetLongitude
 *
 * @return  the gps Longitude in degrees as a double
 */
double GPSGetLongitude(void)
{
		return GPSData.Longitude;
}


/**
 * GPSGetAltitude
 *
 * @return  the gps Altitude in feet as a double
 */
double GPSGetAltitude(void)
{
		return GPSData.Altitude;
}


/**
 * GPSGetSpeed
 *
 * @return  the gps Speed in mph as a double
 */
double GPSGetSpeed(void)
{
		return GPSData.Speed;
}


/**
 * GPSGetSpeed
 *
 * @return  the gps course headin in degree as a double
 */
double GPSGetCourse(void)
{
		return GPSData.Course;
}

#ifdef DEBUG
/**
   displayInfo()

   Prints out any valid data over the Serial port.
 */
static void displayInfo(void)
{
		Serial.print(F("Location: "));
		if (gps.location.isValid())
		{
				Serial.print(gps.location.lat(), 6);
				Serial.print(F(","));
				Serial.print(gps.location.lng(), 6);
		}
		else
		{
				Serial.print(F("INVALID"));
		}

		Serial.print(F("  Date/Time: "));
		if (gps.date.isValid())
		{
				Serial.print(gps.date.month());
				Serial.print(F("/"));
				Serial.print(gps.date.day());
				Serial.print(F("/"));
				Serial.print(gps.date.year());
		}
		else
		{
				Serial.print(F("INVALID"));
		}

		Serial.print(F(" "));
		if (gps.time.isValid())
		{
				if (gps.time.hour() < 10)
				{
						Serial.print(F("0"));
				}
				Serial.print(gps.time.hour());
				Serial.print(F(":"));
				if (gps.time.minute() < 10)
				{
						Serial.print(F("0"));
				}
				Serial.print(gps.time.minute());
				Serial.print(F(":"));
				if (gps.time.second() < 10)
				{
						Serial.print(F("0"));
				}
				Serial.print(gps.time.second());
				Serial.print(F("."));
				if (gps.time.centisecond() < 10)
				{
						Serial.print(F("0"));
				}
				Serial.print(gps.time.centisecond());
		}
		else
		{
				Serial.print(F("INVALID"));
		}

		Serial.println();
}
#endif

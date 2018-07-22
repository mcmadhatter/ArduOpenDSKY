/*
 * SevenSegment.cpp
 *
 * Copyright 2018 Jeffrey Marten Gillmor
 *
 * OpenDSKY is a trademark of S&T GeoTronics LLC  https://opendsky.com
 *
 * This is the 7 segment display driver, it currently only supports a
 * virtual USB 7 segment dispaly, support for the hardware is pending. Enable
 * the #define SERIAL_DEBUG   to show the status of the 7 segmnet over the
 * serial link.
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
#include <LedControl.h>
#include <Thread.h>
#include <ThreadController.h>
#include <TimerOne.h>

/* Open DSKY headers */
#include "Program.h"
#include "SevenSegment.h"
#include "main.h"

/* VT100 code for clearing the screen */
#define CLS  "\033[2J"
#define SERIAL_DEBUG

Thread* sevenSegmentThread = new Thread();

LedControl lc=LedControl(12,10,11,4);


/**
 * SevenSegmentThreadCallback
 *
 * The main callback function, this asks the Program module for the
 * latest display data from whatever is running in the foreground, then
 * it displays it.
 */
void SevenSegmentThreadCallback()
{
		SevenSegmentDisplayStruct* displayData = ProgramGetDisplayData();


		if(displayData != NULL)
		{
#ifdef SERIAL_DEBUG
#ifdef VT100
				Serial.print(CLS);
#endif
				Serial.print("    P");
				if(displayData->Prog != 0)
				{
						Serial.println(displayData->Prog);
				}
				else
				{
						Serial.println("  ");
				}
				Serial.print("V");
				Serial.print(displayData->Verb);
				Serial.print(" N");
				Serial.println(displayData->Noun);


				if(displayData->R1 > 0 )
				{
						Serial.print("+");
				}
				else
				{
						displayData->R1*=-1;
						Serial.print("-");
				}

				Serial.println(displayData->R1);


				if(displayData->R2 > 0 )
				{
						Serial.print("+");
				}
				else
				{
						displayData->R2*=-1;
						Serial.print("-");
				}


				Serial.println(displayData->R2);


				if(displayData->R3 > 0 )
				{
						Serial.print("+");
				}
				else
				{
						displayData->R3*=-1;
						Serial.print("-");
				}
				Serial.println(displayData->R3);
				Serial.println(" ");
#endif
		}
}


/**
 * SevenSegmentSetup
 *
 * Sets up the 7 Segement display driver module.
 */
void SevenSegmentSetup(void)
{

		sevenSegmentThread->onRun(SevenSegmentThreadCallback);
		sevenSegmentThread->setInterval(250);
		controll.add(sevenSegmentThread);

}

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
//#define SERIAL_DEBUG

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
#ifndef SERIAL_DEBUG

/* I have no idea if this 7 segment code works yet, as I am still waiting on my
   delivery of the 7 segment displays. I had a request to show my thoughts on how
   it would be implemented - so here it is. I have not idea of the indexs are
   correct, or even if there are extra steps needed aside from setting digits. I
     would be much obliged for a bit of help from someone who has their 7 segment
     displays*/
				for(uint8_t idx = 0U; idx < 4U; idx++)
				{
						lc.clearDisplay(idx);

				}
				if((0x3 & displayData->R1DigitShowMask) != 0)
				{
						lc.setChar(0, 0, (displayData->Prog%10)&0xF, false);
						lc.setChar(0, 1, (displayData->Prog/10)&0xF, false);
				}
				if((0xC & displayData->R1DigitShowMask) != 0)
				{
						lc.setChar(0, 2, (displayData->Noun%10)&0xF, false);
						lc.setChar(0, 3, (displayData->Noun/10)&0xF, false);
				}
				if((0x30 & displayData->R1DigitShowMask) != 0)
				{
						lc.setChar(0, 4, (displayData->Verb%10)&0xF, false);
						lc.setChar(0, 5, (displayData->Verb/10)&0xF, false);
				}

				if((0x20 & displayData->R1DigitShowMask) == 0x20)
				{
						if(displayData->R1 < 0 )
						{
								lc.setRow(1,0,B00100100);
						}
						else
						{
								lc.setRow(1,0,B01110100);
						}
				}
				for (int i = 0; i < 5; i++)
				{
						if(((1 << i)  & displayData->R1DigitShowMask) != 0)
						{
								lc.setChar(1, (5-i), (displayData->R1 & 0xf), false);
								displayData->R1/=10;
						}
						else
						{
								lc.setChar(1, (5-i), ' ', false);
						}
				}
				if((0x20 & displayData->R2DigitShowMask) == 0x20)
				{
						if(displayData->R2 < 0 )
						{
								lc.setRow(2,0,B00100100);
						}
						else
						{
								lc.setRow(2,0,B01110100);
						}
				}

				for (int i = 0; i < 5; i++)
				{
						if(((1 << i)  & displayData->R2DigitShowMask) != 0)
						{
								lc.setChar(2, (5-i), (displayData->R2 & 0xf), false);
								displayData->R2/=10;
						}
						else
						{
								lc.setChar(2, (5-i), ' ', false);
						}
				}
				if((0x20 & displayData->R3DigitShowMask) == 0x20)
				{
						if(displayData->R3 < 0 )
						{
								lc.setRow(3,0,B00100100);
						}
						else
						{
								lc.setRow(3,0,B01110100);
						}
				}

				for (int i = 0; i < 5; i++)
				{
						if(((1 << i)  & displayData->R3DigitShowMask) != 0)
						{
								lc.setChar(3, (5-i), (displayData->R3 & 0xf), false);
								displayData->R3/=10;
						}
						else
						{
								lc.setChar(3, (5-i), ' ', false);
						}
				}


#else
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
		/* intialise the 7 segment drivers */
		for(uint8_t idx = 0U; idx < 4U; idx++)
		{
				lc.shutdown(idx,false);
				lc.setIntensity(idx,8);
				lc.clearDisplay(idx);
		}


		sevenSegmentThread->onRun(SevenSegmentThreadCallback);
		sevenSegmentThread->setInterval(250);
		controll.add(sevenSegmentThread);


}

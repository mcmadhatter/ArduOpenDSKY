/*
 * DiagnosticPrograms.cpp
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

/* Open DSKY headers */
#include "Neopixels.h"
#include "Program.h"
#include "Sound.h"
#include "DiagnosticPrograms.h"
#include "main.h"


Thread* V35Thread = NULL;


/**
 * V35BulbTest
 *
 * Starts and stops the V35 bulb test (this program does not require a noun).
 * The program can only run in the foreground. Stopping or pushing the program
 * to the background will result in the disposal of the thread, and all the
 * lights being turned off.
 * @param  call - a ProgramCallStateEnum this should be START_PROGRAM
 * @return - a ProgramRunStateEnum with whether the program is running in the
 *           foreground, background or not at all.
 */
ProgramRunStateEnum V35BulbTest(ProgramCallStateEnum call)
{
		switch(call)
		{

		case RESET_PROGRAM:
		case START_PROGRAM:
		case BRING_PROGRAM_TO_FOREGROUND:
				/* Check if the thread already exists before we try to create it. */
				if(V35Thread == NULL)
				{
						V35Thread = new Thread();
						V35Thread->onRun(V35ThreadCallback);
						V35Thread->setInterval(20);
						controll.add(V35Thread);
						return FOREGROUND;
				}

				break;
		case PAUSE_PROGRAM:
				/* pausing causes a switch over to the do nothing thread */
				V35Thread->onRun(ThreadDoNothing);
				return NOT_RUNNING;
				break;

		case UNPAUSE_PROGRAM:
				/* unpausing changes  back to the V35ThreadCallback thread */
				V35Thread->onRun(V35ThreadCallback);
				return FOREGROUND;
				break;

		/* this program only runs in FOREGROUND , evreything else will turn it off*/
		case PUSH_PROGRAM_TO_BACKGROUND:
		case STOP_PROGRAM:
		case NUM_CALL_STATES:
		default:
				for(uint8_t i = 0; i <NUM_PIXELS; i++)
				{
						NeoPixelSetState(i,RAINBOW_COLOUR,PIXEL_OFF);
				}
				controll.remove(V35Thread);
				delete[] V35Thread;
				V35Thread = NULL;
				return NOT_RUNNING;
				break;

		}
		return NOT_RUNNING;
}


/**
 * V35ThreadCallback
 *
 * The main thread callback, this changes which pixels are on, and their colour,
 * every second.
 */
void V35ThreadCallback(void)
{
		uint16_t moduleTime = (millis()%12000);

		for(uint8_t i = 0; i <NUM_PIXELS; i++)
		{
				/* half the pixels white */
				if(moduleTime< 1000)
				{
						NeoPixelSetState(i,PURE_WHITE_COLOUR,((i%2)==0) ? PIXEL_ON : PIXEL_OFF);
				}

				/* the other half of the pixels white */
				else if(moduleTime < 2000)
				{
						NeoPixelSetState(i,PURE_WHITE_COLOUR,((i%2)==0) ? PIXEL_OFF : PIXEL_ON);
				}

				/* half the pixels blue */
				else if(moduleTime < 3000)
				{
						NeoPixelSetState(i,BLUE_COLOUR,((i%2)==0) ? PIXEL_ON : PIXEL_OFF);
				}

				/* the other half of the pixels blue */
				else if(moduleTime < 4000)
				{
						NeoPixelSetState(i,BLUE_COLOUR,((i%2)==0) ? PIXEL_OFF : PIXEL_ON);
				}
				/* half the pixels red */
				else if(moduleTime < 5000)
				{
						NeoPixelSetState(i,RED_COLOUR,((i%2)==0) ? PIXEL_OFF : PIXEL_ON);
				}

				/* the other half of the pixels red */
				else if(moduleTime < 6000)
				{
						NeoPixelSetState(i,RED_COLOUR,((i%2)==0) ? PIXEL_ON : PIXEL_OFF);
				}

				/* half the pixels green */
				else if(moduleTime < 7000)
				{
						NeoPixelSetState(i,GREEN_COLOUR,((i%2)==0) ? PIXEL_OFF : PIXEL_ON);
				}

				/* the other half of the pixels green */
				else if(moduleTime < 8000)
				{
						NeoPixelSetState(i,GREEN_COLOUR,((i%2)==0) ? PIXEL_ON : PIXEL_OFF);
				}
				/* full colour cycle */
				else
				{
						NeoPixelSetState(i,RAINBOW_COLOUR,PIXEL_ON);
				}
		}
}

ProgramRunStateEnum V21N98GiveData(uint8_t dataIdx,int32_t data)
{
		if(data >=0 )
		{
				PlayTrack(data);
		}
		return NOT_RUNNING;
}

ProgramRunStateEnum V21N98SoundTest(ProgramCallStateEnum call)
{
		switch(call)
		{

		case RESET_PROGRAM:
		case START_PROGRAM:
		case BRING_PROGRAM_TO_FOREGROUND:
				/* Check if the thread already exists before we try to create it. */
				return FOREGROUND;
				break;
		/* this program only runs in FOREGROUND , evreything else will turn it off*/
		case PAUSE_PROGRAM:
		case UNPAUSE_PROGRAM:
		case PUSH_PROGRAM_TO_BACKGROUND:
		case STOP_PROGRAM:
		case NUM_CALL_STATES:
		default:
				return NOT_RUNNING;
				break;

		}
		return NOT_RUNNING;
}

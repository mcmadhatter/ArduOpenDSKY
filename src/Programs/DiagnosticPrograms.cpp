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
#include "../Drivers/Neopixels.h"
#include "../Drivers/Program.h"
#include "../Programs/DiagnosticPrograms.h"
#include "../main.h"


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
				NeoPixelSetState(PROG_RUN_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(NOUN_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(VERB_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(COMP_ACTY_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(TEMP_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(GIMBAL_LOCK_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(PROG_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(RESTART_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(TRACKER_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(ALT_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(VEL_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(BLANK_1_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(BLANK_2_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(OPP_ERR_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(KEY_REL_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(STBY_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(NO_ATT_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(UPLINK_ACTY_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
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
void V35ThreadCallback()
{
		/* half the pixels white */
		if((millis()%12000) < 1000)
		{
				NeoPixelSetState(PROG_RUN_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(NOUN_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(VERB_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(COMP_ACTY_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(TEMP_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(GIMBAL_LOCK_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(PROG_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(RESTART_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(TRACKER_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(ALT_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(VEL_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(BLANK_1_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(BLANK_2_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(OPP_ERR_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(KEY_REL_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(STBY_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(NO_ATT_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(UPLINK_ACTY_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
		}

		/* the other half of the pixels white */
		else if((millis()%12000) < 2000)
		{
				NeoPixelSetState(PROG_RUN_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(NOUN_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(VERB_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(COMP_ACTY_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(TEMP_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(GIMBAL_LOCK_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(PROG_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(RESTART_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(TRACKER_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(ALT_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(VEL_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(BLANK_1_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(BLANK_2_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(OPP_ERR_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(KEY_REL_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(STBY_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
				NeoPixelSetState(NO_ATT_PIXEL,PURE_WHITE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(UPLINK_ACTY_PIXEL,PURE_WHITE_COLOUR,PIXEL_ON);
		}

		/* half the pixels blue */
		else if((millis()%12000) < 3000)
		{
				NeoPixelSetState(PROG_RUN_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(NOUN_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(VERB_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(COMP_ACTY_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(TEMP_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(GIMBAL_LOCK_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(PROG_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(RESTART_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(TRACKER_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(ALT_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(VEL_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(BLANK_1_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(BLANK_2_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(OPP_ERR_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(KEY_REL_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(STBY_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(NO_ATT_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(UPLINK_ACTY_PIXEL,BLUE_COLOUR,PIXEL_OFF);
		}

		/* the other half of the pixels blue */
		else if((millis()%12000) < 4000)
		{
				NeoPixelSetState(PROG_RUN_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(NOUN_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(VERB_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(COMP_ACTY_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(TEMP_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(GIMBAL_LOCK_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(PROG_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(RESTART_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(TRACKER_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(ALT_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(VEL_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(BLANK_1_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(BLANK_2_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(OPP_ERR_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(KEY_REL_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(STBY_PIXEL,BLUE_COLOUR,PIXEL_ON);
				NeoPixelSetState(NO_ATT_PIXEL,BLUE_COLOUR,PIXEL_OFF);
				NeoPixelSetState(UPLINK_ACTY_PIXEL,BLUE_COLOUR,PIXEL_ON);
		}
		/* half the pixels red */
		else if((millis()%12000) < 5000)
		{
				NeoPixelSetState(PROG_RUN_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(NOUN_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(VERB_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(COMP_ACTY_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(TEMP_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(GIMBAL_LOCK_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(PROG_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(RESTART_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(TRACKER_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(ALT_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(VEL_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(BLANK_1_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(BLANK_2_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(OPP_ERR_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(KEY_REL_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(STBY_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(NO_ATT_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(UPLINK_ACTY_PIXEL,RED_COLOUR,PIXEL_OFF);
		}

		/* the other half of the pixels red */
		else if((millis()%12000) < 6000)
		{
				NeoPixelSetState(PROG_RUN_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(NOUN_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(VERB_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(COMP_ACTY_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(TEMP_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(GIMBAL_LOCK_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(PROG_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(RESTART_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(TRACKER_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(ALT_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(VEL_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(BLANK_1_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(BLANK_2_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(OPP_ERR_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(KEY_REL_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(STBY_PIXEL,RED_COLOUR,PIXEL_ON);
				NeoPixelSetState(NO_ATT_PIXEL,RED_COLOUR,PIXEL_OFF);
				NeoPixelSetState(UPLINK_ACTY_PIXEL,RED_COLOUR,PIXEL_ON);
		}

		/* half the pixels green */
		else if((millis()%12000) < 7000)
		{
				NeoPixelSetState(PROG_RUN_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(NOUN_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(VERB_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(COMP_ACTY_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(TEMP_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(GIMBAL_LOCK_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(PROG_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(RESTART_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(TRACKER_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(ALT_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(VEL_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(BLANK_1_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(BLANK_2_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(OPP_ERR_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(KEY_REL_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(STBY_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(NO_ATT_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(UPLINK_ACTY_PIXEL,GREEN_COLOUR,PIXEL_OFF);
		}

		/* the other half of the pixels green */
		else if((millis()%12000) < 8000)
		{
				NeoPixelSetState(PROG_RUN_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(NOUN_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(VERB_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(COMP_ACTY_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(TEMP_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(GIMBAL_LOCK_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(PROG_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(RESTART_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(TRACKER_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(ALT_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(VEL_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(BLANK_1_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(BLANK_2_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(OPP_ERR_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(KEY_REL_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(STBY_PIXEL,GREEN_COLOUR,PIXEL_ON);
				NeoPixelSetState(NO_ATT_PIXEL,GREEN_COLOUR,PIXEL_OFF);
				NeoPixelSetState(UPLINK_ACTY_PIXEL,GREEN_COLOUR,PIXEL_ON);
		}
		/* full colour cycle */
		else
		{
				NeoPixelSetState(PROG_RUN_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(NOUN_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(VERB_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(COMP_ACTY_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(TEMP_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(GIMBAL_LOCK_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(PROG_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(RESTART_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(TRACKER_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(ALT_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(VEL_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(BLANK_1_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(BLANK_2_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(OPP_ERR_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(KEY_REL_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(STBY_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(NO_ATT_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
				NeoPixelSetState(UPLINK_ACTY_PIXEL,RAINBOW_COLOUR,PIXEL_ON);
		}

}

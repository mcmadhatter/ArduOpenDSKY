
/*
 * Program.cpp
 *
 * Copyright 2018 Jeffrey Marten Gillmor
 *
 * OpenDSKY is a trademark of S&T GeoTronics LLC  https://opendsky.com
 *
 * This is the main program handler.
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
#include "Keyboard.h"
#include "Program.h"
#include "DiagnosticPrograms.h"
#include "TimePrograms.h"
#include "PositionPrograms.h"
#include "main.h"

#define MAX_NUMBER_ENTRIES 10
#define DEBUG
typedef enum KEYBOARD_MODE_EMUM
{
		VERB_ENTRY,
		NOUN_ENTRY,
		NUMBER_ENTRY,
		FINISH_ENTRY,
		NO_ENTRY,
		NUM_ENTRIES
} KeybordModeEnum;

typedef enum KEYBOARD_INT_ENUM
{
		POSITIVE_DECIMAL,
		NEGATIVE_DECMIAL,
		OCTAL
}KeyboardIntEnum;

static KeyboardIntEnum keyboardInt;
static KeysEnum pressedKey;
static uint32_t numberEntry;
static int32_t verb;
static int32_t noun;
static uint8_t numberIdx;
static bool minusPressed = false;
static KeybordModeEnum keyboardMode;

/* This structure contains all of the programs in the system. The noun number is
   optional. The program function is called when the verb and noun are entered. if
   the program should be called repeatedly, then you will need to setup a thread
   and callback function inside the program function. The set data function is
   used to give numerial data to the function. The Get Disp Data Function Should
   return display data for the 7 segment display driver. Run State is used internally
   by the Program module to track the state of all of the current programs.*/
ProgramStruct ProgramTable[] =
{
		/*verb  noun       Program Function        Set Data Function    Get Disp Data Function   Run State     Description */
		{ 35,  NOT_USED,  &V35BulbTest,             NULL,                NULL,                    NOT_RUNNING}, /* Bulb test */
		{ 30,  NOT_USED,  &V30BringToForeground,    &V30GiveData,        NULL,                    NOT_RUNNING}, /* Bring To foreground */
		{ 34,  NOT_USED,  &V34Terminate,            NULL,                NULL,                    NOT_RUNNING}, /* Terminate Program */
		{ 32,  NOT_USED,  &V32Reset,                NULL,                NULL,                    NOT_RUNNING}, /* Reset Program */
		{ 37,  36,        &V37N36SetAlarmProg,      &V37N36GiveData,     &TimeGetDisplayData,     NOT_RUNNING}, /* Set Alarm Program */
		{ 16,  36,        &V16N36ShowRTCTime,       NULL,                &TimeGetDisplayData,     NOT_RUNNING}, /* Show RTC Time on 7 segment */
		{ 25,  36,        &V25N36LoadManualTime,    &V25N36GiveData,     &TimeGetDisplayData,     NOT_RUNNING}, /* Set RTC manually */
		{ 26,  36,        &V26N36LoadGPSTime,       &V26N36GiveData,     &TimeGetDisplayData,     NOT_RUNNING}, /* Set RTC from GPS */
		{ 16,  43,        &V16N43ShowGPSPosition,   NULL,                &PositionGetDisplayData, NOT_RUNNING}, /* Show GPS poistion Data */
		{ 16,  29,        &V16N29ShowIMUGyro,       NULL,                &PositionGetDisplayData, NOT_RUNNING}, /* Show IMU Gyro Data */
		{ 16,  30,        &V16N30ShowIMUAcel,       NULL,                &PositionGetDisplayData, NOT_RUNNING}, /* Show IMU Acceloromter Data */
		{ 21,  98,        &V21N98SoundTest,         &V21N98GiveData,     NULL,                    NOT_RUNNING}  /* Sound test */
};

#define NUM_PROG_TABLE_ENTRIES  (sizeof(ProgramTable)/sizeof(ProgramStruct))

/**
 * GiveVerbToProgram
 *
 * Works out which program to call based on the verb data, if no prgram is found
 * then the verb data is saved foe later use when a noun is entered.
 * @param  newNoun noun to search for
 * @return         true if a program was found to accept the data. false otherwise
 */
bool GiveVerbToProgram(int16_t newVerb)
{
		uint16_t idx = 0;
		bool found = false;
#ifdef DEBUG
		Serial.print("verb number is ");
		Serial.println(newVerb);
#endif
		/* search through the program table to see if this verb is related to a
		   program, or whether it should be given to the currenty running program */
		while (( idx< NUM_PROG_TABLE_ENTRIES) && (!found))
		{
				if(ProgramTable[idx].VProgramNumber == newVerb)
				{
						found = true;
						verb = newVerb;
						if(ProgramTable[idx].NProgramNumber < 0)
						{
								ProgramTable[idx].CurrentRunState = ProgramTable[idx].Program(BRING_PROGRAM_TO_FOREGROUND);
						}
				}
				idx++;
		}

		return found;
}


/**
 * GiveNounToProgram
 *
 * Works out which program to call based on the previous verb data, and the noun
 * data given here.
 * @param  newNoun noun to search for
 * @return         true if a program was found to accept the data. false otherwise
 */
bool GiveNounToProgram(int16_t newNoun)
{
		uint16_t idx = 0;
		bool found = false;
#ifdef DEBUG
		Serial.print("noun number is ");
		Serial.println(newNoun);
#endif
		/* search through the program table to see if this verb is related to a
		     program, or whether it should be given to the currenty running program */
		while (( idx< NUM_PROG_TABLE_ENTRIES) && (!found))
		{
				if((ProgramTable[idx].VProgramNumber == verb) &&
				   (ProgramTable[idx].NProgramNumber == newNoun))
				{
						found = true;
						noun = newNoun;
						if(ProgramTable[idx].Program != NULL)
						{
								ProgramTable[idx].CurrentRunState = ProgramTable[idx].Program(BRING_PROGRAM_TO_FOREGROUND);
						}
				}
				idx++;
		}


		return found;
}

/**
 * GiveNumbersToProgram
 *
 * Gives number data to the current program in the FOREGROUND, returns true
 * if a program was found to accept the data.
 *
 * @param  number number to give to the program
 * @return        true if a program was found to accept the data. false otherwise
 */
bool GiveNumbersToProgram(int32_t number)
{
		uint16_t idx = 0;
		bool found = false;


		while (!found && (  idx < NUM_PROG_TABLE_ENTRIES))
		{
				if((ProgramTable[idx].CurrentRunState == FOREGROUND) &&
				   (ProgramTable[idx].SetData != NULL))
				{
						ProgramTable[idx].CurrentRunState = ProgramTable[idx].SetData(numberIdx, number);
						found = true;

#ifdef DEBUG
						Serial.print("Send Data to:V");
						Serial.print(ProgramTable[idx].VProgramNumber);
						Serial.print("N");
						Serial.print(ProgramTable[idx].NProgramNumber);
						Serial.print(" I ");
						Serial.print(numberIdx);
						Serial.print(" D ");
						Serial.println(number);
#endif
				}
				idx++;
		}

		return found;
}

/**
 * ProgramTask
 *
 * Takes the keys from the keyboard, and works out what to do with the keypresses
 * It then uses the other functions in this module to send the data to the
 * correct program.
 */
void ProgramTask(void)
{
		KeysEnum temporaryKey = GetPressedKey();

		if(pressedKey != temporaryKey)
		{
				switch(temporaryKey)
				{
				case KEY_CLEAR:
						/* clear resets the current number entry */
						numberIdx = 0;
						break;

				case KEY_VERB:
						/* Enter verb mode */
						keyboardMode = VERB_ENTRY;
						numberEntry = 0U;
						numberIdx = 0U;
						keyboardInt = OCTAL;
						break;

				case KEY_NOUN:
						/* if entering a noun, give the previous verb to a program */
						if(keyboardMode == VERB_ENTRY)
						{
								verb = numberEntry;
								numberEntry = 0U;
								/* Enter noun mode */
								keyboardMode = NOUN_ENTRY;
								numberIdx = 0U;
								(void)GiveVerbToProgram(verb);
						}
						break;

				case KEY_ENTER:
						if(keyboardMode == VERB_ENTRY)
						{
								/* Enter with just a verb gives the verb to a program */
								verb = numberEntry;
								numberEntry = 0U;
								numberIdx = 0U;
								(void)GiveVerbToProgram(verb);
						}
						else if(keyboardMode == NOUN_ENTRY)
						{
								/* Enter after a noun gives the noun to a program */
								noun = numberEntry;
								numberEntry = 0U;
								numberIdx = 0U;
								(void)GiveNounToProgram(noun);
						}
						else if(keyboardMode == NUMBER_ENTRY)
						{
								/* Otherwise in number entry mode */
								if(!GiveNumbersToProgram(numberEntry))
								{
										keyboardMode = NO_ENTRY;
										numberIdx = 0U;
								}
								else
								{
										numberIdx++;
								}
								numberEntry = 0U;
						}
						else
						{
								numberEntry = 0U;
						}
						keyboardMode = NUMBER_ENTRY;
						break;

				case KEY_0:
				case KEY_1:
				case KEY_2:
				case KEY_3:
				case KEY_4:
				case KEY_5:
				case KEY_6:
				case KEY_7:
				case KEY_8:
				case KEY_9:
						numberEntry*=10;
						numberEntry+=(int32_t)temporaryKey;
						if((minusPressed) && (numberEntry > 0))
						{
								numberEntry*=-1;
								minusPressed = false;
						}
						break;

				case KEY_MINUS:
						if(numberEntry >0)
						{
								numberEntry*=-1;
						}
						else
						{
								keyboardInt = NEGATIVE_DECMIAL;
						}
						break;

				case KEY_PLUS:
						keyboardInt = POSITIVE_DECIMAL;
						break;

				default:
						break;
				}
		}
		pressedKey = temporaryKey;
}


/**
 * SetProgram
 *
 * Sets the state of the program specifed by the verb and noun numbers. It will
 * search through the program table to find a matching verb noun combo before
 * actioning the request. The noun can be -1 if it is to be ignored.
 * @param  verbNumber verb of the program to set
 * @param  nounNumber noun of the program to set (can be -1 to be ignored)
 * @param  callState  ProgramCallStateEnum to give to the program.
 * @return            true if a program was found, false otherwise.
 */
bool SetProgram(int16_t verbNumber, int16_t nounNumber, ProgramCallStateEnum callState)
{
		bool newForeground = false;
		bool found = false;

		if((callState == BRING_PROGRAM_TO_FOREGROUND) ||
		   (callState == START_PROGRAM) )
		{
				newForeground = true;
		}

		for (uint16_t idx = 0; idx< NUM_PROG_TABLE_ENTRIES; idx++)
		{
				if((ProgramTable[idx].VProgramNumber == verbNumber) && (ProgramTable[idx].NProgramNumber == nounNumber))
				{

						if(ProgramTable[idx].Program != NULL )
						{
								ProgramTable[idx].CurrentRunState =  ProgramTable[idx].Program(callState);
								found = true;
						}
				}
				/* push the old foreground program to the background */
				else if((ProgramTable[idx].CurrentRunState == FOREGROUND) && newForeground)
				{
						if(ProgramTable[idx].Program != NULL )
						{
								ProgramTable[idx].CurrentRunState = ProgramTable[idx].Program(PUSH_PROGRAM_TO_BACKGROUND);
						}
				}
				else
				{
						/* do nothing */
				}

		}

		return found;
}


/**
 * ProgramGetDisplayData
 *
 * @return  This returns a pointer to the display data for whichever program is
 * currently in the FOREGROUND.
 */
SevenSegmentDisplayStruct* ProgramGetDisplayData(void)
{
		uint16_t idx = 0;
		bool found = false;


		while (  idx < NUM_PROG_TABLE_ENTRIES)
		{
				if((ProgramTable[idx].CurrentRunState == FOREGROUND) &&
				   (ProgramTable[idx].GetDisplayData != NULL))
				{
						return ProgramTable[idx].GetDisplayData();
				}
				idx++;
		}

		return NULL;
}


/**
 * V32Reset
 *
 * This resets whichever program is in the foreground
 * @param  call - a ProgramCallStateEnum this should be START_PROGRAM
 * @return - a ProgramRunStateEnum with whether the program is running in the
 *           foreground, background or not at all.
 */
ProgramRunStateEnum V32Reset(ProgramCallStateEnum call)
{
		uint16_t idx = 0;

		while (  idx < NUM_PROG_TABLE_ENTRIES)
		{
				if(ProgramTable[idx].CurrentRunState == FOREGROUND)
				{
						ProgramTable[idx].CurrentRunState = ProgramTable[idx].Program(RESET_PROGRAM);
						return NOT_RUNNING;
				}
				idx++;
		}

		return NOT_RUNNING;
}


/**
 * V34Terminate
 *
 * This terminates whichever program is in the foreground
 * @param  call - a ProgramCallStateEnum this should be START_PROGRAM
 * @return - a ProgramRunStateEnum with whether the program is running in the
 *           foreground, background or not at all.
 */
ProgramRunStateEnum V34Terminate(ProgramCallStateEnum call)
{
		uint16_t idx = 0;

		while (  idx < NUM_PROG_TABLE_ENTRIES)
		{
				if(ProgramTable[idx].CurrentRunState == FOREGROUND)
				{
						ProgramTable[idx].CurrentRunState = ProgramTable[idx].Program(STOP_PROGRAM);
						return NOT_RUNNING;
				}
				idx++;
		}

		return NOT_RUNNING;
}


/**
 * V30BringToForeground
 *
 * This program is used to bring other programs to the foreground.
 * @param  call - a ProgramCallStateEnum this should be START_PROGRAM
 * @return - a ProgramRunStateEnum with whether the program is running in the
 *           foreground, background or not at all.
 */
ProgramRunStateEnum V30BringToForeground(ProgramCallStateEnum call)
{
		uint16_t idx = 0;

		while (  idx < NUM_PROG_TABLE_ENTRIES)
		{
				if(ProgramTable[idx].CurrentRunState == FOREGROUND)
				{
						ProgramTable[idx].CurrentRunState = ProgramTable[idx].Program(BRING_PROGRAM_TO_FOREGROUND);
						return NOT_RUNNING;
				}
				idx++;
		}

		return NOT_RUNNING;
}


/**
 * V30GiveData
 *
 * V30 brings the program specified by the verb in dataIdx 0 and noun in dataIdx
 * 1 to the foregorund. If the specied NOUN is -1 then it is ignored.
 * @param  dataIdx 0 when setting the ver, 1 when setting the noun
 * @param  data    verb/noun data, -1 to ignore.
 * @return         NOT_RUNNING
 */
ProgramRunStateEnum V30GiveData(uint8_t dataIdx,int32_t data)
{
		static int16_t verb;
		uint16_t idx = 0;
		if(dataIdx == 0)
		{
				while (  idx < NUM_PROG_TABLE_ENTRIES)
				{
						if((ProgramTable[idx].CurrentRunState != FOREGROUND) &&
						   (ProgramTable[idx].VProgramNumber == data))
						{
								if(ProgramTable[idx].NProgramNumber == NOT_USED)
								{
										ProgramTable[idx].CurrentRunState = ProgramTable[idx].Program(BRING_PROGRAM_TO_FOREGROUND);
										return NOT_RUNNING;
								}
								else
								{
										verb = data;
								}
						}
						idx++;
				}
		}
		else
		{
				while (  idx < NUM_PROG_TABLE_ENTRIES)
				{
						if((ProgramTable[idx].CurrentRunState != FOREGROUND) &&
						   (ProgramTable[idx].VProgramNumber == verb) && (ProgramTable[idx].NProgramNumber == data))
						{
								ProgramTable[idx].CurrentRunState = ProgramTable[idx].Program(BRING_PROGRAM_TO_FOREGROUND);
								return NOT_RUNNING;
						}
						idx++;
				}
		}
		return NOT_RUNNING;
}


/**
 * ThreadDoNothing
 *
 * Literally does nothing, allows other threads to pause.
 */
void ThreadDoNothing(void)
{
		/* Do nothing */
}

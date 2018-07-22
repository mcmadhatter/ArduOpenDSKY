/*
 * Program.h
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
#ifndef PROGRAM_H
#define PROGRAM_H


#include "SevenSegment.h"


#define NOT_USED -1
#define NOT_SET -1

typedef enum PROGRAM_RUN_STATE_ENUM
{
		FOREGROUND,
		BACKGROUND,
		NOT_RUNNING,
		NUM_RUN_STATES
}ProgramRunStateEnum;

typedef enum PROGRAM_CALL_STATE_ENUM
{
		START_PROGRAM,
		PAUSE_PROGRAM,
		UNPAUSE_PROGRAM,
		STOP_PROGRAM,
		PUSH_PROGRAM_TO_BACKGROUND,
		BRING_PROGRAM_TO_FOREGROUND,
		RESET_PROGRAM,
		NUM_CALL_STATES
}ProgramCallStateEnum;

typedef struct PROGRAM_STRUCT
{
		int16_t VProgramNumber;
		int16_t NProgramNumber;
		ProgramRunStateEnum (*Program)(ProgramCallStateEnum Call);
		ProgramRunStateEnum (*SetData)(uint8_t DataIdx, int32_t data);
		SevenSegmentDisplayStruct* (*GetDisplayData)(void);
		ProgramRunStateEnum CurrentRunState;


}ProgramStruct;


extern ProgramStruct ProgramTable[];
extern void ProgramSetup(void);
extern void ProgramTask(void);
extern void ThreadDoNothing(void);
extern bool SetProgram(int16_t VerbNumber, int16_t NounNumber, ProgramCallStateEnum callState);
extern SevenSegmentDisplayStruct* ProgramGetDisplayData(void);
extern ProgramRunStateEnum V30BringToForeground(ProgramCallStateEnum Call);
extern ProgramRunStateEnum V34Terminate(ProgramCallStateEnum Call);
extern ProgramRunStateEnum V32Reset(ProgramCallStateEnum Call);
extern ProgramRunStateEnum V30GiveData(uint8_t DataIdx,int32_t data);
#endif

/*
 * TimeProgram.h
 *
 * This file is part of ArduOpenDSKY - the open source software for
 * the OpenDSKY.
 *
 * Software Copyright 2018 Jeffrey Marten Gillmor
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
#ifndef TIME_PROGRAM_H
#define TIME_PROGRAM_H

extern SevenSegmentDisplayStruct* TimeGetDisplayData(void);

extern void V16N36ThreadCallback(void);
extern ProgramRunStateEnum V25N36GiveData(uint8_t dataIdx,int32_t data);
extern ProgramRunStateEnum V25N36LoadManualTime(ProgramCallStateEnum call);
extern ProgramRunStateEnum V16N36ShowRTCTime(ProgramCallStateEnum call);


extern ProgramRunStateEnum V26N36LoadGPSTime(ProgramCallStateEnum call);
extern ProgramRunStateEnum V26N36GiveData(uint8_t dataIdx,int32_t data);
extern ProgramRunStateEnum V37N36SetAlarmProg(ProgramCallStateEnum call);
extern ProgramRunStateEnum V37N36GiveData(uint8_t dataIdx,int32_t data);

#endif

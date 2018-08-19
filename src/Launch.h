/*
 * Launch.h
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
#ifndef LAUNCH_PROGRAM_H
#define LAUNCH_PROGRAM_H

extern SevenSegmentDisplayStruct* LaunchGetDisplayData(void);

extern void V37N02ThreadCallback(void);
extern void V16N65LaunchThreadCallback(void);
extern ProgramRunStateEnum V16N65Launch(ProgramCallStateEnum call);
extern ProgramRunStateEnum V37N02GiveData(uint8_t dataIdx,int32_t data);
extern ProgramRunStateEnum V37N02Launch(ProgramCallStateEnum call);

#endif

/*
 * PositionPrograms.h
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

 #ifndef POSITION_PROGRAM_H
 #define POSITION_PROGRAM_H


extern SevenSegmentDisplayStruct* PositionGetDisplayData(void);
extern void V16N43ThreadCallback(void);
extern ProgramRunStateEnum V16N43ShowGPSPosition(ProgramCallStateEnum call);

extern void V16N29ThreadCallback(void);
extern ProgramRunStateEnum V16N29ShowIMUGyro(ProgramCallStateEnum call);

extern void V16N30ThreadCallback(void);
extern ProgramRunStateEnum V16N30ShowIMUAcel(ProgramCallStateEnum call);
#endif /* POSITION_PROGRAM_H */

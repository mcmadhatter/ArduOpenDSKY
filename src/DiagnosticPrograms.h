/*
 * DiagnosticPrograms.h
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

#ifndef DIAGNOSTIC_PROGRAMS_H
#define DIAGNOSTIC_PROGRAMS_H

extern ProgramRunStateEnum V35BulbTest(ProgramCallStateEnum call);
extern void V35ThreadCallback();
extern ProgramRunStateEnum V21N98GiveData(uint8_t dataIdx,int32_t data);
extern ProgramRunStateEnum V21N98SoundTest(ProgramCallStateEnum call);
extern SevenSegmentDisplayStruct* DiagnosticGetDisplayData(void);
#endif

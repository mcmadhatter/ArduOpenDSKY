/*
 * SevenSegment.h
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
#ifndef SEVEN_SEGMENT_H
#define SEVEN_SEGMENT_H
typedef struct SEVEN_SEGMENT_DISPLAY_STRUCT
{
		int32_t R1;
		int32_t R2;
		int32_t R3;
		uint8_t Verb;
		uint8_t Noun;
		uint8_t Prog;
		uint8_t R1DigitShowMask;
		uint8_t R2DigitShowMask;
		uint8_t R3DigitShowMask;
}SevenSegmentDisplayStruct;


extern void SevenSegmentSetup(void);
#endif /*SEVEN_SEGMENT_H */

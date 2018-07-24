/*
 * RTC.h
 *
 * This file is part of ArduOpenDSKY - the open source software for
 * the OpenDSKY.
 *
 * Driver for the RTC module, and alarm clock handler
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

#ifndef RTC_H
#define RTC_H


#define LARGEST_HOUR_IN_A_DAY       23U
#define LARGEST_MINUTE_IN_AN_HOUR   59U
#define LARGEST_SECOND_IN_A_MINUTE  59U

#define ALARM_IGNORE 0xff

#define HoursMinutesSecondsDayOfMonthMatch  0x00
#define OncePerSecond  0x17
#define SecondsMatch 0x16
#define MinutesSecondsMatch 0x14
#define HoursMinutesSecondsMatch  0x10
#define HoursMinutesSecondsDayOfWeekMatch 0x08
#define DoNotRepeat 0x00


extern void RtcSetHour(uint8_t hour);
extern void RtcSetMinute(uint8_t minute);
extern void RtcSetSecond(uint8_t second);
extern void RtcSetYear(uint16_t year);
extern void RtcSetMonth(uint8_t month);
extern void RtcSetDay(uint8_t day);
extern void RTCSetup(void);
extern void RtcSetDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
extern void RtcGetDateTime(uint16_t* year, uint8_t* month, uint8_t* day, uint8_t* hour, uint8_t* minute, uint8_t* second);
extern void RtcSetAlarmProgram(uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint16_t verb, uint16_t noun, int32_t progData,  uint8_t repeat);
#endif

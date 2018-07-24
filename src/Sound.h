/*
 * Sound.h
 *
 * Copyright 2018 Jeffrey Marten Gillmor
 *
 * OpenDSKY is a trademark of S&T GeoTronics LLC  https://opendsky.com
 *
 * Driver to allow sounds to be played either the skip to next track button
 * can be used, or optionally with some wire mods to the openDSKY, the
 * softare serial can be used to TX commands.
 *
 * NASA have made some great sounds avialable here:
 * https://www.nasa.gov/connect/sounds/index.html
 *
 * Don't forget to update the TracksEnum in Sound.h to match the tracks you
 * have put onto the microSDcard.
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
#ifndef SOUND_H
#define SOUND_H


typedef enum TRACKS_ENUM
{
		HOUSTON,
		COUNTDOWN,
		LANDING,
		NUM_TRACKS
}TracksEnum;

extern void SoundSetup(void);
extern bool PlayTrack(uint16_t TrackNumber);
extern void SoundThreadCallback(void);
#endif

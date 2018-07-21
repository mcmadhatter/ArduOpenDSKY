/*
 * Neopixels.h
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

#ifndef NEOPIXELS_H
#define NEOPIXELS_H

typedef enum NEOPIXEL_WORD
{
		PROG_RUN_PIXEL,
		NOUN_PIXEL,
		VERB_PIXEL,
		COMP_ACTY_PIXEL,
		TEMP_PIXEL,
		GIMBAL_LOCK_PIXEL,
		PROG_PIXEL,
		RESTART_PIXEL,
		TRACKER_PIXEL,
		ALT_PIXEL,
		VEL_PIXEL,
		BLANK_1_PIXEL,
		BLANK_2_PIXEL,
		OPP_ERR_PIXEL,
		KEY_REL_PIXEL,
		STBY_PIXEL,
		NO_ATT_PIXEL,
		UPLINK_ACTY_PIXEL,
		NUM_PIXELS
}NeopixelWordEnum;


typedef enum PIXEL_COLOURS_ENUM
{
		WARM_WHITE_COLOUR,
		COOL_WHITE_COLOUR,
		PURE_WHITE_COLOUR,
		RED_COLOUR,
		GREEN_COLOUR,
		BLUE_COLOUR,
		AMBER_COLOUR,
		YELLOW_COLOUR,
		CYAN_COLOUR,
		MAGENTA_COLOUR,
		RAINBOW_COLOUR,
		NUM_COLOURS
}PixelColoursEnum;

typedef enum PIXEL_ON_STATE_ENUM
{
		PIXEL_OFF,
		PIXEL_ON,
		PIXEL_SLOW_FLASH,
		PIXEL_FAST_FLASH,
		PIXEL_FADE_UP,
		PIXEL_FADE_DOWN,
		PIXEL_BREATH,
		PIXEL_RANDOM,
		NUM_PIXEL_ON_STATES
}PixelOnStateEnum;

extern void NeoPixelThreadCallback(void);
extern void NeoPixelSetup(void);
extern void NeoPixelSetState(NeopixelWordEnum Word, PixelColoursEnum Colour, PixelOnStateEnum State);
#endif

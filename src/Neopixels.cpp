/*
 * Neopixels.cpp
 *
 * Copyright 2018 Jeffrey Marten Gillmor
 *
 * OpenDSKY is a trademark of S&T GeoTronics LLC  https://opendsky.com
 *
 * Driver for the neopixels used on the OpenDSKY hardware, it allows individual
 * neopixels to be controlled by their openDSKY name. Both RGB and RGBW
 * neopixels are supported
 *
 * This modules uses the following libraries:
 *
 * Adafruit NeoPixel by Adafruit this should be available from here
 * https://github.com/adafruit/Adafruit_NeoPixel
 *
 * Wheel function has been taken from Adafruit's exmaple in the Adafruit
 * NeoPixel library
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

/* Arduino libraries */
#include <Adafruit_NeoPixel.h>
#include <Thread.h>
#include <ThreadController.h>
#include <TimerOne.h>

/* Open DSKY headers */
#include "Neopixels.h"
#include "main.h"

#define RGBW /* Use this if you have got RGBW (4 Colour) Neo Pixels, otherwise
	            it will default to 3 Colour ones */

#ifdef RGBW
Adafruit_NeoPixel strip = Adafruit_NeoPixel(18, 6, NEO_GRBW + NEO_KHZ800);
#else
Adafruit_NeoPixel strip = Adafruit_NeoPixel(18, 6, NEO_GRB + NEO_KHZ800);
#endif

#define TWO_HUNDRED_MILLISECONDS    20
#define FOUR_HUNDRED_MILLISECONDS   40
#define EIGHT_HUNDRED_MILLISECONDS  80

Thread* neoPixelThread = new Thread();

typedef struct NEOPIXEL_CONTROL_STRUCT
{
		uint8_t Colour;
		uint8_t OnState;
		uint8_t ActionCount;

}NeoPixelControlStruct;

NeoPixelControlStruct NeoPixelControlTable[NUM_PIXELS] = {0U,};

static void NeoPixelSetPixelColour(uint8_t idx, PixelColoursEnum Colour);
static uint32_t Wheel(byte wheelPos);


/**
 * NeoPixelSetup
 *
 * Sets up the neopixel driver library
 */
void NeoPixelSetup(void)
{
		strip.setBrightness(20);
		strip.begin();

		strip.show(); // Initialize all pixels to 'off'
		randomSeed(analogRead(0));
		neoPixelThread->onRun(NeoPixelThreadCallback);
		neoPixelThread->setInterval(20);
		controll.add(neoPixelThread);

}


/**
 * NeoPixelThreadCallback
 *
 * Main thread callback for the neopixel library. This get called frequently
 * so as to make sure any flashing and fading effects are smooth. Every 10ms
 * a counter is incremented (ActionCount) this can then be used by the effects
 * to drive their status.
 */
void NeoPixelThreadCallback(void)
{

		for(uint8_t idx = 0U; idx < (uint8_t)NUM_PIXELS; idx++)
		{
				NeoPixelControlTable[idx].ActionCount++;
				switch (NeoPixelControlTable[idx].OnState)
				{

				case PIXEL_ON:
						NeoPixelSetPixelColour(idx, (PixelColoursEnum)NeoPixelControlTable[idx].Colour);
						break;
				case PIXEL_SLOW_FLASH:
						if(NeoPixelControlTable[idx].ActionCount < FOUR_HUNDRED_MILLISECONDS)
						{
								NeoPixelSetPixelColour(idx, (PixelColoursEnum)NeoPixelControlTable[idx].Colour);
						}
						else if(NeoPixelControlTable[idx].ActionCount < EIGHT_HUNDRED_MILLISECONDS)
						{
#ifdef RGBW
								strip.setPixelColor(idx, strip.Color(0,0,0,0) );
#else
								strip.setPixelColor(idx, strip.Color(0,0,0) );
#endif
						}
						else
						{
								NeoPixelControlTable[idx].ActionCount = 0;
						}
						break;
				case PIXEL_FAST_FLASH:
						if(NeoPixelControlTable[idx].ActionCount < TWO_HUNDRED_MILLISECONDS)
						{
								NeoPixelSetPixelColour(idx, (PixelColoursEnum)NeoPixelControlTable[idx].Colour);
						}
						else if(NeoPixelControlTable[idx].ActionCount < FOUR_HUNDRED_MILLISECONDS)
						{
#ifdef RGBW
								strip.setPixelColor(idx, strip.Color(0,0,0,0) );
#else
								strip.setPixelColor(idx, strip.Color(0,0,0) );
#endif
						}
						else
						{
								NeoPixelControlTable[idx].ActionCount = 0;
						}
						break;
						break;
				case PIXEL_FADE_UP:
				case PIXEL_FADE_DOWN:
				case PIXEL_BREATH:
						/* not yet supported, need to do hv<->rgb for that */
						break;
				case PIXEL_RANDOM:
						if(random(255) > NeoPixelControlTable[idx].ActionCount)
						{
								if(random(2) == 0)
								{
										NeoPixelSetPixelColour(idx, (PixelColoursEnum)NeoPixelControlTable[idx].Colour);
								}
								else
								{
#ifdef RGBW
										strip.setPixelColor(idx, strip.Color(0,0,0,0) );
#else
										strip.setPixelColor(idx, strip.Color(0,0,0) );
#endif
								}
						}

						break;
				case PIXEL_OFF:
				default:
#ifdef RGBW
						strip.setPixelColor(idx, strip.Color(0,0,0,0) );
#else
						strip.setPixelColor(idx, strip.Color(0,0,0) );
#endif
						break;
				}
		}
}


/**
 * NeoPixelSetState
 *
 * This allows other modules and programs to set the state of an individual
 * neopixel
 * @param Word   NeopixelWordEnum with the bulb to set
 * @param Colour PixelColoursEnum with the colour to sey
 * @param State  PixelOnStateEnum with the effect to set
 */
void NeoPixelSetState(NeopixelWordEnum word, PixelColoursEnum colour, PixelOnStateEnum state)
{
		if((word < NUM_PIXELS) && (colour < NUM_COLOURS) && (state < NUM_PIXEL_ON_STATES))
		{

				if(NeoPixelControlTable[word].OnState != (uint8_t)state)
				{
						NeoPixelControlTable[word].ActionCount = 0;
				}
				NeoPixelControlTable[word].Colour = (uint8_t)colour;
				NeoPixelControlTable[word].OnState = (uint8_t)state;
		}
}

/**
 * NeoPixelSetPixelColour
 *
 *  Sets the specified pixel to the specified colour
 * @param idx    index of the pixel to set
 * @param Colour PixelColoursEnum with the colour to set to.
 */
static void NeoPixelSetPixelColour(uint8_t idx, PixelColoursEnum colour)
{

		switch (colour)
		{
		case WARM_WHITE_COLOUR:
#ifdef RGBW
				strip.setPixelColor(idx, strip.Color(240,240,0, 255) );
#else
				strip.setPixelColor(idx, strip.Color(255,255,220) );
#endif
				break;
		case COOL_WHITE_COLOUR:
#ifdef RGBW
				strip.setPixelColor(idx, strip.Color(255,255,255, 255) );
#else
				strip.setPixelColor(idx, strip.Color(255,255,255) );
#endif
				break;
		case PURE_WHITE_COLOUR:
#ifdef RGBW
				strip.setPixelColor(idx, strip.Color(0,0,0, 255) );
#else
				strip.setPixelColor(idx, strip.Color(255,255,255) );
#endif
				break;
		case RED_COLOUR:
#ifdef RGBW
				strip.setPixelColor(idx, strip.Color(255,0,0, 0) );
#else
				strip.setPixelColor(idx, strip.Color(255,0,0) );
#endif
				break;
		case GREEN_COLOUR:
#ifdef RGBW
				strip.setPixelColor(idx, strip.Color(0,255,0, 0) );
#else
				strip.setPixelColor(idx, strip.Color(0,255,0) );
#endif
				break;
		case AMBER_COLOUR:
#ifdef RGBW
				strip.setPixelColor(idx, strip.Color(255,90,0, 0) );
#else
				strip.setPixelColor(idx, strip.Color(255,90,0) );
#endif
				break;
		case BLUE_COLOUR:
#ifdef RGBW
				strip.setPixelColor(idx, strip.Color(0,0,255, 0) );
#else
				strip.setPixelColor(idx, strip.Color(0,0,255) );
#endif
				break;
		case YELLOW_COLOUR:
#ifdef RGBW
				strip.setPixelColor(idx, strip.Color(255,255,0, 0) );
#else
				strip.setPixelColor(idx, strip.Color(255,255,0) );
#endif
				break;
		case CYAN_COLOUR:
#ifdef RGBW
				strip.setPixelColor(idx, strip.Color(0,255,255, 0) );
#else
				strip.setPixelColor(idx, strip.Color(0,255,255) );
#endif
				break;
		case MAGENTA_COLOUR:
#ifdef RGBW
				strip.setPixelColor(idx, strip.Color(255,0,255, 0) );
#else
				strip.setPixelColor(idx, strip.Color(255,0,255) );
#endif
				break;
		case RAINBOW_COLOUR:
				strip.setPixelColor(idx, Wheel((NeoPixelControlTable[idx].ActionCount) & 255));
				NeoPixelControlTable[idx].ActionCount++;

				break;

		default:
				/* do nothing */
				break;
		}
		strip.show();
}


/**
 * Wheel
 *
 *  Used to cycle through all the colouts r - g - b
 *  Taken from the adafruit example code
 *
 * @param  wheelPos  Input a value 0 to 255 to get a colour value.
 * @return          A colour value
 */
static uint32_t Wheel(byte wheelPos) {
		wheelPos = 255 - wheelPos;
		if(wheelPos < 85)
		{
#ifdef RGBW
				return strip.Color(255 - wheelPos * 3, 0, wheelPos * 3,0);
#else
				return strip.Color(255 - wheelPos * 3, 0, wheelPos * 3);
#endif

		}
		if(wheelPos < 170)
		{
				wheelPos -= 85;
#ifdef RGBW
				return strip.Color(0, wheelPos * 3, 255 - wheelPos * 3,0);
#else
				return strip.Color(0, wheelPos * 3, 255 - wheelPos * 3);
#endif
		}
		wheelPos -= 170;
#ifdef RGBW
		return strip.Color(wheelPos * 3, 255 - wheelPos * 3, 0,0);
#else
		return strip.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
#endif
}

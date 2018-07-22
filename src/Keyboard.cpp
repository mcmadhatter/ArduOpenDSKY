/*
 * Keyboard.cpp
 *
 * Copyright 2018 Jeffrey Marten Gillmor
 *
 * OpenDSKY is a trademark of S&T GeoTronics LLC  https://opendsky.com
 *
 * Both the physical keyboard and a serial port based keyboard are supported.
 * Use #define SERIAL_KEYBOARD to allow keys to be read from the serial port
 * rather than pressing buttons. The serial keybaord does not play nicely with
 * the GPS driver, as they use the same serial lines.
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
#include <Wire.h>
#include <Thread.h>
#include <ThreadController.h>
#include <TimerOne.h>

/* Open DSKY headers */
#include "Program.h"
#include "Keyboard.h"
#include "main.h"

#define SERIAL_KEYBOARD

static KeysEnum currentKey;
static KeysEnum previousKey;
static KeysEnum pressedKey;

Thread* keyboardThread = new Thread();
static KeysEnum ReadKeys(void);

#ifdef SERIAL_KEYBOARD
static void ReadSerialKeys(void);
#endif

/**
 * KeyboardSetup
 *
 * Setup the keyboard drvier module.
 */
void KeyboardSetup(void)
{
		pinMode(A0, INPUT);
		pinMode(A1, INPUT);
		pinMode(A2, INPUT);

		keyboardThread->onRun(KeyboardTask);
		keyboardThread->setInterval(10);
		controll.add(keyboardThread);

		currentKey = KEY_NONE;
		previousKey = KEY_NONE;
		pressedKey  = KEY_NONE;
}

/**
 * KeyboardTask
 *
 * Read and debounce the current key, this task gets run really often but
 */
void KeyboardTask(void)
{
		static int8_t currentKeyCount = 0;
		previousKey = currentKey;

#ifdef SERIAL_KEYBOARD
		ReadSerialKeys();
#else
		currentKey = ReadKeys();

/* debounce the key reading */
		if(previousKey != currentKey)
		{
				if(currentKeyCount > 0)
				{
						currentKeyCount-=3;
				}
		}
		else
		{
				if(currentKeyCount < 10)
				{
						currentKeyCount++;
				}
				if(currentKeyCount > 5)
				{
						pressedKey = currentKey;
				}

		}
#endif
}

/**
 * GetPressedKey
 *
 * @return  returns the currently pressed key
 */
KeysEnum GetPressedKey(void)
{
		return pressedKey;
}

/**
 * ReadKeys
 *
 * Read the analog ports and work out which keys are pressed. NKRO is not
 * supported, instead first key wins.
 * @return  the pressed key
 */
static KeysEnum ReadKeys(void)
{
		int value_row1 = analogRead(A0);
		int value_row2; /* do this later */
		int value_row3; /* do this later */

		if (value_row1 < 225)
		{
#ifdef DEBUG
				Serial.println("Verb");
#endif
				return KEY_VERB;
		}
		else if (value_row1 < 370)
		{
#ifdef DEBUG
				Serial.println("+");
#endif
				return KEY_PLUS;
		}
		else if (value_row1 < 510)
		{
#ifdef DEBUG
				Serial.println("7");
#endif
				return KEY_7;
		}
		else if (value_row1 < 650)
		{
#ifdef DEBUG
				Serial.println("8");
#endif
				return KEY_8;
		}
		else if (value_row1 < 790)
		{
#ifdef DEBUG
				Serial.println("9");
#endif
				return KEY_9;
		}
		else if (value_row1 < 930)
		{
#ifdef DEBUG
				Serial.println("Clear");
#endif
				return KEY_CLEAR;
		}
		else
		{
				/* no row 1 pressed */
		}
		value_row2 = analogRead(A1);


		if (value_row2 < 200)
		{
#ifdef DEBUG
				Serial.println("Noun");
#endif
				return KEY_NOUN;
		}
		else if (value_row2 < 330)
		{
#ifdef DEBUG
				Serial.println("-");
#endif
				return KEY_MINUS;
		}
		else if (value_row2 < 455)
		{
#ifdef DEBUG
				Serial.println("4");
#endif
				return KEY_4;
		}
		else if (value_row2 < 577)
		{
#ifdef DEBUG
				Serial.println("5");
#endif
				return KEY_5;
		}
		else if (value_row2 < 700)
		{
#ifdef DEBUG
				Serial.println("6");
#endif
				return KEY_6;
		}
		else if (value_row2 < 823)
		{
#ifdef DEBUG
				Serial.println("Proceed");
#endif
				return KEY_PROCEED;
		}
		else if (value_row2 < 930)
		{
#ifdef DEBUG
				Serial.println("Enter");
#endif
				return KEY_ENTER;
		}
		else
		{
				/* no row 2 pressed */
		}


		value_row3 = analogRead(A2);
		if (value_row3 < 225)
		{
#ifdef DEBUG
				Serial.println("0");
#endif
				return KEY_0;
		}
		else if (value_row3 < 370)
		{
#ifdef DEBUG
				Serial.println("1");
#endif
				return KEY_1;
		}

		else if (value_row3 < 510)
		{
#ifdef DEBUG
				Serial.println("2");
#endif
				return KEY_2;
		}
		else if (value_row3 < 650)
		{
#ifdef DEBUG
				Serial.println("3");
#endif
				return KEY_3;
		}
		else if (value_row3 < 790)
		{
#ifdef DEBUG
				Serial.println("Releae");
#endif
				return KEY_RELEASE;
		}
		else if (value_row3 < 930)
		{
#ifdef DEBUG
				Serial.println("Reset");
#endif
				return KEY_RESET;
		}
		else
		{
			#ifdef DEBUG
				Serial.print(value_row1);
				Serial.print("\t");
				Serial.print(value_row2);
				Serial.print("\t");
				Serial.println(value_row3);
			#endif
				return KEY_NONE;
		}
}


#ifdef SERIAL_KEYBOARD
/**
 * ReadSerialKeys
 *
 * Read key presses from the serial port and update the pressedKey
 */
static void ReadSerialKeys(void)
{

		if(Serial.available() > 0)
		{

				switch(Serial.read())
				{
				case 'V':
				case 'v':
						pressedKey = KEY_VERB;
						break;

				case 'n':
				case 'N':
						pressedKey = KEY_NOUN;
						break;


				case '+':
						pressedKey = KEY_PLUS;
						break;

				case '-':
						pressedKey = KEY_MINUS;
						break;

				case '0':
						pressedKey = KEY_0;
						break;

				case '1':
						pressedKey = KEY_1;
						break;

				case '2':
						pressedKey = KEY_2;
						break;

				case '3':
						pressedKey = KEY_3;
						break;

				case '4':
						pressedKey = KEY_4;
						break;

				case '5':
						pressedKey = KEY_5;
						break;

				case '6':
						pressedKey = KEY_6;
						break;

				case '7':
						pressedKey = KEY_7;
						break;

				case '8':
						pressedKey = KEY_8;
						break;

				case '9':
						pressedKey = KEY_9;
						break;

				case '\r':
				case ' ':
						pressedKey = KEY_ENTER;
						break;

				case 'd':
						pressedKey = KEY_CLEAR;
						break;

				case 'p':
						pressedKey = KEY_PROCEED;
						break;

				case 'r':
						pressedKey = KEY_RELEASE;
						break;

				case 't':
						pressedKey = KEY_RESET;
						break;

				default:
						pressedKey = KEY_NONE;
						break;
				}
		}
		else
		{
				pressedKey = KEY_NONE;
		}

}
#endif

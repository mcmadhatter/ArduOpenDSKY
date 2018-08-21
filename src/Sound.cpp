/*
 * Sound.cpp
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

#include <Wire.h>
#include <Thread.h>
#include <SoftwareSerial.h>
#include <ThreadController.h>
#include <TimerOne.h>
#include "Sound.h"
#include "main.h"


static bool packetSending = false;


/*#define TX_WIRE_MOD */ /* Uncomment this if you have modified your openDSKY to
                           connect the TX line on the DFPlayer */

#ifdef TX_WIRE_MOD
typedef enum SOUND_PACKET_BYTES_ENUM
{
		STARTBYTE_IDX,
		VERSION_IDX,
		LENGTH_IDX,
		COMMAND_IDX,
		FEEDBACK_IDX,
		PARA1_IDX,
		PARA2_IDX,
		CHECKSUM1_IDX,
		CHECKSUM2_IDX,
		ENDBYTE_IDX,
		NUM_SOUND_BYTES

}SoundPacketBytesEnum;
SoftwareSerial soundSerial(4, 5); // RX, TX
static uint16_t SoundCalculateCheckum(void);
static void SendPacket(void);
static bool SingleCommand(uint8_t commndNumber);
static uint8_t packet[NUM_SOUND_BYTES] = {0x7EU, 0xFFU, 0U, 0U, 0U, 0U, 0U, 0U, 0xEFU};
#else
#include <digitalWriteFast.h>
static TracksEnum currentTrack = NUM_TRACKS;
static TracksEnum requestedTrack = NUM_TRACKS;
Thread* SoundThread = new Thread();

#endif

/**
 * SoundSetup
 *
 * Setup the sound driver
 */
void SoundSetup(void)
{
#ifdef TX_WIRE_MOD
		soundSerial.begin(9600);
#else
		pinModeFast(9, OUTPUT);
		digitalWriteFast(9, LOW);
		SoundThread->onRun(SoundThreadCallback);
		SoundThread->setInterval(100);
		controll.add(SoundThread);
#endif
}

#ifdef TX_WIRE_MOD
/**
 * SoundCalculateCheckum
 *
 * Calculate the checksum needed to transmit serial messages to the DFPlayerMini
 * @return  the checksum
 */
static uint16_t SoundCalculateCheckum(void)
{
		uint16_t returnValue = 0U;
		for(uint8_t idx = (uint8_t)VERSION_IDX; idx < (uint8_t)CHECKSUM1_IDX; idx++)
		{
				returnValue += packet[idx];
		}
		return -returnValue;
}

/**
 * SendPacket
 *
 * Send a packet to the DFPlayerMini via the software serial port
 */
static void SendPacket(void)
{
		packetSending = true;
		for(uint8_t idx = (uint8_t)STARTBYTE_IDX; idx < (uint8_t)NUM_SOUND_BYTES; idx++)
		{
				soundSerial.write(packet[idx]);
		}
		packetSending = false;
}


/**
 * SingleCommand
 *
 * Sends a single command to DFPlayerMini via the software serial port
 * @param  commandNumber command to send
 * @return  true if succesful false otherwise
 */
static bool SingleCommand(uint8_t commandNumber)
{
		if(!packetSending)
		{
				packet[STARTBYTE_IDX] = 0x7E;
				packet[VERSION_IDX] = 0xFF;
				packet[LENGTH_IDX] = 0x6;
				packet[COMMAND_IDX] = commandNumber;
				packet[FEEDBACK_IDX] = 0x0;
				packet[PARA1_IDX] = 0U;
				packet[PARA2_IDX] = 0U;
				uint16_t checksum = SoundCalculateCheckum();
				packet[CHECKSUM1_IDX] = (checksum >> 8) & 0xFFU;
				packet[CHECKSUM2_IDX] = checksum & 0xFFU;
				packet[ENDBYTE_IDX] = 0xEF;
				SendPacket();
				return true;
		}
		return false;
}
#endif


/**
 * PauseTrack
 *
 * Pauses the currently playing track
 * @return  true if succesful false otherwise
 */
bool PauseTrack(void)
{
#ifdef TX_WIRE_MOD
		return SingleCommand(0x0EU);
#else
		return false;
#endif
}

/**
 * UnPauseTrack
 *
 * UnPauses the currently playing track
 * @return  true if succesful false otherwise
 */
bool UnPauseTrack(void)
{
#ifdef TX_WIRE_MOD
		return SingleCommand(0x0DU);
#else
		return false;
#endif
}


/**
 * PlayTrack
 *
 * Plays the requested track number
 * @param  trackNumber to play
 * @return  true if succesful false otherwise
 */
bool PlayTrack(uint16_t trackNumber)
{
		if(!packetSending)
		{
#ifdef TX_WIRE_MOD
				packet[STARTBYTE_IDX] = 0x7E;
				packet[VERSION_IDX] = 0xFF;
				packet[LENGTH_IDX] = 0x6;
				packet[COMMAND_IDX] = 0x3;
				packet[FEEDBACK_IDX] = 0x0;
				packet[PARA1_IDX] = (trackNumber >> 8) & 0xFFU;
				packet[PARA2_IDX] = trackNumber & 0xFFU;
				uint16_t checksum = SoundCalculateCheckum();
				packet[CHECKSUM1_IDX] = (checksum >> 8) & 0xFFU;
				packet[CHECKSUM2_IDX] = checksum & 0xFFU;
				packet[ENDBYTE_IDX] = 0xEF;
				SendPacket();
#else
				if(trackNumber < NUM_TRACKS)
				{
						requestedTrack = trackNumber;
				}

#endif
				return true;
		}
		return false;
}

/**
 * SoundThreadCallback
 *
 * Used to make sure the correct track is played when the skip button method is
 * used.
 */
void SoundThreadCallback(void)
{
#ifndef TX_WIRE_MOD
		static bool toggleState = false;

		if(currentTrack != requestedTrack)
		{
				if(toggleState)
				{
						toggleState = false;
						pinMode(9, OUTPUT);
						currentTrack = currentTrack + 1;
						currentTrack %= NUM_TRACKS;
				}
				else
				{
						toggleState = true;
						pinMode(9, INPUT);
				}
		}
#endif
}

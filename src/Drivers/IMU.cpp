/*
 * IMU.cpp
 *
 * Driver for regularly getting the IMU data.
 *
 * This driver requires the I2Cdevlib-MPU6050 library by Jeff Rowberg to be
 * installed. This should be avilable from here.
 * https://github.com/jrowberg/i2cdevlib
 *
 * This file is part of ArduOpenDSKY - the open source software for
 * the OpenDSKY.
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
 *
 */

#include <Wire.h>
#include <I2Cdev.h>
#include "MPU6050.h"
#include <helper_3dmath.h>
#include <Thread.h>
#include <ThreadController.h>
#include <TimerOne.h>

#include "../Drivers/IMU.h"
#include "../main.h"


#define INTERRUPT_PIN 2

MPU6050 mpu(0x69);

Thread* IMUThread = new Thread();

typedef struct IMU_DATA
{
		/* orientation/motion variables */
		Quaternion q;   // [w, x, y, z]         quaternion container
		VectorInt16 aa; // [x, y, z]            accel sensor measurements
		VectorInt16 aaReal; // [x, y, z]        gravity-free accel sensor measurements
		VectorInt16 aaWorld; // [x, y, z]       world-frame accel sensor measurements
		VectorFloat gravity; // [x, y, z]       gravity vector
		float euler[3]; // [psi, theta, phi]    Euler angle container
		float ypr[3];   // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
		VectorInt16 accelRaw;
		VectorInt16 gyroRaw;
}IMUDataStruct;


IMUDataStruct IMUData;


/**
 * IMUSetup
 *
 * Intitialise the IMU hardware, and setup the IMU sensor thread so that it gets
 * the IMU data 3 times a second.
 */
void IMUSetup(void )
{
		mpu.initialize();
		pinMode(INTERRUPT_PIN, INPUT);
		IMUThread->onRun(IMUThreadCallback);
		IMUThread->setInterval(300);
		controll.add(IMUThread);
}


/**
 * IMUThreadCallback
 *
 * The main callback thread for the IMUData.
 */
void IMUThreadCallback(void)
{
		/* Get the raw data from the IUM */
		mpu.getMotion6(&IMUData.accelRaw.x, &IMUData.accelRaw.y, &IMUData.accelRaw.z, &IMUData.gyroRaw.x, &IMUData.gyroRaw.y, &IMUData.gyroRaw.z);

#ifdef DEBUG
		Serial.print(IMUData.accelRaw.x); Serial.print("\t");
		Serial.print(IMUData.accelRaw.y); Serial.print("\t");
		Serial.print(IMUData.accelRaw.z); Serial.print("\t");
		Serial.print(IMUData.gyroRaw.x); Serial.print("\t");
		Serial.print(IMUData.gyroRaw.y); Serial.print("\t");
		Serial.println(IMUData.gyroRaw.z);
#endif
}


/**
 * GetAcelRaw
 *
 * @return  an array of 3 uint16_t containing the raw accelometer data
 */
VectorInt16 GetAcelRaw(void)
{
		return IMUData.accelRaw;
}


/**
 * GetGyroRaw
 *
 * @return  an array of 3 uint16_t containing the raw gyro data
 */
VectorInt16 GetGyroRaw(void)
{
		return IMUData.gyroRaw;
}

#include <Arduino.h>
#include "firmware.h"

#include <Wire.h>

Time rtc;

void loadRtcTime()
{
	Wire.beginTransmission(RTC_I2C_ADDRESS);
	Wire.write(0); /* base address */
	Wire.endTransmission();
	Wire.requestFrom(RTC_I2C_ADDRESS, 7);
	rtc.seconds = Wire.read(/*0*/) & 0x7F;
	rtc.minutes = Wire.read(/*1*/) & 0x7F;
	rtc.hours   = Wire.read(/*2*/) & 0x3F;
	rtc.pm = rtc.hours & 0x20;
	rtc.hours &= 0x1F;
	             Wire.read(/*3*/); /* unused */
	rtc.day     = Wire.read(/*4*/) & 0x3F;
	rtc.month   = Wire.read(/*5*/) & 0x1F;
	rtc.year    = Wire.read(/*6*/);
}

void setRtcTime(Time &time)
{
	/* stop RTC clock */
	Wire.beginTransmission(RTC_I2C_ADDRESS);
	Wire.write(0); /* starting address */
	Wire.write(/*0*/ 0);
	Wire.endTransmission();
	
	/* update time */
	Wire.beginTransmission(RTC_I2C_ADDRESS);
	Wire.write(1); /* starting address */
	Wire.write(/*1*/ time.minutes);
	Wire.write(/*2*/ time.hours | (time.pm ? 0x20 : 0) | 0x40);
	Wire.write(/*3*/ 0x08);  /* we don't care about the day of week, but enable VBAT mode */
	Wire.write(/*4*/ time.day);
	Wire.write(/*5*/ time.month);
	Wire.write(/*6*/ time.year);
	Wire.write(/*7*/ 0x40); /* enable 1Hz output */
	Wire.endTransmission();
	
	/* write seconds last and enable oscillator */
	Wire.beginTransmission(RTC_I2C_ADDRESS);
	Wire.write(0); /* starting address */
	Wire.write(/*0*/ time.seconds | 0x80);
	Wire.endTransmission();
	
	rtc = time;
}

static void updateFromRtc()
{
	interrupts();
	loadRtcTime();
	for (int i = 0; i < NUM_DISPLAYS; i++)
	{
		if (displays[i].useRtc)
		{
			setDisplay(displays + i, rtc);
		}
	}
}

void setupRtc()
{
	Wire.begin();
	updateFromRtc();
	pinMode(SECONDS_PIN, INPUT);
	attachInterrupt(0, updateFromRtc, RISING);
}


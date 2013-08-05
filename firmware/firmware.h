#ifndef FIRMWARE_H
#define FIRMWARE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/pgmspace.h>

#define NUM_DISPLAYS 3

#define LED_AM (1 << 4)
#define LED_PM (1 << 5)
#define LED_SEC1 (1 << 6)
#define LED_SEC2 (1 << 7)
#define LED_CATHODE (1 << 0)

#define bcd2dec(x) (((((x) >> 4) & 0xF) * 10) + ((x) & 0xF))

#define COL0_PIN 8
#define SECONDS_PIN 2

/* Don't change these, they correspond to OC1A and OC1B */
#define LATCH_PIN 9
#define BLANK_PIN 10

/* offsets in the digits array */
#define DAY 0
#define YEAR 2
#define HOUR 6
#define MINUTE 8

/* from main file */

void saveConfig();

/* from fonts.c */

extern const PROGMEM uint16_t font16['Z' - '0' + 1];
extern const PROGMEM uint16_t font7a[10];
extern const PROGMEM uint16_t font7b[10];

/* from rtc.cpp */

#define RTC_I2C_ADDRESS 0x6F

struct Time {
	uint8_t seconds, minutes, hours, day, month, year;
	bool pm;
};

extern Time rtc;

void setupRtc();
void loadRtcTime();
void setRtcTime(Time &time);

/* from display.cpp */

struct Display {
	bool useRtc;
	char month[3];
	uint8_t digits[10];
	uint8_t leds;
};

extern Display displays[NUM_DISPLAYS];

void setupDisplayDriver();
void setDisplay(Display *disp, Time &time);

/* from serial.cpp */

void checkCommand();

#endif

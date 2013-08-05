#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>

#include "firmware.h"

void saveConfig()
{
	for (uint8_t i = 0; i < sizeof(displays); i++)
	{
		EEPROM.write(i, ((uint8_t *)displays)[i]);
	}
}

static void loadConfig()
{
	for (uint8_t i = 0; i < sizeof(displays); i++)
	{
		((uint8_t *)displays)[i] = EEPROM.read(i);
	}
}

void setup()
{
	loadConfig();
	setupRtc();
	setupDisplayDriver();
	Serial.begin(9600);
}

void loop()
{
	checkCommand();
}

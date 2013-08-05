#include <Arduino.h>
#include "firmware.h"

static bool cmdPrintClock(char which)
{
	Display tmp;
	Display *disp;
	if (which == 'R')
	{
		tmp.useRtc = false;
		setDisplay(&tmp, rtc);
		disp = &tmp;
	}
	else if (which >= '1' && which <= ('0' + NUM_DISPLAYS))
	{
		disp = displays + which - '1';
	}
	else
	{
		return false;
	}
	
	if (disp->useRtc)
	{
		Serial.println("RTC");
		return true;
	}
	
	char str[15];
	for (int i = 0; i < 3; i++)
	{
		str[i] = disp->month[i];
	}
	for (int i = 0; i < 10; i++)
	{
		str[i + 3] = disp->digits[i] + '0';
	}
	str[13] = '0' + ((disp->leds >> 4) & 7);
	str[14] = 0;
	Serial.println(str);
	return true;
}

static bool cmdSetRtc(char *cmd, uint8_t len)
{
	if (len < 12)
	{
		return false;
	}
	
	uint8_t data[6];
	
	for (int i = 0; i < 6; i++)
	{
		char *text = cmd + (2 * i);
		if (text[0] < '0' || text[0] > '9' || text[1] < '0' || text[1] > '9')
		{
			return false;
		}
		data[i] = ((text[0] - '0') << 4) | (text[1] - '0');
	}
	
	if (data[1] >= 0x13 || data[2] >= 0x32 || data[3] >= 0x24 || data[4] >= 0x60 || data[5] >= 0x60)
	{
		return false;
	}
	
	Time newTime = {
		data[5], data[4], (data[3] > 0x12) ? data[3] - 0x12 : data[3], data[2], data[1], data[0], (data[3] > 0x12)
	};
	
	setRtcTime(newTime);
	
	Serial.println("OK");
	return true;
}

static bool cmdSetClock(char which, char *cmd, uint8_t len)
{
	if (which == 'R')
	{
		return cmdSetRtc(cmd, len);
	}

	if (which <= '0' || which > ('0' + NUM_DISPLAYS))
	{
		return false;
	}
	
	Display *disp = displays + which - '1';
	
	if (len >= 3 && cmd[0] == 'R' && cmd[1] == 'T' && cmd[2] == 'C')
	{
		disp->useRtc = true;
		saveConfig();
		Serial.println("OK");
		return true;
	}
	
	if (len < 13)
	{
		return false;
	}
	
	disp->useRtc = false;
	for (int i = 0; i < 3; i++)
	{
		disp->month[i] = cmd[i];
	}
	for (int i = 0; i < 10; i++)
	{
		uint8_t val = cmd[i + 3] - '0';
		disp->digits[i] = (val > 9) ? 8 : val;
	}

	if (len > 13)
	{
  		/* Copy bit 3 into bit 4, so that we can set both second LEDs with a single bit */
  		uint8_t data = (cmd[13] - '0') & 7;
  		data |= (data & 4) << 1;
		disp->leds = data << 4;
  	}
  	else
  	{
		disp->leds = 0;
    	}

	saveConfig();
	Serial.println("OK");
	return true;
}

void serialPrintProgmem(PGM_P str)
{
	char c;
	while ((c = pgm_read_byte(str++)))
		Serial.print(c);
}

const PROGMEM char help[] =
	"     H A L P !\n\n"
	"Set Display Static: N=MMMDDYYYYhhmmL\n"
	"Set Display to RTC: N=RTC\n"
	"Get Display: N?\n"
	"    N is Display ID (1..3)\n"
	"       1 = first after controller\n"
	"    MMM is A..Z+0..9\n"
	"    DD, YYYY, hh, mm is 0..9\n"
	"    L is a LED bitmask: 1=AM, 2=PM, 4=seconds\n\n"
	"Set RTC: R=YYMMDDhhmmss\n"
	"Get RTC: R?\n"
	"    all digits 0..9\n"
	"    HH is 24hr format\n"
	"    YY < 60 means 20xx, YY >= 60 means 19xx\n\n";

static bool parseCommand(char *cmd, uint8_t len)
{
	if (len == 1 && cmd[0] == '?')
	{
		serialPrintProgmem(help);
		return true;
	}
	if (len < 2)
	{
		return false;
	}
	switch (cmd[1])
	{
	case '?':
		return cmdPrintClock(cmd[0]);
	case '=':
		return cmdSetClock(cmd[0], cmd + 2, len - 2);
	default:
		return false;
	}
}

void checkCommand()
{
	static char cmdBuf[16];
	static uint8_t cmdBufPtr = 0;

	while (Serial.available() && Serial.peek() != '\n')
	{
  		char c = Serial.read();
		//Serial.println(c, DEC);
		if (cmdBufPtr < sizeof(cmdBuf))
		{
			cmdBuf[cmdBufPtr++] = c;
		}
	}
	
	if (Serial.peek() == '\n')
	{
		Serial.read();
		if (!parseCommand(cmdBuf, cmdBufPtr))
		{
			Serial.println("Nope - enter \"?\" for help");
		}
		cmdBufPtr = 0;
	}
}


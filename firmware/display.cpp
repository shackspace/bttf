#include "Arduino.h"
#include <SPI.h>

#include "firmware.h"

const char PROGMEM monthnames[12][4] = {
	"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

const char PROGMEM shamelessplug[] = "   SHACKSPACE <?      WWW:SHACKSPACE:DE ;>     ";

Display displays[NUM_DISPLAYS];

void setupDisplayDriver()
{
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	pinMode(COL0_PIN, OUTPUT);
	pinMode(LATCH_PIN, OUTPUT);
	pinMode(BLANK_PIN, OUTPUT);
	
	/* set up timer 1 like this:
	*              ___________
	*  BLANK  ____/           \________________   OCR1B
	*                 _____
	*  LATCH  _______/     \___________________   OCR1A
	*
	*  INTR   __________!______________________
	*
	* with a period of 1kHz. That will ensure 128 full scans per second, which should be enough to come across as steady.
	*/
	
	TCCR1A = (2 << COM1A0)  /* Set OC1A on match counting down, clear on match counting up */
	       | (2 << COM1B0)  /* Same for OC1B */
	       | (2 << WGM10);  /* Phase correct PWM mode, using ICR1 as limit */
	ICR1 = 0x3FF; /* count up to 1k and back down -> 1kHz period */
	TCNT1 = 0;
	OCR1A = 1;
	OCR1B = 2;
	TIMSK1 = 1 << TOIE1;
	TIFR1 = 1 << TOV1;
	TCCR1B = (2 << WGM12)
	       | (2 << CS10);    /* clk / 8 --> 2MHz counting freq */
}

static uint16_t getTlcValue(Display *disp, int anode, bool shackmode, int startchar)
{
	if (anode < 3)
	{
		char c = shackmode ? pgm_read_byte(shamelessplug + startchar + anode) : disp->month[anode];
		if (c >= '0' && c <= 'Z')
		{
			return pgm_read_word(font16 + c - '0');
		}
		else
		{
			return 0;
		}
	}
	else
	{
		uint16_t result = pgm_read_word(font7a + disp->digits[anode - 3]) | pgm_read_word(font7b + disp->digits[anode + 2]);
		return (disp->leds & (1 << anode)) ? result | LED_CATHODE : result;
	}
}

static void updateDisplays(uint8_t anode)
{
	static uint16_t shack_timer = 0;
	static uint8_t shackDisplay;
	static const unsigned int FPS = 128;
	static const unsigned int FRAMESPERCHAR = FPS / 4;
	static const unsigned int PLUGLEN = (sizeof(shamelessplug) - 3) * FRAMESPERCHAR;
	
	if (shack_timer == 0)
	{
		shackDisplay = rand() % NUM_DISPLAYS;
	}
	
	/* Go through the displays in reverse order because the first display shifted into the chain will end up at the end of the chain */
	for (int8_t i = NUM_DISPLAYS - 1; i >= 0; i--)
	{
		uint16_t tlc_data = getTlcValue(displays + i, anode, (shack_timer < PLUGLEN) && i == shackDisplay, shack_timer / FRAMESPERCHAR);
		SPI.transfer(tlc_data >> 8);
		SPI.transfer(tlc_data & 0xFF);
	}
	
	if (anode == 7)
	{
		shack_timer++;
	}
}

ISR(TIMER1_OVF_vect)
{
	/* BLANK and LATCH just happened. Don't try to be fast enough, just prepare the input data for the next LATCH. */
	static uint8_t anode = 7;
	anode = (anode + 1) & 7;
	
	updateDisplays(anode);
	digitalWrite(COL0_PIN, !(anode == 0));
}

/* all numbers are expected in BCD */
void setDisplay(Display *disp, Time &time)
{
	/* arbitrarily set the year range to 1960...2059 */
	if (time.year < 0x60)
	{
		disp->digits[YEAR + 0] = 2;
		disp->digits[YEAR + 1] = 0;
	}
	else
	{
		disp->digits[YEAR + 0] = 1;
		disp->digits[YEAR + 1] = 9;
	}

	disp->digits[YEAR + 2] = time.year >> 4;
	disp->digits[YEAR + 3] = time.year & 0xF;

	for (int i = 0; i < 3; i++)
		disp->month[i] = pgm_read_byte(monthnames[bcd2dec(time.month) - 1] + i);

	disp->digits[DAY + 0] = time.day >> 4;
	disp->digits[DAY + 1] = time.day & 0xF;

	disp->digits[HOUR + 0] = time.hours >> 4;
	disp->digits[HOUR + 1] = time.hours & 0xF;

	disp->digits[MINUTE + 0] = time.minutes >> 4;
	disp->digits[MINUTE + 1] = time.minutes & 0xF;

	disp->leds = (time.pm ? LED_PM : LED_AM) | ((time.seconds & 1) ? 0 : (LED_SEC1 | LED_SEC2));
}


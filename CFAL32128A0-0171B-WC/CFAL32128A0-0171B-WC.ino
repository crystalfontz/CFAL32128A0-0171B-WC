//==============================================================================
//
//  CRYSTALFONTZ CFAL32128A0-0171B-WC EXAMPLE FIRMWARE
//
//  Code written for Seeeduino v4.2 set to 3.3v (important!)
//
//  This code uses interfaces:
//    OLED Display     = 4-wire SPI or I2C (selectable in prefs.h)
//    Touch Controller = 4-wire SPI or I2C (selectable in prefs.h)
//
//  The OLED/Touch controller is a Sitronix SSD7317
//
//  The OLED and touch controllers are in the same IC in the panel, but are
//   for the most part treated like two separate IC's.
//
//  Seeeduino v4.2, an open-source 3.3v capable Arduino clone.
//    https://www.seeedstudio.com/Seeeduino-V4.2-p-2517.html
//    https://github.com/SeeedDocument/SeeeduinoV4/raw/master/resources/Seeeduino_v4.2_sch.pdf
//
//==============================================================================
//
//  2019-10-30 Mark Williams / Crystalfontz
//
//==============================================================================
//This is free and unencumbered software released into the public domain.
//
//Anyone is free to copy, modify, publish, use, compile, sell, or
//distribute this software, either in source code form or as a compiled
//binary, for any purpose, commercial or non-commercial, and by any
//means.
//
//In jurisdictions that recognize copyright laws, the author or authors
//of this software dedicate any and all copyright interest in the
//software to the public domain. We make this dedication for the benefit
//of the public at large and to the detriment of our heirs and
//successors. We intend this dedication to be an overt act of
//relinquishment in perpetuity of all present and future rights to this
//software under copyright law.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//OTHER DEALINGS IN THE SOFTWARE.
//
//For more information, please refer to <http://unlicense.org/>
//==============================================================================

#include <Arduino.h>
#include <Wire.h>
#include "prefs.h"
#include "ssd7317_oled.h"
#include "ssd7317_touch.h"
#include "font_8x8x1.h"

//////////////////////////////////////////////////////////

//oled display buffer ram
uint8_t OLEDBuf[(SSD7317_OLED_HEIGHT*SSD7317_OLED_WIDTH)/8]; //1 bit per pixel

//local functions
void PrintChar(uint8_t x, uint8_t y, uint8_t c);
void PrintString(uint8_t x, uint8_t y, const char *str);

//timing
#define SYS_TICK					millis()
#define SYS_TICKSPEED				(1) /* 1mS / tick */
#define TIMER_MAX					UINT32_MAX
#define TIMER_IF_EXPIRED(var)		if ((SYS_TICK - (var)) < (TIMER_MAX/2))
#define TIMER_SET(var,time_ms)		(var) = SYS_TICK + ((time_ms) * SYS_TICKSPEED);

//touch display types
#define TOUCH_NONE 0
#define TOUCH_TAP 1
#define TOUCH_HOLD 2
#define TOUCH_DTAP 3
#define TOUCH_SWIPE 4

//////////////////////////////////////////////////////////

//run at power-on / reset
//do inits
void setup()
{
	//startup serial output
	Serial.begin(115200);
	Serial.println("setup()");

	//init the oled controller
	//oled must be initialised before the touch controller
	Serial.println("SSD7317_OLED_Init()");
	//uses hardware i2c
	SSD7317_OLED_Init();

	//blank the display buffer, intro text
	memset(OLEDBuf, 0x00, sizeof(OLEDBuf));
	PrintString(16,0, "CRYSTALFONTZ");
	PrintString(20,2, "CFAL32128A0");
	PrintString(32,3, "0171B-WC");
	SSD7317_OLED_WriteBuffer(OLEDBuf);
	delay(1000);

	//touch init display
#ifdef OLED_I2C
	//disable hardware i2c for now
	Wire.end();
#endif

	//init the capactive touch controller
	//uses software i2c for init as it must ignore I2C NACKs
	Serial.println("SSD7317_Touch_Init()");
	SSD7317_Touch_Init();

	//now use hardware i2c for touch
#ifdef TOUCH_I2C
	Wire.begin();
	SSD7317_Touch_HWI2C(true);
	//kick the i2c speed up to 400kHz
	Wire.setClock(5000);
#endif

	//done inits
	Serial.println("setup() done");
}

void DisplayKeyNumbers(void)
{
	//blank display & draw 4 div key lines
	memset(OLEDBuf, 0x00, sizeof(OLEDBuf));
	for (uint8_t i = 0; i < 4; i++)
	{
		OLEDBuf[32+(i*SSD7317_OLED_WIDTH)] = 0xFF;
		OLEDBuf[64+(i*SSD7317_OLED_WIDTH)] = 0xFF;
		OLEDBuf[96+(i*SSD7317_OLED_WIDTH)] = 0xFF;
	}
	//key numbers (right to left)
	PrintChar(0+(32/2)-4, 1, '4');
	PrintChar(32+(32/2)-4, 1, '3');
	PrintChar(64+(32/2)-4, 1, '2');
	PrintChar(96+(32/2)-4, 1, '1');
}

void PrintKeyState(uint8_t Key, const char *State, uint8_t YLoc)
{
	//print key state text on oled
	PrintString(((4-Key)*32)+(32/2)-(strlen(State)*4), 2, State);
}

void PrintKeyNames(uint8_t Act, uint8_t Detail)
{
	//print key actions text on display
	const char ActStrings[4][5] = {"KEY", "TAP", "HOLD", "DTAP"};
	PrintKeyState(1, ActStrings[0], 0xFF);
	PrintKeyState(2, ActStrings[0], 0xFF);
	PrintKeyState(3, ActStrings[0], 0xFF);
	PrintKeyState(4, ActStrings[0], 0xFF);
	if (Detail == 0)
		//nothing more to do
		return;
	//act/detail string
	if (Act < 4)
		PrintKeyState(Detail, ActStrings[Act], 0xFF);
}

void HighlightKey(uint8_t Key)
{
	//highlight the key square on the display
	uint8_t *tpos, *bpos;
	uint8_t i;

	//top & bottom row highlight
	tpos = OLEDBuf + ((4-Key) * 32) + 1;
	bpos = OLEDBuf + ((4-Key) * 32) + (3 * SSD7317_OLED_WIDTH) + 1;
	for (i = 0; i < 31; i++)
	{
		if (i % 2)
			*tpos = 0xAA;
		else
			*tpos = 0x55;
		*bpos = *tpos;
		tpos++;
		bpos++;
	}
}

void DisplayScreenSwipe(uint8_t Detail, uint8_t StartEnd)
{
	//display has been swiped / dragged
	//draw a line down the middle of the display
	memset(OLEDBuf, 0x00, sizeof(OLEDBuf));
	//draw swipe text
	if (Detail == 1)
		PrintString(16,1, "SWIPED LEFT");
	else
		PrintString(16,1, "SWIPED RIGHT");
	//draw in / out text
	char InOut[8] = "X to X";
	InOut[0] = ((StartEnd >> 4) & 0b0111) + '0';
	InOut[5] = (StartEnd & 0b0111) + '0';
	PrintString(40,2, InOut);
}

void DisplayKeySwipe(uint8_t Detail, uint8_t StartEnd)
{
	//incell key has been swiped up or down
	//Detail indicates direction
	//lower 3 bits of StartEnd gives key number
	uint8_t Key = StartEnd & 0b0111;
	HighlightKey(Key);
	if (Detail == 1)
	{
		//swiped down
		PrintKeyState(Key, "DOWN", 0xFF);
	}
	else
	{
		//swiped up
		PrintKeyState(Key, " UP ", 0xFF);
	}
}

void DisplayExternalPress(uint8_t Act, uint8_t Detail)
{
	//external (outcell) press/hold
	memset(OLEDBuf, 0x00, sizeof(OLEDBuf));
	switch (Act)
	{
		case TOUCH_TAP: PrintString(16,1, "OUTCELL TAP"); break;
		case TOUCH_HOLD: PrintString(16,1, "OUTCELL HOLD"); break;
		case TOUCH_DTAP: PrintString(16,1, "OUTCELL DTAP"); break;
	}
	PrintChar(63, 2, Detail + '0');
}

void DisplayUpdate(bool Activity)
{
	//display needs to change state, do update

	if (Activity == false)
	{
		//nothing happening
		DisplayKeyNumbers();
		PrintKeyNames(0, 0);
		SSD7317_OLED_WriteBuffer(OLEDBuf);
		//done
		return;
	}

	if ((SSD7317_Gesture_Data.Act == TOUCH_TAP) ||
		(SSD7317_Gesture_Data.Act == TOUCH_HOLD) ||
		(SSD7317_Gesture_Data.Act == TOUCH_DTAP))
	{
		//key tapped, held or double-pressed
		if (SSD7317_Gesture_Data.Location == 0)
		{
			//oled/incell activity
			DisplayKeyNumbers();
			HighlightKey(SSD7317_Gesture_Data.Detail);
			PrintKeyNames(SSD7317_Gesture_Data.Act, SSD7317_Gesture_Data.Detail);
			//Serial.println("Disp = Incell Tap/Hold/DTap");
		}
		if (SSD7317_Gesture_Data.Location == 2)
		{
			//external/outcell activity
			DisplayExternalPress(SSD7317_Gesture_Data.Act, SSD7317_Gesture_Data.Detail);
			//Serial.println("Disp = Outcell Tap/Hold/DTap");
		}
		SSD7317_OLED_WriteBuffer(OLEDBuf);
		return;
	}

	if (SSD7317_Gesture_Data.Act == TOUCH_SWIPE)
	{
		//display swiped
		//we need to check those 4th Start/End bits
		if (SSD7317_Gesture_Data.Location == 1)
		{
			//location is eq 1, is a key swipe
			DisplayKeyNumbers();
			PrintKeyNames(0, 0);
			DisplayKeySwipe(SSD7317_Gesture_Data.Detail, SSD7317_Gesture_Data.StartEnd);
			//Serial.println("Disp = Incell Key Swipe");
		}
		else
		{
			//key swipe bits were not set, the screen was swiped left/right
			DisplayScreenSwipe(SSD7317_Gesture_Data.Detail, SSD7317_Gesture_Data.StartEnd);
			//Serial.println("Disp = Incell Disp Swipe");
		}

		//update oled
		SSD7317_OLED_WriteBuffer(OLEDBuf);
		return;
	}
}

void SerialLogTouch(void)
{
	//log the touch to serial for debugging
	const char	TouchActs[5][8] = {"na", "tap", "hold", "dbltap", "swipe"};
	const char	Location[4][10] = {"oled", "key-swipe", "ext", "na"};
	Serial.print("Touched = location-"); Serial.print(Location[SSD7317_Gesture_Data.Location & 0x03] );
	Serial.print(" act-");
	if (SSD7317_Gesture_Data.Act < 5)
		Serial.print(TouchActs[SSD7317_Gesture_Data.Act]);
	else
		Serial.print(SSD7317_Gesture_Data.Act);
	Serial.print(" detail-"); Serial.print(SSD7317_Gesture_Data.Detail);
	Serial.print(" in-"); Serial.print((SSD7317_Gesture_Data.StartEnd >> 4) & 0x0F);
	Serial.print(" out-"); Serial.println(SSD7317_Gesture_Data.StartEnd & 0x0F);
}

//main program loop
void loop()
{
	//main loop
	Serial.println("loop()");

	uint32_t	DispTimer;
	uint8_t		LastTouchType = TOUCH_NONE;

	//first display update
	DisplayUpdate(false);

	while(1)
	{
		//this MUST be checked at least every 50mS
		if (SSD7317_TouchData_Waiting == true)
		{
			//flaged for falling touch irq pin
			//handle the touch data
			//this also resets SSD7317_TouchData_Waiting flag var
			SSD7317_Touch_Handle();

			//serial log
			SerialLogTouch();
			//check for a new touch
			if (LastTouchType == TOUCH_NONE)
			{
				//new touch
				LastTouchType = SSD7317_Gesture_Data.Act;
				//start display timer
				TIMER_SET(DispTimer, 600); //display touch for x msec
				//update display with appropriate touch type
				DisplayUpdate(true);
			}
		}

		//check for prev touch & display timer lapsing
		if (LastTouchType != TOUCH_NONE)
		{
			TIMER_IF_EXPIRED(DispTimer)
			{
				//reset touch type/key
				LastTouchType = TOUCH_NONE;
				//draw untouched normal screen
				DisplayUpdate(false);
			}
		}
	}
}

void PrintChar(uint8_t x, uint8_t y, uint8_t c)
{
	//print a character to the display buffer
	uint8_t	*bufpos;
	uint8_t	i, j;

	c -= FONT8x8_BASE;
	if (c > FONT8x8_CHARNUM-1)
		return;
	bufpos = OLEDBuf + x + (y * SSD7317_OLED_WIDTH);
	for (i = 0; i < FONT8x8_WIDTH; i++)
	{
		*bufpos = 0; //blank
		for (j = 0; j < FONT8x8_HEIGHT; j++)
		{
			uint8_t b = pgm_read_byte_near(&Font_8x8[(c * FONT8x8_HEIGHT) + j]);
			*bufpos |= ((b >> (FONT8x8_WIDTH - i)) & 1) << j;
		}
		bufpos++;
	}
}

void PrintString(uint8_t x, uint8_t y, const char *str)
{
	//render a string to the display buffer
	uint8_t i;
	for (i = 0; i < SSD7317_OLED_WIDTH / FONT8x8_WIDTH; i++)
	{
		if (str[i] == '\0') break; //null char
		PrintChar(x + (i * FONT8x8_WIDTH), y, (uint8_t)str[i]);
	}
}

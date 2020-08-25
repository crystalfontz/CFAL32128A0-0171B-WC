//==============================================================================
//
//  CRYSTALFONTZ CFAL64128A0-096B-WC EXAMPLE FIRMWARE
//
//  OLED DISPLAY I2C INTERFACE FIRMWARE
//
//  Code written for Seeeduino v4.2 set to 3.3v (important!)
//
//  The controller is a Sitronix SSD7317.
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

#include "prefs.h"

#ifdef OLED_I2C

#include <Arduino.h>
#include <Wire.h>
#include "ssd7317_oled_i2c.h"

//////////////////////////////////////////////////////////

#define SSD7317_OLED_DC_CMD			(0)
#define SSD7317_OLED_DC_DATA		(1)

static void SSD7317_OLED_Setup();
static void SSD7317_OLED_WR_CMD(unsigned char command);

//////////////////////////////////////////////////////////

void SSD7317_OLED_Init(void)
{
	Serial.println("SSD7317_OLED_Init() I2C");
	//pin setup
	digitalWrite(SSD7317_OLED_RST, HIGH); //reset pin
	pinMode(SSD7317_OLED_RST, OUTPUT);

	//reset
	digitalWrite(SSD7317_OLED_RST, LOW);
	delay(10);
	digitalWrite(SSD7317_OLED_RST, HIGH);
	delay(10);

	//I2C init
	Wire.begin();

	//run setup commands
	SSD7317_OLED_Setup();

	//bank lcd
	SSD7317_OLED_Blank();
}

#define SEGS 16
void SSD7317_OLED_WriteBuffer(uint8_t *buf)
{
	uint16_t i, j;
	for (i = 0; i < ( SSD7317_OLED_HEIGHT * SSD7317_OLED_WIDTH / 8) / SEGS ; i++)
	{
		//we have to break this up into lots of SEGS due to Arduino I2C limitations
		j = i*SEGS;
		SSD7317_OLED_WR_CMD(0x21);		//col address
		SSD7317_OLED_WR_CMD(j / 8);
		SSD7317_OLED_WR_CMD(0x7F);

		SSD7317_OLED_WR_CMD(0x22);		//page address
		SSD7317_OLED_WR_CMD(j % 8);
		SSD7317_OLED_WR_CMD(0x07);		//128x64

		Wire.beginTransmission(SSD7317_OLED_I2C_ADDR);
		Wire.write(0x40); //control byte, data bit set
		Wire.write(&buf[j], SEGS);
		Wire.endTransmission();
	}
}

void SSD7317_OLED_Blank(void)
{
	//blank the display
	SSD7317_OLED_WR_CMD(0x21);		//col address
	SSD7317_OLED_WR_CMD(0x00);
	SSD7317_OLED_WR_CMD(0x7F);

	SSD7317_OLED_WR_CMD(0x22);		//page address
	SSD7317_OLED_WR_CMD(0x00);
	SSD7317_OLED_WR_CMD(0x07);		//128x64

	Wire.beginTransmission(SSD7317_OLED_I2C_ADDR);
	Wire.write(0x40); //control byte, data bit set
	for (uint16_t i = 0; i < SSD7317_OLED_HEIGHT * SSD7317_OLED_WIDTH / 8; i++)
		Wire.write(0x00);
	Wire.endTransmission();
}

//////////////////////////////////////////////////////////

static void SSD7317_OLED_Setup()
{
	//initialise the oled driver ic
	SSD7317_OLED_WR_CMD(0XFD); // Command Lock Main
	SSD7317_OLED_WR_CMD(0X12); // 12H:Unlock 16:Lock

	SSD7317_OLED_WR_CMD(0XAE); // Display OFF (sleep mode)

	SSD7317_OLED_WR_CMD(0XAD); // External or internal IREF Selection
	SSD7317_OLED_WR_CMD(0X00); //10 in , 00 ext

	SSD7317_OLED_WR_CMD(0XA8); // Set Multiplex Ratio Main
	SSD7317_OLED_WR_CMD(0X3f); // Set 64 duty -

	SSD7317_OLED_WR_CMD(0XD3); // Set Display Offset
	SSD7317_OLED_WR_CMD(0X10);

	SSD7317_OLED_WR_CMD(0XA2); // Set Display Start Line
	SSD7317_OLED_WR_CMD(0X00); // Start Line 00

	SSD7317_OLED_WR_CMD(0XA1); // Set Segment Remap

	SSD7317_OLED_WR_CMD(0XC8); // Set COM Output Scan Direction

	SSD7317_OLED_WR_CMD(0XDA); //Com Pins Hardware
	SSD7317_OLED_WR_CMD(0X12);

	SSD7317_OLED_WR_CMD(0X81); // Set Contrast Control
	SSD7317_OLED_WR_CMD(0X8f);

	SSD7317_OLED_WR_CMD(0XA4); // A4:Display Ram ;A5:Display All Pixel

	SSD7317_OLED_WR_CMD(0XA6); // A6:0 is Pixel off; A7:0 is Pixel on

	SSD7317_OLED_WR_CMD(0XD5); // Set Front Clock Divider /Oscillator Frequency
	SSD7317_OLED_WR_CMD(0X80);

	SSD7317_OLED_WR_CMD(0XD9); // Set Pre-charge Period
	SSD7317_OLED_WR_CMD(0X32);

	SSD7317_OLED_WR_CMD(0XDB); //Set VcomH
	SSD7317_OLED_WR_CMD(0X30);

	//==============================================
	SSD7317_OLED_WR_CMD(0X31);
	SSD7317_OLED_WR_CMD(0Xd0);

	SSD7317_OLED_WR_CMD(0X34);
	SSD7317_OLED_WR_CMD(0X0f);

	SSD7317_OLED_WR_CMD(0X37);
	SSD7317_OLED_WR_CMD(0X01);

	SSD7317_OLED_WR_CMD(0X36);
	SSD7317_OLED_WR_CMD(0X0f);

	SSD7317_OLED_WR_CMD(0X35);
	SSD7317_OLED_WR_CMD(0X0b);
	//=============================================

	SSD7317_OLED_WR_CMD(0x20); //mem adressing mode
	SSD7317_OLED_WR_CMD(0x01); //com-page h-mode

	SSD7317_OLED_WR_CMD(0XAF); //display on
}

static void SSD7317_OLED_WR_CMD(unsigned char command)
{
	//send command
	uint8_t data[2];
	data[0] = 0x00; // control byte - 0 << 6 for command
	data[1] = command;
	Wire.beginTransmission(SSD7317_OLED_I2C_ADDR);
	Wire.write(data, 2);
	Wire.endTransmission();
}

#endif

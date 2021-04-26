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

void SSD7317_OLED_WriteBuffer(uint8_t *buf)
{
	uint16_t i;

	//set top left address

	//split write into 32 byte blocks due to Arduino libs
	//I2C buffer size limit
	for (i = 0; i < (SSD7317_OLED_HEIGHT * SSD7317_OLED_WIDTH / 8) / 32; i++)
	{
		//update display ram loction if needed
		if ((i % 4) == 0)
		{
			//start of page, set display ram position
			SSD7317_OLED_WR_CMD(0x21);		//col address
			SSD7317_OLED_WR_CMD(0);			//X (0 = left)
			SSD7317_OLED_WR_CMD(0x7F);

			SSD7317_OLED_WR_CMD(0x22);		//page address
			SSD7317_OLED_WR_CMD(i >> 2);	//Y (0 = top, 3 = bottom)
			SSD7317_OLED_WR_CMD(0x07);
		}
		//write data
		Wire.beginTransmission(SSD7317_OLED_I2C_ADDR);
		Wire.write(0x40);				//send control byte, data bit set
		Wire.write(buf, 32);			//write 32 bytes
		buf += 32;						//next buf location
		Wire.endTransmission();			//end
	}
}

void SSD7317_OLED_Blank(void)
{
	//blank the display
	//*needs to be optimised*
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

//oled manufacturer supplied init commands
const uint8_t SSD7317_128x32_Init[] =
{
	0xFD, 0x12, 0xAE, 0x20, 0x00, 0xAD, 0x10, 0xA8, 0x1F, 0xD3,
	0x20, 0xA2, 0x00, 0xA0, 0xC8, 0xDA, 0x12, 0x81, 0x24, 0xA4,
	0xA6, 0xD5, 0xA1, 0xD9, 0x43, 0xDB, 0x30, 0x31, 0xD0, 0x34,
	0x0F, 0x37, 0x01, 0x36, 0x0F, 0x35, 0x0B, 0xAF
};

static void SSD7317_OLED_Setup(void)
{
	//initialise the oled driver ic
	//send the init commands
	for (uint8_t i = 0; i < sizeof(SSD7317_128x32_Init); i++)
		SSD7317_OLED_WR_CMD(SSD7317_128x32_Init[i]);
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

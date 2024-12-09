//==============================================================================
//
//  CRYSTALFONTZ CFAL64128A0-096B-WC EXAMPLE FIRMWARE
//
//  OLED DISPLAY SPI INTERFACE FIRMWARE
//
//  Code written for Seeeduino set to 3.3v (important!)
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
#include <Arduino.h>
#include "prefs.h"

#ifdef OLED_SPI
#include <SPI.h>
#endif
#ifdef OLED_I2C
#include <Wire.h>
#endif
#include "ssd7317_oled.h"

//////////////////////////////////////////////////////////

#define SSD7317_OLED_DC_CMD			(0)
#define SSD7317_OLED_DC_DATA		(1)

static void SSD7317_OLED_Setup();
#ifdef OLED_SPI
static void SSD7317_OLED_DISP_SPI4_WR(unsigned char data, unsigned char DC);
#endif
#ifdef OLED_I2C
static void SSD7317_OLED_WR_CMD(unsigned char command);
#endif

//////////////////////////////////////////////////////////

void SSD7317_OLED_Init(void)
{
  //pin setup
#ifdef OLED_SPI
  digitalWrite(SSD7317_OLED_SPI_CS, HIGH);
  pinMode(SSD7317_OLED_SPI_CS, OUTPUT);
  digitalWrite(SSD7317_OLED_SPI_DC, HIGH);
  pinMode(SSD7317_OLED_SPI_DC, OUTPUT);
  digitalWrite(SSD7317_OLED_RST, HIGH);
  pinMode(SSD7317_OLED_RST, OUTPUT);

  //spi setup
  SPI.begin();
  SPI.beginTransaction(SPISettings(SSD7317_OLED_SPI_FREQ, MSBFIRST, SPI_MODE0));

  //reset
  digitalWrite(SSD7317_OLED_RST, LOW);
  delay(10);
  digitalWrite(SSD7317_OLED_RST, HIGH);
  delay(10);
#endif

#ifdef OLED_I2C
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
#endif

  //run setup commands
  SSD7317_OLED_Setup();

  //bank lcd
  SSD7317_OLED_Blank();
}

void SSD7317_OLED_WriteBuffer(uint8_t *buf)
{
#ifdef OLED_SPI
  SSD7317_OLED_DISP_SPI4_WR(0x21, SSD7317_OLED_DC_CMD);		//col address
  SSD7317_OLED_DISP_SPI4_WR(0x00, SSD7317_OLED_DC_CMD);
  SSD7317_OLED_DISP_SPI4_WR(0x7F, SSD7317_OLED_DC_CMD);

  SSD7317_OLED_DISP_SPI4_WR(0x22, SSD7317_OLED_DC_CMD);		//page address
  SSD7317_OLED_DISP_SPI4_WR(0x00, SSD7317_OLED_DC_CMD);
  SSD7317_OLED_DISP_SPI4_WR(0x07, SSD7317_OLED_DC_CMD);		//128x64

  //setup D/C# bit
  digitalWrite(SSD7317_OLED_SPI_DC, SSD7317_OLED_DC_DATA);
  //setup CS
  digitalWrite(SSD7317_OLED_SPI_CS, LOW);
  //send data
  SPI.transfer(buf, SSD7317_OLED_HEIGHT * SSD7317_OLED_WIDTH / 8);
  //done
  digitalWrite(SSD7317_OLED_SPI_CS, HIGH);
#endif

#ifdef OLED_I2C
	//split write into 16 byte blocks due to Arduino libs I2C buffer size limit
	uint16_t row, seg;
	for (row = 0; row < 4; row++)
	{
		//set display ram position
		SSD7317_OLED_WR_CMD(0x21);		//set col address
		SSD7317_OLED_WR_CMD(0);			//X (0 = left)
		SSD7317_OLED_WR_CMD(0x7F);

		SSD7317_OLED_WR_CMD(0x22);		//set page address
		SSD7317_OLED_WR_CMD(row);		//Y (0 = top, 3 = bottom)
		SSD7317_OLED_WR_CMD(0x07);

		for (seg = 0; seg < 8; seg++)
		{
			//write data
			Wire.beginTransmission(SSD7317_OLED_I2C_ADDR);
			Wire.write(0x40);				//send control byte, data bit set
			Wire.write(buf, 16);			//write 16 bytes
			buf += 16;						//next buf location
			Wire.endTransmission();			//end
		}
	}
#endif
}

void SSD7317_OLED_Blank(void)
{
  //blank the display
#ifdef OLED_SPI
  SSD7317_OLED_DISP_SPI4_WR(0x21, SSD7317_OLED_DC_CMD);		//col address
  SSD7317_OLED_DISP_SPI4_WR(0x00, SSD7317_OLED_DC_CMD);
  SSD7317_OLED_DISP_SPI4_WR(0x7F, SSD7317_OLED_DC_CMD);

  SSD7317_OLED_DISP_SPI4_WR(0x22, SSD7317_OLED_DC_CMD);		//page address
  SSD7317_OLED_DISP_SPI4_WR(0x00, SSD7317_OLED_DC_CMD);
  SSD7317_OLED_DISP_SPI4_WR(0x07, SSD7317_OLED_DC_CMD);		//128x64

  //setup D/C# bit
  digitalWrite(SSD7317_OLED_SPI_DC, SSD7317_OLED_DC_DATA);
  //setup CS
  digitalWrite(SSD7317_OLED_SPI_CS, LOW);
  //send data
  for (uint16_t i = 0; i < SSD7317_OLED_HEIGHT * SSD7317_OLED_WIDTH / 8; i++)
  {
    SPI.transfer(0x00);
  }
  //done
  digitalWrite(SSD7317_OLED_SPI_CS, HIGH);
#endif

#ifdef OLED_I2C
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
  {
		Wire.write(0x00);
  }
	Wire.endTransmission();
#endif
}

//////////////////////////////////////////////////////////
#define CFA (1)
//oled manufacturer supplied init commands
const uint8_t SSD7317_128x32_Init[] =
{
  0xFD, 0x12,   // Set command unlock
  0xAE,         // Set display off
  0x20, 0x00,   // Set Memory Addressing mode
#if (CFA == 1)
	0xAD, 0x10,   // Set Internal IREF disable
#else
	0xAD, 0x00,   // Set Internal IREF disable
#endif
  0xA8, 0x1F,   // Set Multiplex Ratio
  0xD3, 0x20,   // Set Display offset
  0xA2, 0x00,   // Set Display Start Line
  0xA0,         // Set Segment Re-Map
  0xC8,         // Set COM Output Scan Direction
  0xDA, 0x12,   // Set COM Pins Hardware Configuration
	0x81, 0xFF,   // Set Contrast Control (brightness)
  0xA4,         // Set entire display off
  0xA6,         // Set Normal Display
#if (CFA == 1)
	0xD5, 0xA1,   // Set Display Clock Divide Ratio/Oscillator Frequency
#else
	0xD5, 0xB0,   // Set Display Clock Divide Ratio/Oscillator Frequency
#endif
  0xD9, 0x43,   // Set Discharge / Pre-Charge Period
  0xDB, 0x30,   // Set VCOM Deselect Level
  0x31, 0xD0,   // Touch function Set 1
  0x34, 0x0F,   // Touch function Set 2
  0x37, 0x01,   // Touch function Set 3
  0x36, 0x0F,   // Touch function Set 4
#if (SSD7317_TOUCH_I2C_ADDR == 0x53)
	0x35, 0x0A,   // Touch function Set 5
#elif (SSD7317_TOUCH_I2C_ADDR == 0x5B)
	0x35, 0x0B,   // Touch function Set 5
#endif
	0xAF          // Set Display On
};

static void SSD7317_OLED_Setup(void)
{
  //initialise the oled driver ic
  //send the init commands
  for (uint8_t i = 0; i < sizeof(SSD7317_128x32_Init); i++)
  {
#ifdef OLED_SPI
    SSD7317_OLED_DISP_SPI4_WR(SSD7317_128x32_Init[i], SSD7317_OLED_DC_CMD);
#endif

#ifdef OLED_I2C
		SSD7317_OLED_WR_CMD(SSD7317_128x32_Init[i]);
#endif
  }
}

#ifdef OLED_SPI
static void SSD7317_OLED_DISP_SPI4_WR(unsigned char data, unsigned char DC)
{
  //write a byte of data to the oled controller ic
  //setup D/C# bit
  digitalWrite(SSD7317_OLED_SPI_DC, SSD7317_OLED_DC_CMD);
  //setup CS
  digitalWrite(SSD7317_OLED_SPI_CS, LOW);
  //send data (also controls CS)
  SPI.transfer(data);
  //done
  digitalWrite(SSD7317_OLED_SPI_CS, HIGH);
}
#endif
#ifdef OLED_I2C
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

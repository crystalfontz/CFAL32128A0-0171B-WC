//==============================================================================
//
//  CRYSTALFONTZ CFAL32128A0-0171B-WC EXAMPLE FIRMWARE
//
//  TOUCH PANEL CONTROLLER INTERFACE FIRMWARE
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

#ifdef TOUCH_SPI

#include <Arduino.h>
#include <SPI.h>
#include "ssd7317_touch_spi.h"
#include "ssd7317_touch_fwblob.h"

//enable to output some debugging information over serial/usb
//#define SERIAL_DEBUG
#ifdef SERIAL_DEBUG
#define LOG_S(x)	Serial.print((x))
#define LOG_LN(x)	Serial.println((x))
#else
#define LOG_S(x)
#define LOG_LN(x)
#endif

//////////////////////////////////////////////////////////

#define SSD7317_TOUCH_FW_PAGE_SIZE	0x200

//////////////////////////////////////////////////////////

static void SSD7317_Touch_Setup(void);
static void SSD7317_Touch_IRQ(void);
//static void SSD7317_Touch_Process(uint8_t *data);
uint16_t SSD7317_TIC_CPU_BurstRead(uint16_t address, uint8_t data[], uint16_t num);
uint16_t SSD7317_TIC_CPU_RegRead();

volatile SSD7317_InTouch_t SSD7317_Gesture_Data;
volatile bool SSD7317_TouchData_Waiting = false;

//////////////////////////////////////////////////////////

void SSD7317_Touch_Init(void)
{
	//vars
	SSD7317_TouchData_Waiting = false;

	//pin setup
	LOG_LN("SSD7317_Touch_Init()");
	digitalWrite(SSD7317_TOUCH_CS, HIGH);
	pinMode(SSD7317_TOUCH_CS, OUTPUT);
	digitalWrite(SSD7317_TOUCH_RST, HIGH);
	pinMode(SSD7317_TOUCH_RST, OUTPUT);
	pinMode(SSD7317_TOUCH_IRQ, INPUT_PULLUP);

	//spi setup
	SPI.begin();
	SPI.beginTransaction(SPISettings(SSD7317_TOUCH_SPI_FREQ, MSBFIRST, SPI_MODE0));

	//reset
	digitalWrite(SSD7317_TOUCH_RST, LOW);
	delay(10);
	digitalWrite(SSD7317_TOUCH_RST, HIGH);
	delay(10);

	//run setup commands
	SSD7317_Touch_Setup();

	//more touch settings
	//SSD7317_NM_GestureEnable(0b100001111); //all gestures enabled
	//SSD7317_TouchThreshold_Set(200); //default sensitivity
	//we dont need to touch these

	//setup interrupt handler for touches
	attachInterrupt(digitalPinToInterrupt(SSD7317_TOUCH_IRQ), SSD7317_Touch_IRQ, FALLING);

	LOG_LN("SSD7317_Touch_Init() done");
}


//////////////////////////////////////////////////////////

static void SSD7317_Touch_IRQ(void)
{
	//flag main app to check data
	//must call SSD7317_Touch_Handle() within 50mS
	SSD7317_TouchData_Waiting = true;
}

void SSD7317_Touch_Handle(void)
{
	//touch int has gone high, get touch data via spi
	uint8_t snl[2];

	//reset flag
	SSD7317_TouchData_Waiting = false;

	//read status
	SSD7317_TIC_CPU_RegRead();

	//read touch status 2 bytes first
	SSD7317_TIC_CPU_BurstRead(0x0AF0, snl, 2);
	//the touch controller can report more than one touch at a time
	// but we'll only handle single touch reports for simplicity.
	//If it does report more than one touch, the data length will
	// be a multiple of 6 (6 bytes per touch).
	uint8_t data_len = snl[0];
	if (data_len != 6)
		//invalid or too many touches
		return;

	//now read touch data directly into the gesture status struct
	SSD7317_TIC_CPU_BurstRead(0x0AF1, (uint8_t*)&SSD7317_Gesture_Data, data_len);
	if (SSD7317_Gesture_Data.ID != 0xF6)
	{
		//the ID byte is always 0xF6 for valid touch data
		//it is not in this case, so invalidate the data
		memset((void*)&SSD7317_Gesture_Data, 0x00, 6);
	}
}

//////////////////////////////////////////////////////////

uint16_t SSD7317_TIC_CPU_RegRead(void)
{
	uint16_t ret = 0;
	digitalWrite(SSD7317_TOUCH_CS, LOW);
	SPI.transfer(0x03);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	ret |= SPI.transfer(0xFF);
	ret |= SPI.transfer(0xFF) << 8;
	digitalWrite(SSD7317_TOUCH_CS, HIGH);
	return ret;
}

uint16_t SSD7317_TIC_BIOS_RegRead()
{
	uint16_t ret = 0;
	digitalWrite(SSD7317_TOUCH_CS, LOW);
	SPI.transfer(0x03);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	ret |= SPI.transfer(0xFF);
	ret |= SPI.transfer(0xFF) << 8;
	digitalWrite(SSD7317_TOUCH_CS, HIGH);
	return ret;
}

uint16_t SSD7317_TIC_BIOS_RegWrite(uint16_t address)
{
	uint8_t addr[2];
	addr[0] = (uint8_t)((address >> 8) & 0xFF);
	addr[1] = (uint8_t)((address >> 0) & 0xFF);
	digitalWrite(SSD7317_TOUCH_CS, LOW);
	SPI.transfer(0x02);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(addr[1]);
	SPI.transfer(addr[0]);
	digitalWrite(SSD7317_TOUCH_CS, HIGH);
	return 0;
}

uint16_t SSD7317_TIC_BIOS_BurstWrite(uint16_t address, uint8_t data[], uint16_t num)
{
	uint8_t addr[2];
	uint16_t i;
	addr[0] = (uint8_t)((address >> 8) & 0xFF);
	addr[1] = (uint8_t)((address >> 0) & 0xFF);
	digitalWrite(SSD7317_TOUCH_CS, LOW);
	SPI.transfer(0x06);
	SPI.transfer(addr[1]);
	SPI.transfer(addr[0]);
	for(i = 0; i < num; i++)
		SPI.transfer(data[i]);
	digitalWrite(SSD7317_TOUCH_CS, HIGH);
	return 0;
}

uint16_t SSD7317_TIC_BIOS_BurstWrite_PROGMEM(uint16_t address, uint8_t data[], uint16_t num)
{
	//modified to read data from PROGMEM
	uint8_t addr[2];
	uint16_t i;
	addr[0] = (uint8_t)((address >> 8) & 0xFF);
	addr[1] = (uint8_t)((address >> 0) & 0xFF);
	digitalWrite(SSD7317_TOUCH_CS, LOW);
	SPI.transfer(0x06);
	SPI.transfer(addr[1]);
	SPI.transfer(addr[0]);
	for(i = 0; i < num; i++)
	{
		addr[0] = pgm_read_byte_near(data+i);
		SPI.transfer(addr[0]);
	}
	digitalWrite(SSD7317_TOUCH_CS, HIGH);
	return 0;
}

uint16_t SSD7317_TIC_BIOS_BurstRead(uint16_t address, uint8_t data[], uint16_t num)
{
	uint8_t addr[2];
	uint16_t i;
	addr[0] = (uint8_t)((address >> 8) & 0xFF);
	addr[1] = (uint8_t)((address >> 0) & 0xFF);
	digitalWrite(SSD7317_TOUCH_CS, LOW);
	SPI.transfer(0x07);
	SPI.transfer(addr[1]);
	SPI.transfer(addr[0]);
	SPI.transfer(0x00);
	for(i = 0; i < num; i++)
		data[i] = SPI.transfer(0xFF);
	digitalWrite(SSD7317_TOUCH_CS, HIGH);
	return 0;
}

uint16_t SSD7317_TIC_CPU_BurstRead(uint16_t address, uint8_t data[], uint16_t num)
{
	uint8_t addr[2];
	uint16_t i;

	addr[0] = (uint8_t)((address >> 8) & 0xFF);
	addr[1] = (uint8_t)((address >> 0) & 0xFF);
	digitalWrite(SSD7317_TOUCH_CS, LOW);
	SPI.transfer(0x06);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(addr[1]);
	SPI.transfer(addr[0]);

	digitalWrite(SSD7317_TOUCH_CS, HIGH);
	delayMicroseconds(300); //200uS+ required before read
	digitalWrite(SSD7317_TOUCH_CS, LOW);

	SPI.transfer(0x07);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	for(i = 0; i < num; i++)
		data[i] = SPI.transfer(0xFF);
	digitalWrite(SSD7317_TOUCH_CS, HIGH);
	return 0;
}

uint16_t SSD7317_TIC_CPU_BurstWrite(uint16_t address, uint8_t data[], uint16_t num)
{
	uint8_t addr[2];
	uint16_t i;
	addr[0] = (uint8_t)((address >> 8) & 0xFF);
	addr[1] = (uint8_t)((address >> 0) & 0xFF);
	digitalWrite(SSD7317_TOUCH_CS, LOW);
	SPI.transfer(0x06);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(addr[1]);
	SPI.transfer(addr[0]);
	for(i = 0; i < num; i++)
		SPI.transfer(data[i]);
	digitalWrite(SSD7317_TOUCH_CS, HIGH);
	return 0;
}

//////////////////////////////////////////////////////////

static void SSD7317_Touch_Setup(void)
{
	//SSD7317 integrated touch controller init
	//the function of a lot of this is unknown
	//the manufacturer provides an init flow chart, we just follow it

	uint8_t wd8[8];
	int ram_code_size;
	int page_num, write_num;
	int i;
	LOG_LN("SSD7317_Touch_Setup()");

	// wait for touch ic's interrupt to go high after reset
	while (SSD7317_TOUCH_IRQ_RD == HIGH) { _NOP(); }
	LOG_LN("SSD7317_Touch_Setup() - Step1");

	// initialize SSD7317
	SSD7317_TIC_BIOS_RegRead();
	LOG_LN("SSD7317_Touch_Setup() - Step2");

	// write PM firmware blob
	ram_code_size = sizeof(SSD7317_PM_Content) / sizeof(SSD7317_PM_Content[0]);
	SSD7317_TIC_BIOS_RegWrite(RAM_BLOCK_PM);
	delayMicroseconds(300);
	for (i = 0; i < 24; i++)
		SSD7317_TIC_BIOS_BurstWrite_PROGMEM(0x0002 * i, (uint8_t*)SSD7317_PM_Content + SSD7317_TOUCH_FW_PAGE_SIZE * i, SSD7317_TOUCH_FW_PAGE_SIZE);
	LOG_LN("SSD7317_Touch_Setup() - Step3");

	//write TM firmware blob
	ram_code_size = sizeof(SSD7317_TM_Content) / sizeof(SSD7317_TM_Content[0]);
	SSD7317_TIC_BIOS_RegWrite(RAM_BLOCK_TM);
	delayMicroseconds(300);
	page_num = (ram_code_size % SSD7317_TOUCH_FW_PAGE_SIZE) ? (ram_code_size / SSD7317_TOUCH_FW_PAGE_SIZE) + 1 : ram_code_size / SSD7317_TOUCH_FW_PAGE_SIZE;
	for (i = 0; i < page_num; i++)
	{
		ram_code_size -= SSD7317_TOUCH_FW_PAGE_SIZE;
		write_num = (ram_code_size >= 0) ? SSD7317_TOUCH_FW_PAGE_SIZE : (ram_code_size + SSD7317_TOUCH_FW_PAGE_SIZE);
		SSD7317_TIC_BIOS_BurstWrite_PROGMEM(0x0002 * i, (uint8_t*)SSD7317_TM_Content + SSD7317_TOUCH_FW_PAGE_SIZE * i, write_num);
	}
	LOG_LN("SSD7317_Touch_Setup() - Step4");

	//write DM firmware blob
	ram_code_size = sizeof(SSD7317_DM_Content) / sizeof(SSD7317_DM_Content[0]);
	SSD7317_TIC_BIOS_RegWrite(RAM_BLOCK_DM);
	delayMicroseconds(300);
	for (i = 0; i < 4; i++)
		SSD7317_TIC_BIOS_BurstWrite_PROGMEM(0x0002 * i, (uint8_t*)SSD7317_DM_Content + SSD7317_TOUCH_FW_PAGE_SIZE * i, SSD7317_TOUCH_FW_PAGE_SIZE);
	LOG_LN("SSD7317_Touch_Setup() - Step5");

	//firware has been written, do some more misc inits
	SSD7317_TIC_BIOS_RegWrite(0x0000);
	delayMicroseconds(200);
	wd8[0] = 0x00; wd8[1] = 0x00;
	SSD7317_TIC_BIOS_BurstWrite(0x8300, wd8, 2);
	delayMicroseconds(200);
	wd8[0] = 0x03; wd8[1] = 0x00;
	SSD7317_TIC_BIOS_BurstWrite(0x8000, wd8, 2);
	delayMicroseconds(200);
	wd8[0] = 0x00; wd8[1] = 0x00;
	SSD7317_TIC_BIOS_BurstWrite(0x8000, wd8, 2);
	LOG_LN("SSD7317_Touch_Setup() - Step6");

    //wait for touch IC's interrupt to go high then low
	//this signals that the firmware blobs have been loaded successfully
	//  we should probably add a timeout here and re-attempt firmware blob
	//  uploading if timed-out
	while(SSD7317_TOUCH_IRQ_RD == LOW)	{ _NOP(); }
    while(SSD7317_TOUCH_IRQ_RD == HIGH)	{ _NOP(); }

	//firmware has been loaded, last inits
	wd8[0] = 0x00; wd8[1] = 0x00;
	SSD7317_TIC_CPU_BurstWrite(0x0043, wd8, 2);

	//done
	LOG_LN("SSD7317_Touch_Setup() done");
}

//////////////////////////////////////////////////////////

void SSD7317_NM_GestureEnable(uint16_t enable)
{
	//normal-mode touch gestures setup
	// bit 0 = enables incell tap
	// bit 1 = enables incell hold
	// bit 2 = enables incell double-tap
	// bit 3 = enables incell up/down swipes
	// bit 4 = not used
	// bit 5-7 = not used (must be 0)
	// bit 8 = enables outcell tap/hold
	// bit 9-15 = not used (must be 0)

	uint8_t		data[2];
	uint16_t	buf;

	if (enable & 0xFEE0)
		return;

	SSD7317_TIC_CPU_BurstRead(0x005D, data, 2);

	buf = (data[1] << 8) | data[0];
	buf = (buf & 0xFEE0) | enable;

	data[0] = (uint8_t) buf;
	data[1] = (uint8_t) (buf >> 8);
	SSD7317_TIC_CPU_BurstWrite(0x005D, data, 2);
}

void SSD7317_LPM_GestureEnable(uint16_t enable)
{
	//low-power mode touch gestures setup
	uint8_t		data[2];
	uint16_t	buf;

	if(enable & 0xFF1F)
		return;

	SSD7317_TIC_CPU_BurstRead(0x005D, data, 2);
	buf = (data[1] << 8) | data[0];
	buf = (buf & 0xFF1F) | enable;
	data[0] = (uint8_t) buf;
	data[1] = (uint8_t) (buf >> 8);
	SSD7317_TIC_CPU_BurstWrite(0x005D, data, 2);
}

void SSD7317_TouchThreshold_Set(uint16_t threshold)
{
	//set touch threshold of tap for NM & LPM mode
	//default is approx 200. lower value is more sensitive.
	uint8_t		wd[2];
	wd[0] = (uint8_t)((threshold >> 0) & 0xFF);
	wd[1] = (uint8_t)((threshold >> 8) & 0xFF);
	SSD7317_TIC_CPU_BurstWrite(0x005F, wd, 2);
}

void SSD7317_LongTap_Frames_Set(uint16_t frames)
{
	//set frame number with touch on for long tap gesture
	uint8_t         wd[2];
	wd[0] = (uint8_t)((frames >> 0) & 0xFF);
	wd[1] = (uint8_t)((frames >> 8) & 0xFF);
	SSD7317_TIC_CPU_BurstWrite(0x0082, wd, 2);
}

void SSD7317_SingleTap_MaxFrames_Set(uint16_t frames)
{
	//set max frame numbers with touch on for single tap gesture
	//if over this number, single tap fails
	uint8_t         wd[2];
	wd[0] = (uint8_t)((frames >> 0) & 0xFF);
	wd[1] = (uint8_t)((frames >> 8) & 0xFF);
	SSD7317_TIC_CPU_BurstWrite(0x0083, wd, 2);
}

void SSD7317_SkipFrames_AfterGestureReport(uint16_t frames)
{
	//set frame number skipped after gesture event recognized
	uint8_t         wd[2];
	wd[0] = (uint8_t)((frames >> 0) & 0xFF);
	wd[1] = (uint8_t)((frames >> 8) & 0xFF);
	SSD7317_TIC_CPU_BurstWrite(0x0085, wd, 2);
}

void SSD7317_LPM_ScanRate_Set(uint16_t rate)
{
	//set touch sensing scan rate setting for Low Power Mode
	uint8_t         wd[2];
	wd[0] = (uint8_t)((rate >> 0) & 0xFF);
	wd[1] = (uint8_t)((rate >> 8) & 0xFF);
	SSD7317_TIC_CPU_BurstWrite(0x00AC, wd, 2);
}

#define REPORT_MODE_GESTURE     0
#define REPORT_MODE_MODIFIED    4

void SSD7317_ReportingMode_Set(uint16_t mode)
{
	//set reporting mode
	uint8_t         wd[2];
	wd[0] = (uint8_t)((mode >> 0) & 0xFF);
	wd[1] = (uint8_t)((mode >> 8) & 0xFF);
	SSD7317_TIC_CPU_BurstWrite(0x0050, wd, 2);
	//SSD7317_TOUCH_IRQ_ReportMode(mode);
}

//////////////////////////////////////////////////////////

#endif
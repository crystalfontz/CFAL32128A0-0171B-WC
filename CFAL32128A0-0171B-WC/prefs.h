#pragma once
//==============================================================================
//
//  CRYSTALFONTZ CFAL32128A0-0171B-WC EXAMPLE FIRMWARE
//
//  Project settings
//
//==============================================================================

//The BS1 pin on the module selects display I2C or SPI interface mode:
// SPI: BS1 = low (gnd)
// I2C: BS1 = high (3.3v)

//this setting must correspond with the BS1 mode selection pin
#define OLED_SPI
//#define OLED_I2C

//The BS3 pin on the module selects touch I2C or SPI interface mode:
// SPI: BS3 = low (gnd)
// I2C: BS3 = high (3.3v)

//this setting must correspond with the BS3 mode selection pin
#define TOUCH_I2C
//#define TOUCH_SPI

//==============================================================================

//oled pixel dimensions
#define SSD7317_OLED_WIDTH		128
#define SSD7317_OLED_HEIGHT		32

//oled i2c interface pins on the Seeeduino
#define SSD7317_OLED_I2C_SCLK	(PIN_WIRE_SCL)	/*D0*/
#define SSD7317_OLED_I2C_SDA	(PIN_WIRE_SDA)	/*D1,D2*/
//SSD7317 DCS pin should be tied high
//SSD7317 DC pin should be tied low (sets I2C addr to 0x3C)

//oled spi interface pins on the Seeeduino
#define SSD7317_OLED_SPI_SCLK	(PIN_SPI_SCK)	/*D0*/
#define SSD7317_OLED_SPI_SDIN	(PIN_SPI_MOSI)	/*D1,D2*/
#define SSD7317_OLED_SPI_CS		(10)			/*DCS*/
#define SSD7317_OLED_SPI_DC		(8)				/*D/C*/

//oled common pins
#define SSD7317_OLED_RST		(3)		/*RES*/

//SPI coms frequency
#define SSD7317_OLED_SPI_FREQ	(8000000)
#define SSD7317_TOUCH_SPI_FREQ	(8000000)

//oled i2c address
#define SSD7317_OLED_I2C_ADDR	(0x3C)

//touch controller I2C interface pins on the Seeeduino
#define SSD7317_TOUCH_SCL		(SCL)			/*TD0*/
#define SSD7317_TOUCH_SDA		(SDA)			/*TD1,TD2*/
#define SSD7317_TOUCH_CS		(9)				/*TCS*/
#define SSD7317_TOUCH_RST		(4)		/*TRES*/
#define SSD7317_TOUCH_IRQ		(2)				/*IRQ*/

///////////////////////////////////////////////////////////////
//config checks - do not modify

#ifndef OLED_I2C
# ifndef OLED_SPI
# error OLED_I2C or OLED_SPI must be set in prefs.h
# endif
#endif

#ifdef OLED_I2C
# ifdef OLED_SPI
# error OLED_I2C or OLED_SPI (not both) must be set in prefs.h
# endif
#endif

#ifndef TOUCH_I2C
# ifndef TOUCH_SPI
# error TOUCH_I2C or TOUCH_SPI must be set in prefs.h
# endif
#endif

#ifdef TOUCH_I2C
# ifdef TOUCH_SPI
# error TOUCH_I2C or TOUCH_SPI (not both) must be set in prefs.h
# endif
#endif

#pragma once
//==============================================================================
//
//  CRYSTALFONTZ CFAL32128A0-0171B-WC EXAMPLE FIRMWARE
//
//  Project settings
//
//==============================================================================

// OLED INTERFACE MODE SELECTION
// The BS1 pin on the module selects display I2C or SPI interface mode:
//  SPI: BS1 = low (gnd)
//  I2C: BS1 = high (3.3v)
// One of the following must be enabled.
//==============================================================================
#define OLED_SPI
// #define OLED_I2C
//==============================================================================
// TOUCH INTERFACE SELECTION
// The BS3 pin on the module selects touch I2C or SPI interface mode:
// SPI: BS3 = low (gnd)
// I2C: BS3 = high (3.3v)
// One of the following must be enabled.
//==============================================================================
// #define TOUCH_SPI
#define TOUCH_I2C
//==============================================================================

#ifdef OLED_I2C
// OLED I2C ADDRESS SELECTION
// If using the OLED in I2C mode, the D/C pin selects one of two addresses:
// D/C low = 0x3C
// D/C high = 0x3D
// One of the following must be enabled.
#define SSD7317_OLED_I2C_ADDR	(0x3C)
//#define SSD7317_OLED_I2C_ADDR	(0x3D)
#endif

#ifdef TOUCH_I2C
// TOUCH I2C ADDRESS SELECTION
// If using the touch panel in I2C mode, the touch I2C base address is set
// via an OLED init command. One of two addresses can be selected.
// One of the following must be enabled.
#define SSD7317_TOUCH_I2C_ADDR	(0x53)
//#define SSD7317_TOUCH_I2C_ADDR	(0x5B)
#endif

//==============================================================================

//oled pixel dimensions
#define SSD7317_OLED_WIDTH		128
#define SSD7317_OLED_HEIGHT		32

#ifdef OLED_I2C
//oled i2c interface pins on the Seeeduino
#define SSD7317_OLED_I2C_SCLK	(PIN_WIRE_SCL)	/*D0*/
#define SSD7317_OLED_I2C_SDA	(PIN_WIRE_SDA)	/*D1,D2*/
//SSD7317 DCS pin should be tied high
//SSD7317 DC pin should be tied low (sets I2C addr to 0x3C)
#endif

#ifdef OLED_SPI
//oled spi interface pins on the Seeeduino
#define SSD7317_OLED_SPI_SCLK	(PIN_SPI_SCK)	/*D0*/
#define SSD7317_OLED_SPI_SDIN	(PIN_SPI_MOSI)	/*D1,D2*/
#define SSD7317_OLED_SPI_CS		(10)			/*DCS*/
#define SSD7317_OLED_SPI_DC		(8)				/*D/C*/
#endif

//oled common pins
#define SSD7317_OLED_RST		(3)				/*RES*/

//SPI coms frequency
#define SSD7317_OLED_SPI_FREQ	(8000000)
#define SSD7317_TOUCH_SPI_FREQ	(8000000)

//touch controller I2C interface pins on the Seeeduino
#define SSD7317_TOUCH_SCL		(SCL)			/*TD0*/
#define SSD7317_TOUCH_SDA		(SDA)			/*TD1,TD2*/
#define SSD7317_TOUCH_CS		(9)				/*TCS*/
#define SSD7317_TOUCH_RST		(4)				/*TRES*/
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
# ifndef SSD7317_OLED_I2C_ADDR
# error OLED I2C address not selected in prefs.h
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
# ifndef SSD7317_TOUCH_I2C_ADDR
# error TOUCH I2C address not selected in prefs.h
# endif
#endif

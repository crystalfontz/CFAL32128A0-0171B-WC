#pragma once
#include <Arduino.h>
#include "prefs.h"

//////////////////////////////////////////////

//touch CS
#define SSD7317_TOUCH_CS_CLR	digitalWrite(SSD7317_TOUCH_CS, LOW)
#define SSD7317_TOUCH_CS_SET	digitalWrite(SSD7317_TOUCH_CS, HIGH)
//touch IRQ pin
#define SSD7317_TOUCH_IRQ_RD	digitalRead(SSD7317_TOUCH_IRQ)
//touch I2C SCL
#define SSD7317_TOUCH_SCL_CLR	digitalWrite(SSD7317_TOUCH_SCL, LOW)
#define SSD7317_TOUCH_SCL_SET	digitalWrite(SSD7317_TOUCH_SCL, HIGH)
//touch I2C SDA
#define SSD7317_TOUCH_SDA_CLR	digitalWrite(SSD7317_TOUCH_SDA, LOW)
#define SSD7317_TOUCH_SDA_SET	digitalWrite(SSD7317_TOUCH_SDA, HIGH)
// #define SSD7317_TOUCH_SDA_RD	digitalRead(SSD7317_TOUCH_SDA) /*this is a function*/

typedef struct
{
	uint8_t Act;
	uint8_t Detail;
	uint8_t StartEnd;
	uint8_t Reserved;
	uint8_t Location;
} SSD7317_InTouch_t;

//////////////////////////////////////////////

extern volatile bool SSD7317_TouchData_Waiting;
extern volatile SSD7317_InTouch_t SSD7317_Gesture_Data;
extern volatile uint8_t SSD7317_Raw_Data[8];

void SSD7317_Touch_Init(void);
void SSD7317_Touch_HWI2C(bool enable);
void SSD7317_Touch_Handle(void);

//touch feature settings
void SSD7317_NM_GestureEnable(uint16_t enable);
void SSD7317_LPM_GestureEnable(uint16_t enable);
void SSD7317_TouchThreshold_Set(uint16_t threshold);
void SSD7317_LongTap_Frames_Set(uint16_t frames);
void SSD7317_SingleTap_MaxFrames_Set(uint16_t frames);
void SSD7317_SkipFrames_AfterGestureReport(uint16_t frames);
void SSD7317_LPM_ScanRate_Set(uint16_t rate);
void SSD7317_ReportingMode_Set(uint16_t mode);

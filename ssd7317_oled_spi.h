#pragma once
#include <Arduino.h>

//public functions
void SSD7317_OLED_Init(void);
void SSD7317_OLED_Blank(void);
void SSD7317_OLED_WriteBuffer(uint8_t *buf);

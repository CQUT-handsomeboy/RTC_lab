#ifndef TFTLCD_H

#define TFTLCD_H
#include "stdint.h"

#define HEIGHT	480
#define WIDTH	320

#define delay HAL_Delay


#define BACK_COLOR 0x0000
#define FRONT_COLOR 0xFFFF

void TFT_WriteCmd(uint16_t cmd);
void TFT_WriteData(uint16_t dat);
uint16_t TFT_ReadData();
uint16_t TFT_ReadID(void);
void TFT_Init();
void TFT_WriteCmdData(uint16_t cmd,uint16_t data);
void TFT_WriteData_Color(uint16_t color);
void TFT_Clear(uint16_t color);
void TFT_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);
void TFT_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void TFT_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t font,uint8_t mode);
void TFT_ShowLine(uint16_t x,uint16_t y,uint8_t size,uint8_t *p);
void TFT_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void TFT_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
#endif

#include "tftlcd.h"
#include "stdint.h"
#include "main.h"
#include "stdio.h"
#include "tftlcd_fonts.h"

void TFT_WriteCmd(uint16_t cmd)
{
    *(__IO uint16_t *)(0x6C000000) = cmd;
}

void TFT_WriteData(uint16_t dat)
{
    *(__IO uint16_t *)(0x6C000080) = dat;
}

uint16_t TFT_ReadData()
{
	return *(__IO uint16_t *)(0x6C000800);
}

uint16_t TFT_ReadID(void)
{
		uint16_t id;
		delay(20);
		TFT_WriteCmd(0Xd3);
		id = TFT_ReadData();
		id = TFT_ReadData();
		id = TFT_ReadData();
		id <<= 8;
		id |= TFT_ReadData();
		return id;
}

void TFT_Init()
{
	TFT_WriteCmd(0xFF);
	TFT_WriteCmd(0xFF);
	delay(5);

	TFT_WriteCmd(0xFF);
	TFT_WriteCmd(0xFF);
	TFT_WriteCmd(0xFF);
	TFT_WriteCmd(0xFF);
	delay(10);

	TFT_WriteCmd(0xB0);
	TFT_WriteData(0x00);

	TFT_WriteCmd(0xB3);
	TFT_WriteData(0x02);
	TFT_WriteData(0x00);
	TFT_WriteData(0x00);
	TFT_WriteData(0x00);

	TFT_WriteCmd(0xC0);
	TFT_WriteData(0x13);
	TFT_WriteData(0x3B);//480
	TFT_WriteData(0x00);
	TFT_WriteData(0x00);
	TFT_WriteData(0x00);
	TFT_WriteData(0x01);
	TFT_WriteData(0x00);//NW
	TFT_WriteData(0x43);

	TFT_WriteCmd(0xC1);
	TFT_WriteData(0x08);
	TFT_WriteData(0x1B);//CLOCK
	TFT_WriteData(0x08);
	TFT_WriteData(0x08);

	TFT_WriteCmd(0xC4);
	TFT_WriteData(0x11);
	TFT_WriteData(0x01);
	TFT_WriteData(0x73);
	TFT_WriteData(0x01);

	TFT_WriteCmd(0xC6);
	TFT_WriteData(0x00);

	TFT_WriteCmd(0xC8);
	TFT_WriteData(0x0F);
	TFT_WriteData(0x05);
	TFT_WriteData(0x14);
	TFT_WriteData(0x5C);
	TFT_WriteData(0x03);
	TFT_WriteData(0x07);
	TFT_WriteData(0x07);
	TFT_WriteData(0x10);
	TFT_WriteData(0x00);
	TFT_WriteData(0x23);

	TFT_WriteData(0x10);
	TFT_WriteData(0x07);
	TFT_WriteData(0x07);
	TFT_WriteData(0x53);
	TFT_WriteData(0x0C);
	TFT_WriteData(0x14);
	TFT_WriteData(0x05);
	TFT_WriteData(0x0F);
	TFT_WriteData(0x23);
	TFT_WriteData(0x00);

	TFT_WriteCmd(0x35);
	TFT_WriteData(0x00);

	TFT_WriteCmd(0x44);
	TFT_WriteData(0x00);
	TFT_WriteData(0x01);

	TFT_WriteCmd(0xD0);
	TFT_WriteData(0x07);
	TFT_WriteData(0x07);//VCI1
	TFT_WriteData(0x1D);//VRH
	TFT_WriteData(0x03);//BT

	TFT_WriteCmd(0xD1);
	TFT_WriteData(0x03);
	TFT_WriteData(0x5B);//VCM
	TFT_WriteData(0x10);//VDV

	TFT_WriteCmd(0xD2);
	TFT_WriteData(0x03);
	TFT_WriteData(0x24);
	TFT_WriteData(0x04);

	TFT_WriteCmd(0x2A);
	TFT_WriteData(0x00);
	TFT_WriteData(0x00);
	TFT_WriteData(0x01);
	TFT_WriteData(0x3F);//320

	TFT_WriteCmd(0x2B);
	TFT_WriteData(0x00);
	TFT_WriteData(0x00);
	TFT_WriteData(0x01);
	TFT_WriteData(0xDF);//480

	TFT_WriteCmd(0x36);
	TFT_WriteData(0x00);

	TFT_WriteCmd(0xC0);
	TFT_WriteData(0x13);

	TFT_WriteCmd(0x3A);
	TFT_WriteData(0x55);

	TFT_WriteCmd(0x11);
	delay(150);

	TFT_WriteCmd(0x29);
	delay(30);

	TFT_WriteCmd(0x2C);
}

void TFT_WriteCmdData(uint16_t cmd,uint16_t data)
{
	TFT_WriteCmd(cmd);
	TFT_WriteData(data);
}

void TFT_WriteData_Color(uint16_t color)
{
	TFT_WriteData(color);
}

void TFT_Clear(uint16_t color)
{
	uint16_t i, j ;

	TFT_Set_Window(0, 0, WIDTH-1,HEIGHT-1);
  	for(i=0; i<WIDTH; i++)
	{
		for (j=0; j<HEIGHT; j++)
		{
			TFT_WriteData_Color(color);
		}
	}
}

void TFT_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	TFT_WriteCmd(0x2A);
	TFT_WriteData(sx/256);
	TFT_WriteData(sx%256);
	TFT_WriteData(width/256);
	TFT_WriteData(width%256);

	TFT_WriteCmd(0x2B);
	TFT_WriteData(sy/256);
	TFT_WriteData(sy%256);
	TFT_WriteData(height/256);
	TFT_WriteData(height%256);

	TFT_WriteCmd(0x2C);
}

void TFT_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
#ifdef HEIGHT_AS_WIDTH
	TFT_Set_Window(y, HEIGHT - x ,1, 1);
	TFT_WriteData_Color(color);
#else
	TFT_Set_Window(x, y,x, y);
	TFT_WriteData_Color(color);
#endif
}

void TFT_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{
    uint8_t temp,t1,t;
	uint16_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);
 	num=num-' ';
	for(t=0;t<csize;t++)
	{
		if(size==12)temp=ascii_1206[num][t];
		else if(size==16)temp=ascii_1608[num][t];
		else if(size==24)temp=ascii_2412[num][t];
		else return;
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)TFT_DrawPoint(x,y,FRONT_COLOR);
			else if(mode==0)TFT_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=HEIGHT)return;		//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=HEIGHT)return;	//��������
				break;
			}
		}
	}
}

void TFT_ShowLine(uint16_t x,uint16_t y,uint8_t size,uint8_t *p)
{
	uint8_t x0 = x;
	while((*p<='~')&&(*p>=' '))
	{
			if((x > WIDTH) || (y > HEIGHT))
				break;
			TFT_ShowChar(x,y,*p,size,0);
			x+=size/2;
			p++;
	}
}

void TFT_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    // 绘制矩形的顶部边
    for (int i = x; i < x + width; i++) {
        TFT_DrawPoint(i, y, color);
    }

    // 绘制矩形的底部边
    for (int i = x; i < x + width; i++) {
        TFT_DrawPoint(i, y + height - 1, color);
    }

    // 绘制矩形的左侧边
    for (int i = y; i < y + height; i++) {
        TFT_DrawPoint(x, i, color);
    }

    // 绘制矩形的右侧边
    for (int i = y; i < y + height; i++) {
        TFT_DrawPoint(x + width - 1, i, color);
    }
}

void TFT_DrawAxis() {
  TFT_DrawLine(0,HEIGHT /2,WIDTH,HEIGHT/2,0xFFFF);
  TFT_DrawLine(WIDTH/2,0,WIDTH/2,HEIGHT,0xFFFF);
}

void TFT_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
	uint16_t t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向
	else if(delta_x==0)incx=0;//垂直线
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;//水平线
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )//画线输出
	{
		TFT_DrawPoint(uRow,uCol,color);//画点
		xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

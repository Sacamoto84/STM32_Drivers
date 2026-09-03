#include "TFT.h"

//32 бит BMP с альфа каналом
//void uTFT_BMP_From_File_Alpha(TFT_LCD_t *LCD, int32_t x0, int32_t y0,
//		char *Name, int offset);

////16x16
//const uint8_t lcd_image_mas[] =
//{ 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x26, 0x22,
//  0x26, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//  0x00, 0x3C, 0x66, 0x42, 0x64, 0x3F, 0x40, 0x40,
//  0x40, 0x3F, 0x64, 0x42, 0x62, 0x3C, 0x00, 0x00};

//const uint8_t sperma[] =
//{ 0x00, 0x0E, 0x3D, 0x7F, 0x3F, 0x0E, 0x00, 0x00};

//BMP pBmp10 = {
//	16,                  //x
//	16,                  //y
//	&lcd_image_mas[0],   //steam8
//	NULL,                //steam16
//	NULL,                //Palette
//	1                    //bit
//};

//BMP pBmpSp = {
//	8,                  //x
//	8,                  //y
//	&sperma[0],         //steam8
//	NULL,                //steam16
//	NULL,                //Palette
//	1                    //bit
//};

#if (LCD_COLOR)
void uTFT_Bitmap_From_Flash_Direct(TFT_LCD_t* LCD, uint16_t X, uint16_t Y, BMP * bmp)
{	
	int16_t xmin, xmax;
	int16_t ymin, ymax;

	int16_t deltaX1, deltaX2; 
	int16_t deltaY1, deltaY2; 
	const int16_t *p;
	
	//Измерение размера окна
	xmin = X;
  xmax = X + bmp->x - 1;
	
	if (xmin >= LCD->TFT_WIDTH ) return;
	if (xmax < 0) return;
	if (xmax >= LCD->TFT_WIDTH)  xmax = LCD->TFT_WIDTH - 1;
	if (xmin<0)	xmin = 0;

	
	ymin = Y;
  ymax = Y + bmp->y - 1;
	
	if (ymin >= LCD->TFT_HEIGHT) return;
	if (ymax < 0) return;
	if (ymax >= LCD->TFT_HEIGHT)  ymax = LCD->TFT_HEIGHT - 1;
	if (ymin<0) ymin = 0;
	
	if ((xmin==xmax)||(ymin==ymax)) return;
	
	deltaX1 = X;
	if (deltaX1 > 0) deltaX1 = 0;
	deltaX1 = deltaX1 * -1;
	
	deltaX2 = X + bmp->x - LCD->TFT_WIDTH;
	if (deltaX2 < 0) deltaX2 = 0;
	
	deltaY1 = Y;
	if (deltaY1 > 0) deltaY1 = 0;
	deltaY1 = deltaY1 * -1;
	
	deltaY2 = Y + bmp->y - LCD->TFT_HEIGHT;
	if (deltaY2 < 0) deltaY2 = 0;
	
	p = &bmp->steam16[deltaY1 * bmp->x];
	
	Spi16to8(LCD);
	if (LCD->GPIO_CS != NULL)	
		LCD_Select(LCD);
	__nop;
	__nop;
	
		switch (LCD->LCD_Driver)
	{
		case ST7789  : ST7789_AddrSet(LCD, xmin, ymin, xmax, ymax);  break;
		case ST7735  : ST7735_AddrSet(LCD, xmin, ymin, xmax, ymax);  break;
		default:
			break;
	}
		
	//ST7735_AddrSet(LCD, 0, 0, 79, 159);
	HAL_GPIO_WritePin(LCD->GPIO_DC, LCD->GPIO_Pin_DC, 1);
	
	Spi8to16(LCD);
	__nop;
	__nop;
	  for(uint32_t i = 0; i < (bmp->y - deltaY2 - deltaY1)  ;i++)
		{	  
			p += deltaX1;		
			for(uint32_t ii = 0; ii< (bmp->x - deltaX2 - deltaX1) ; ii++)
			{
				while( (LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0 ); 
			  LCD->hspi->Instance->DR = *p++;   
			}	
			p += deltaX2;	
		}		

	__nop;
	__nop;
	__nop;
	__nop;
	__nop;
	__nop;
	__nop;
	__nop;
	Spi16to8(LCD);
	__nop;
	__nop;
	if (LCD->GPIO_CS != NULL)	
		LCD_Unselect(LCD);	
}
#endif



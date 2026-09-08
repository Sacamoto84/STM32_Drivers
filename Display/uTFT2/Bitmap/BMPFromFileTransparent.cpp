#include "bitmap.h"

#if (FAT_FS)

void BMPFromFileTransparent(TFT * tft, int32_t x0, int32_t y0, char * Name, uint16_t tr_color)
{
	int res;
	uint32_t index;
	uint32_t index_max;
	int32_t x, y;
	//uint32_t dobavka_x;
	uint16_t color;

	uint8_t bmp_header_buffer[54]; //Буффер заголовка
	UINT bytesread;

	uint8_t * buf =  NULL;

	uint32_t offBits, width, height, clrUsed;
	uint16_t bitCount;

	res = f_open(&SDFile, Name, FA_READ);

	if (res == FR_OK)
	{
		buf = (uint8_t*)malloc(4096);
		if (buf == NULL) {
			f_close(&SDFile);
			return;
		}

		res = f_read (&SDFile, &bmp_header_buffer[0], 54, &bytesread);

		if (res == FR_OK && BMP_ParseHeader(bmp_header_buffer, bytesread,
				&offBits, &width, &height, &bitCount, &clrUsed))
		{

		if (clrUsed != 0) //Палитра используется
		{

		  uint16_t bmp_color_table[256];        //Сама палитра
		  uint32_t bmp_buffer_color_table[256]; //Буффер читаемый для патитры 1K

		  //Читаем таблицу палитры
		  f_read (&SDFile, &bmp_buffer_color_table[0], clrUsed * 4, &bytesread);

		  //Конвертируем таблицу плитры
		  for(uint32_t i=0; i < clrUsed;i++) bmp_color_table[i] = RGB888_RGB565(bmp_buffer_color_table[i]);

		  //Данные пикселей начинаются с bfOffBits
		  f_lseek(&SDFile, offBits);

		  //Максимальный индекс
		  index_max = width * height;

		  for(index = 0; index < index_max; index++)
		  {
			  if (index % 4096 == 0)
					f_read (&SDFile, buf, 4096, &bytesread);
			  x = (index % width) + x0;
			  y = height - (index / width) - 1 + y0;
				if (x <= (int32_t)width && bytesread >= (index % 4096) + 1)
				{
					color =  bmp_color_table[buf[index % 4096]];
                    if (color != tr_color )
					  tft->SetPixel(x,y, color);
				}
		  }

		}
		else if (bitCount == 24) //Палитра не используется, 24 бита на пиксель
		{
				f_lseek(&SDFile, offBits);

				index_max = width * height;
				for(index = 0; index < index_max; index++)
				{
					if (index % 1365 == 0)
						f_read (&SDFile, buf, 4095, &bytesread);
					x = (index % width) + x0;
					y = height - (index / width) - 1 + y0;
					if (x <= (int32_t)width && bytesread >= (index % 1365) * 3 + 3)
					{
						color = RGB565(buf[(index % 1365)*3+2], buf[(index % 1365)*3+1],buf[(index % 1365)*3]);
						if (color != tr_color )
					    tft->SetPixel(x,y, color);

					}
				}
		}
		}
		f_close(&SDFile);
	}

	if (buf)
	{
		free(buf);
		buf = NULL;
	}

}

#endif

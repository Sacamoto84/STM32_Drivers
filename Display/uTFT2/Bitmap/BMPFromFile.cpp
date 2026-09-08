#include "bitmap.h"

#if (FAT_FS)

//Только для картино кратнях 4 по ширине
void BMPFromFile(TFT * tft, int32_t x0, int32_t y0, char * Name)
{
	int res;
	uint32_t index;
	uint32_t index_max;
	int32_t x, y;
	//uint32_t dobavka_x;

	uint8_t bmp_header_buffer[54]; //Буффер заголовка
	UINT bytesread;

	uint32_t offBits, width, height, clrUsed;
	uint16_t bitCount;

	uint8_t BMP_From_File_buf[4096];

	res = f_open(&SDFile, Name, FA_READ);

	if (res == FR_OK)
	{
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
			  for(uint32_t i=0; i < clrUsed;i++)
				  bmp_color_table[i] = RGB888_RGB565(bmp_buffer_color_table[i]);

			  //Данные пикселей начинаются с bfOffBits
			  f_lseek(&SDFile, offBits);

			  //Максимальный индекс
			  index_max = width * height;

			  for(index = 0; index < index_max; index++)
			  {
				  if (index % 4096 == 0)
						f_read (&SDFile, &BMP_From_File_buf[0], 4096, &bytesread);

				  x = (index % width) + x0;
				  y = height - (index / width) - 1 + y0;
				  //Пишем только реально прочитанные байты
				  if (bytesread >= (index % 4096) + 1)
						tft->SetPixel(x,y, bmp_color_table[BMP_From_File_buf[index % 4096]]);
			  }

			}
			else if (bitCount == 24) //Палитра не используется, 24 бита на пиксель
			{
				//Данные пикселей начинаются с bfOffBits
				f_lseek(&SDFile, offBits);

				index_max = width * height;
				for(index = 0; index < index_max; index++)
				{
					if (index % 1365 == 0)
						f_read (&SDFile, &BMP_From_File_buf[0], 4095, &bytesread);

					x = (index % width) + x0;
					y = height - (index / width) - 1 + y0;
					//Пишем только реально прочитанные байты
					if (bytesread >= (index % 1365) * 3 + 3)
						tft->SetPixel(x,y, RGB565(BMP_From_File_buf[(index % 1365)*3+2], BMP_From_File_buf[(index % 1365)*3+1],BMP_From_File_buf[(index % 1365)*3]));
				}
			}
		}
		f_close(&SDFile);
	}


}

#endif

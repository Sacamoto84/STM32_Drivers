#include "bitmap.h"

#if (FAT_FS)

//32 бит BMP с альфа каналом
List_Update_Particle BMPFromFile32b(TFT * tft, int32_t x0, int32_t y0, char * Name, int offset, int swap)
{
	List_Update_Particle result;

	int res;
	uint32_t index;
	uint32_t index_max;
	int32_t x, y;
	//uint32_t dobavka_x;
	uint16_t sColor;
	//uint8_t  sAlpha;
	float    sAlpha_Float, oneminusalpha;
	uint16_t dColor;
	uint8_t  sR,sG,sB;
	uint8_t  dR,dG,dB;

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
			result.H = height;
			result.W = width;

			result.x0  = x0;
			result.y0  = y0;

			result.x1  = x0 + result.W - 1;
			result.y1  = y0 + result.H - 1;

			//Пропуск байтов между заголовком и данными пикселей
			if (offset > 0) {
				//Продвигаем указатель файла без чтения в буфер
				if (f_lseek(&SDFile, f_tell(&SDFile) + (uint32_t)offset) != FR_OK) {
					f_close(&SDFile);
					return result;
				}
			}

			index_max = result.H * result.W;

			for(index = 0; index < index_max; index++)
			{
				if (index % 1024 == 0)
					f_read (&SDFile, &BMP_From_File_buf[0], 4096, &bytesread);

				//Обрабатываем только реально прочитанные байты
				if (bytesread < (index % 1024) * 4 + 4)
					break;

			    x = (index % width) + x0;
			    y = height - (index / width) - 1 + y0;

			    sAlpha_Float = BMP_From_File_buf[(index % 1024)*4+3] / 255.0;

			    if (swap == 0)
			    {
				 sR = BMP_From_File_buf[(index % 1024)*4+2];
				 sG = BMP_From_File_buf[(index % 1024)*4+1];
				 sB = BMP_From_File_buf[(index % 1024)*4];
			    }
			    else
			    {
				 sB = BMP_From_File_buf[(index % 1024)*4+2];
				 sG = BMP_From_File_buf[(index % 1024)*4+1];
				 sR = BMP_From_File_buf[(index % 1024)*4];
			    }

				dColor = tft->GetPixel(x, y);

				dR = (dColor & 0xF800) >> 8;
				dG = (dColor & 0x7E0) >> 3;
				dB = (dColor & 0x1F) << 3;

				oneminusalpha = 1.0F - sAlpha_Float;

				sR = ((sR * sAlpha_Float) + (oneminusalpha * dR));
                sG = ((sG * sAlpha_Float) + (oneminusalpha * dG));
                sB = ((sB * sAlpha_Float) + (oneminusalpha * dB));

			    sColor = RGB565(sR, sG, sB);
				tft->SetPixel(x, y, sColor);
			}
		}
		else
		{
			//Ошибочный заголовок - вернуть пустую область
			result.x0 = 0; result.y0 = 0; result.x1 = 0; result.y1 = 0;
			result.W = 0;  result.H = 0;
		}
		f_close(&SDFile);
	}

	return result;
}

#endif

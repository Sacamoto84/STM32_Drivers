#include "bitmap.h"

#if (FAT_FS)

//32 бит BMP с альфа каналом
//offset - дополнительное смещение данных после bfOffBits (для нестандартных файлов)
//swap - 0: формат [B][G][R][A], 1: формат [R][G][B][A] (порядок байтов цвета)
List_Update_Particle BMPFromFile32b(TFT * tft, int32_t x0, int32_t y0, const char * Name, int offset, int swap)
{
	//Детерминированный результат даже при ошибках
	List_Update_Particle result = {0, 0, 0, 0, 0, 0};

	int res;
	uint32_t index;
	uint32_t index_max;
	int32_t x, y;
	uint16_t sColor;
	float    sAlpha_Float, oneminusalpha;
	uint16_t dColor;
	uint8_t  sR,sG,sB;
	uint8_t  dR,dG,dB;

	uint8_t bmp_header_buffer[54]; //Буфер заголовка
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
			if (bitCount != 32) {
				//Не 32-битный файл - не декодируем вслепую
				f_close(&SDFile);
				return result;
			}

			result.H = (int16_t)height;
			result.W = (int16_t)width;

			result.x0  = (int16_t)x0;
			result.y0  = (int16_t)y0;

			result.x1  = (int16_t)(x0 + result.W - 1);
			result.y1  = (int16_t)(y0 + result.H - 1);

			//Старт данных: bfOffBits из заголовка + пользовательское смещение
			//(раньше offBits игнорировался и skip делался от текущей позиции -
			//для файлов с палитрой/V4-заголовком читали не оттуда)
			if (f_lseek(&SDFile, offBits + (uint32_t)offset) != FR_OK) {
				f_close(&SDFile);
				return result;
			}

			index_max = (uint32_t)result.H * result.W;

			for(index = 0; index < index_max; index++)
			{
				if (index % 1024 == 0) {
					//Дочитываем блок; обрабатываем только реально прочитанное
					if (f_read (&SDFile, &BMP_From_File_buf[0], 4096, &bytesread) != FR_OK)
						break;
				}

				if (bytesread < (index % 1024) * 4 + 4)
					break;

			    x = (index % width) + x0;
			    y = (int32_t)height - (index / width) - 1 + y0;

			    sAlpha_Float = BMP_From_File_buf[(index % 1024)*4+3] / 255.0F;

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

				sR = (uint8_t)((sR * sAlpha_Float) + (oneminusalpha * dR));
                sG = (uint8_t)((sG * sAlpha_Float) + (oneminusalpha * dG));
                sB = (uint8_t)((sB * sAlpha_Float) + (oneminusalpha * dB));

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

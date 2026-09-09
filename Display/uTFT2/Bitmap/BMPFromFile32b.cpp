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
	uint8_t bmp_header_buffer[54]; //Буфер заголовка
	UINT bytesread;

	uint32_t offBits, width, height, clrUsed;
	uint16_t bitCount;

	static uint8_t BMP_From_File_buf[1024] __attribute__((aligned(4)));

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

			uint32_t buf_pos = 0;
			uint32_t pixels_in_buf = 0;

			for (int32_t row = 0; row < (int32_t)height; row++)
			{
				int32_t y = (int32_t)height - 1 - row + y0;
				for (int32_t col = 0; col < (int32_t)width; col++)
				{
					if (buf_pos >= pixels_in_buf) {
						if (f_read(&SDFile, &BMP_From_File_buf[0], sizeof(BMP_From_File_buf), &bytesread) != FR_OK)
							goto done_draw;
						pixels_in_buf = bytesread / 4;
						buf_pos = 0;
						if (pixels_in_buf == 0)
							goto done_draw;
					}

					uint32_t p_offset = buf_pos * 4;
					buf_pos++;

					uint8_t alpha = BMP_From_File_buf[p_offset + 3];
					if (alpha == 0) continue;

					int32_t x = col + x0;
					uint8_t sR, sG, sB;
					if (swap == 0) {
						sR = BMP_From_File_buf[p_offset + 2];
						sG = BMP_From_File_buf[p_offset + 1];
						sB = BMP_From_File_buf[p_offset];
					} else {
						sB = BMP_From_File_buf[p_offset + 2];
						sG = BMP_From_File_buf[p_offset + 1];
						sR = BMP_From_File_buf[p_offset];
					}

					uint16_t fgColor = RGB565(sR, sG, sB);
					if (alpha == 255) {
						tft->SetPixel(x, y, fgColor);
					} else {
						uint16_t dColor = tft->GetPixel(x, y);
						tft->SetPixel(x, y, tft->alphaBlend(alpha, fgColor, dColor));
					}
				}
			}
done_draw: ;
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

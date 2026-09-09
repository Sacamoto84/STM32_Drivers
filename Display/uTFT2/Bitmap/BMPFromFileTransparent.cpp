#include "bitmap.h"

#include <stdlib.h> //malloc/free

#if (FAT_FS)

//Вывод несжатого BMP (1/4/8/24 бита) с пропуском пикселей цвета tr_color
void BMPFromFileTransparent(TFT * tft, int32_t x0, int32_t y0, const char * Name, uint16_t tr_color)
{
	uint8_t bmp_header_buffer[54]; //Буфер заголовка
	static uint8_t lineBuf[4096] __attribute__((aligned(4))); //Статический буфер строки

	UINT bytesread;

	uint32_t offBits, width, height, clrUsed;
	uint16_t bitCount;

	if (f_open(&SDFile, Name, FA_READ) != FR_OK)
		return;

	int res = f_read(&SDFile, &bmp_header_buffer[0], 54, &bytesread);

	if (res == FR_OK && BMP_ParseHeader(bmp_header_buffer, bytesread,
			&offBits, &width, &height, &bitCount, &clrUsed))
	{
		BMP_DrawFile(tft, &SDFile, offBits, width, height, bitCount,
				clrUsed, x0, y0, 1, tr_color, lineBuf, sizeof(lineBuf));
	}

	f_close(&SDFile);
}

#endif

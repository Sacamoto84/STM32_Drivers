#include "bitmap.h"

#include <stdlib.h> //malloc/free

#if (FAT_FS)

//Вывод несжатого BMP (1/4/8/24 бита) с пропуском пикселей цвета tr_color
void BMPFromFileTransparent(TFT * tft, int32_t x0, int32_t y0, const char * Name, uint16_t tr_color)
{
	uint8_t bmp_header_buffer[54]; //Буфер заголовка
	uint8_t *buf = NULL;

	UINT bytesread;

	uint32_t offBits, width, height, clrUsed;
	uint16_t bitCount;

	if (f_open(&SDFile, Name, FA_READ) != FR_OK)
		return;

	buf = (uint8_t*)malloc(4096); //Буфер строки картинки
	if (buf == NULL) {
		f_close(&SDFile);
		return;
	}

	int res = f_read(&SDFile, &bmp_header_buffer[0], 54, &bytesread);

	if (res == FR_OK && BMP_ParseHeader(bmp_header_buffer, bytesread,
			&offBits, &width, &height, &bitCount, &clrUsed))
	{
		BMP_DrawFile(tft, &SDFile, offBits, width, height, bitCount,
				clrUsed, x0, y0, 1, tr_color, buf, 4096);
	}

	f_close(&SDFile);

	free(buf);
}

#endif

#include "bitmap.h"

#if (FAT_FS)

//Вывод несжатого BMP (1/4/8/24 бита) из файла.
//Построчное чтение с учетом выравнивания строк до 4 байт.
void BMPFromFile(TFT * tft, int32_t x0, int32_t y0, const char * Name)
{
	uint8_t bmp_header_buffer[54]; //Буфер заголовка
	static uint8_t lineBuf[1024] __attribute__((aligned(4))); //Буфер строки картинки (хватает для палитры 256*4 и строки до 341px 24bpp)
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
				clrUsed, x0, y0, 0, 0, lineBuf, sizeof(lineBuf));
	}

	f_close(&SDFile);
}

#endif

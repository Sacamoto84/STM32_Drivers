#include "bitmap.h"

// 756us 216MHz
//Копирует bmp->steam16 размером с весь экран в buffer16
void BitmapFromFlashBackground16bit(TFT * tft, Bitmap *bmp) {

	uint32_t pixels = (uint32_t)tft->LCD->TFT_HEIGHT * tft->LCD->TFT_WIDTH;
	uint32_t count = pixels / 2; //32-битных слов

	//Быстрый путь: оба буфера выровнены на 4 байта
	if ((((uintptr_t)&tft->LCD->buffer16[0]) & 3u) == 0 &&
		(((uintptr_t)bmp->steam16) & 3u) == 0) {

		const uint32_t *p16 = (const uint32_t *)bmp->steam16;
		uint32_t *p = (uint32_t *)&tft->LCD->buffer16[0];

		while (count >= 16) {
			*p++ = *p16++; *p++ = *p16++; *p++ = *p16++; *p++ = *p16++;
			*p++ = *p16++; *p++ = *p16++; *p++ = *p16++; *p++ = *p16++;
			*p++ = *p16++; *p++ = *p16++; *p++ = *p16++; *p++ = *p16++;
			*p++ = *p16++; *p++ = *p16++; *p++ = *p16++; *p++ = *p16++;
			count -= 16;
		}
		while (count--)
			*p++ = *p16++;

		//Нечетный хвост
		if (pixels & 1u)
			tft->LCD->buffer16[pixels - 1] = bmp->steam16[pixels - 1];
	} else {
		//Не выровнено - копируем по 16 бит
		const uint16_t *ps = bmp->steam16;
		uint16_t *pd = &tft->LCD->buffer16[0];
		while (pixels--)
			*pd++ = *ps++;
	}

}

#include "bitmap.h"

// 756us 216MHz
//Копирует bmp->steam16 размером с весь экран в buffer16
void BitmapFromFlashBackground16bit(TFT * tft, Bitmap *bmp) {

	//Прямая запись в buffer16 - только для 16-битного фреймбуфера
	if (tft->LCD->Bit != 16) return;
	//Источник должен покрывать весь экран, иначе чтение за его границей
	if (bmp->W < tft->LCD->TFT_WIDTH || bmp->H < tft->LCD->TFT_HEIGHT) return;

	const uint16_t *src16 = (bmp->steam16 != NULL) ? bmp->steam16
			: (const uint16_t *)bmp->data;

	uint32_t pixels = (uint32_t)tft->LCD->TFT_HEIGHT * tft->LCD->TFT_WIDTH;
	uint32_t count = pixels / 2; //32-битных слов

	//Быстрый путь: оба буфера выровнены на 4 байта
	if ((((uintptr_t)&tft->LCD->buffer16[0]) & 3u) == 0 &&
		(((uintptr_t)src16) & 3u) == 0) {

		const uint32_t *p16 = (const uint32_t *)src16;
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
			tft->LCD->buffer16[pixels - 1] = src16[pixels - 1];
	} else {
		//Не выровнено - копируем по 16 бит
		const uint16_t *ps = src16;
		uint16_t *pd = &tft->LCD->buffer16[0];
		while (pixels--)
			*pd++ = *ps++;
	}

}

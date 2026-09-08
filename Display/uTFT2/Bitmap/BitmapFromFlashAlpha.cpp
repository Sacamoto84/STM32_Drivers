#include "bitmap.h"

//32 бит BMP с альфа каналом Сохранять как инвертированая альфа и свап  , customAlpha = 1.0 полная альфа
void BitmapFromFlashAlpha(TFT * tft, int32_t x0, int32_t y0, Bitmap *bmp, float customAlpha) {

	//Прямая запись в buffer16 - только для 16-битного фреймбуфера
	if (tft->LCD->Bit != 16) return;

	//steam32 заполнен getResBitmapID; для вручную собранных Bitmap - data
	const uint32_t *src32 = (bmp->steam32 != NULL) ? bmp->steam32 : bmp->data;

	uint32_t index_max = (uint32_t)bmp->W * bmp->H;

	for (uint32_t index = 0; index < index_max; index++) {
		int32_t x = (int32_t)(index % bmp->W) + x0;
		//Вертикальный флип (формат хранится инвертированным):
		//строка fr рисуется как y0 + H - 1 - fr для любого столбца
		//(старая формула теряла первый пиксель каждой строки)
		int32_t y = y0 + (int32_t)bmp->H - 1 - (int32_t)(index / bmp->W);

		//Клиппинг: не выходим за границы фреймбуфера
		if (x < 0 || x >= tft->LCD->TFT_WIDTH ||
			y < 0 || y >= tft->LCD->TFT_HEIGHT)
			continue;

		float sAlpha_Float = (float)((src32[index] >> 24) / 255.0F) * customAlpha;

		uint8_t sR = src32[index] >> 16;
		uint8_t sG = src32[index] >> 8;
		uint8_t sB = src32[index] & 0xFF;

		uint16_t dColor = tft->LCD->buffer16[x + y * tft->LCD->TFT_WIDTH];

		uint8_t dR = (dColor & 0xF800) >> 8;
		uint8_t dG = (dColor & 0x7E0) >> 3;
		uint8_t dB = (dColor & 0x1F) << 3;

		float oneminusalpha = 1.0F - sAlpha_Float;

		sR = (uint8_t)((sR * sAlpha_Float) + (oneminusalpha * dR));
		sG = (uint8_t)((sG * sAlpha_Float) + (oneminusalpha * dG));
		sB = (uint8_t)((sB * sAlpha_Float) + (oneminusalpha * dB));

		uint16_t sColor = RGB565(sR, sG, sB);
		tft->LCD->buffer16[x + y * tft->LCD->TFT_WIDTH] = sColor;
	}
}

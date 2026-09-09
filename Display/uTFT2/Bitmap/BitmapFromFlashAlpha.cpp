#include "bitmap.h"

//32 бит BMP с альфа каналом Сохранять как инвертированая альфа и свап  , customAlpha = 1.0 полная альфа
void BitmapFromFlashAlpha(TFT * tft, int32_t x0, int32_t y0, Bitmap *bmp, float customAlpha) {

	//Прямая запись в buffer16 - только для 16-битного фреймбуфера
	if (tft->LCD->Bit != 16 || tft->LCD->buffer16 == NULL || bmp == NULL) return;
	if (bmp->W == 0 || bmp->H == 0) return;

	//steam32 заполнен getResBitmapID; для вручную собранных Bitmap - data
	const uint32_t *src32 = (bmp->steam32 != NULL) ? bmp->steam32 : bmp->data;
	if (src32 == NULL) return;

	if (customAlpha <= 0.0f) return;
	uint16_t global_alpha = (customAlpha >= 1.0f) ? 256 : (uint16_t)(customAlpha * 256.0f);

	for (int32_t row = 0; row < (int32_t)bmp->H; row++) {
		//Вертикальный флип (формат хранится инвертированным):
		int32_t y = y0 + (int32_t)bmp->H - 1 - row;
		if (y < 0 || y >= tft->LCD->TFT_HEIGHT) continue;

		uint32_t src_row_offset = (uint32_t)row * bmp->W;
		uint32_t dst_row_offset = (uint32_t)y * tft->LCD->TFT_WIDTH;

		for (int32_t col = 0; col < (int32_t)bmp->W; col++) {
			int32_t x = x0 + col;
			if (x < 0 || x >= tft->LCD->TFT_WIDTH) continue;

			uint32_t pixel = src32[src_row_offset + col];
			uint8_t raw_alpha = (uint8_t)(pixel >> 24);
			if (raw_alpha == 0) continue;

			uint8_t alpha = (global_alpha == 256) ? raw_alpha : (uint8_t)((raw_alpha * global_alpha) >> 8);
			if (alpha == 0) continue;

			uint8_t sR = (uint8_t)(pixel >> 16);
			uint8_t sG = (uint8_t)(pixel >> 8);
			uint8_t sB = (uint8_t)(pixel);
			uint16_t sColor = RGB565(sR, sG, sB);

			uint32_t fb_idx = dst_row_offset + (uint32_t)x;
			if (alpha == 255) {
				tft->LCD->buffer16[fb_idx] = sColor;
			} else {
				uint16_t dColor = tft->LCD->buffer16[fb_idx];
				tft->LCD->buffer16[fb_idx] = tft->alphaBlend(alpha, sColor, dColor);
			}
		}
	}
}

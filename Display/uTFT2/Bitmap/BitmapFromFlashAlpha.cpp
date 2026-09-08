#include "bitmap.h"

//32 бит BMP с альфа каналом Сохранять как инвертированая альфа и свап  , customAlpha = 1.0 полная альфа
void BitmapFromFlashAlpha(TFT * tft, int32_t x0, int32_t y0, Bitmap *bmp, float customAlpha) {

	volatile uint32_t index;
	volatile uint32_t index_max;
	volatile int32_t x, y;
	volatile uint16_t sColor;
	volatile float sAlpha_Float, oneminusalpha;
	volatile uint16_t dColor;
	volatile uint8_t sR, sG, sB;
	volatile uint8_t dR, dG, dB;

	index_max = bmp->W * bmp->H;

	for (index = 0; index < index_max; index++) {
		x = (index % (bmp->W)) + x0;
		y =  bmp->H - ((index_max-index) / (bmp->W)) - 1 + y0;

		//Клиппинг: не выходим за границы фреймбуфера
		if (x < 0 || x >= tft->LCD->TFT_WIDTH ||
			y < 0 || y >= tft->LCD->TFT_HEIGHT)
			continue;

		sAlpha_Float = (float) ((bmp->steam32[index] >> 24) / 255.0F)* customAlpha;

		sR = bmp->steam32[index] >> 16;
		sG = bmp->steam32[index] >> 8;
		sB = bmp->steam32[index] & 0xFF;

		dColor = tft->LCD->buffer16[x + y * tft->LCD->TFT_WIDTH]; //GetPixel(x, y);

		dR = (dColor & 0xF800) >> 8;
		dG = (dColor & 0x7E0) >> 3;
		dB = (dColor & 0x1F) << 3;

		oneminusalpha = 1.0F - sAlpha_Float;

		sR = ((sR * sAlpha_Float) + (oneminusalpha * dR));
		sG = ((sG * sAlpha_Float) + (oneminusalpha * dG));
		sB = ((sB * sAlpha_Float) + (oneminusalpha * dB));

		sColor = RGB565(sR, sG, sB);
		tft->LCD->buffer16[x + y * tft->LCD->TFT_WIDTH] = sColor;
	}
}

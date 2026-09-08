#include "bitmap.h"

void BitmapFromFlashTransparent(TFT * tft, uint16_t X, uint16_t Y,	Bitmap bmp, uint16_t TrColor) {

	if (bmp.bit == 1) {
		const uint8_t *p;
		p = (uint8_t *)bmp.data;

		int32_t pX;
		int32_t pY;
		uint8_t tmpCh;
		uint8_t bL;

		pY = Y;
		while (pY < (int32_t)Y + bmp.H) {
			pX = X;
			while (pX < (int32_t)X + bmp.W) {
				bL = 0;
				tmpCh = *p++;
				if (tmpCh) {
					while (bL < 8) {
						if (tmpCh & 0x01)
							tft->SetPixel(pX, pY + bL, 1);
						tmpCh >>= 1;
						if (tmpCh)
							bL++;
						else {
							pX++;
							break;
						}
					}
				} else {
					pX++;
				}
			}
			pY += 8;
		}

	}

	if (bmp.bit == 16) {
		//X, Y - uint16_t, отрицательными быть не могут
		int32_t x0 = X;
		int32_t y0 = Y;
		int32_t x1 = (int32_t)X + bmp.W; //эксклюзивная граница
		int32_t y1 = (int32_t)Y + bmp.H;
		int32_t W = tft->LCD->TFT_WIDTH;
		int32_t H = tft->LCD->TFT_HEIGHT;

		if (x1 > W) x1 = W;
		if (y1 > H) y1 = H;
		if (x0 >= x1 || y0 >= y1) return;

		//Пропущенные столбцы/строки источника
		uint32_t skip_cols = (uint32_t)(x0 - X);
		uint32_t skip_rows = (uint32_t)(y0 - Y);

		const uint16_t *p16 = bmp.steam16 + skip_rows * bmp.W + skip_cols;

		for (int32_t pY = y0; pY < y1; pY++) {
			const uint16_t *src = p16;
			p16 += bmp.W;
			uint16_t *dst = &tft->LCD->buffer16[pY * W];
			for (int32_t pX = x0; pX < x1; pX++) {
				uint16_t temp = *src++;
				if (TrColor != temp)
					*dst = temp;
				dst++;
			}
		}
	}
}

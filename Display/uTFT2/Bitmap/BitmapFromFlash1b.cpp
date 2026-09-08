#include "bitmap.h"

void BitmapFromFlash1b(TFT * tft, int16_t X, int16_t Y, Bitmap *bmp)
{
	if (bmp->bit == 1) {

			int32_t pX;
			int32_t pY;
			uint8_t tmpCh;
			uint8_t bL;
			const uint8_t *p;

			p = (uint8_t *)bmp->data;

			pY = Y;
			while (pY < Y + bmp->H) {
				pX = X;
				while (pX < X + bmp->W) {
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

}

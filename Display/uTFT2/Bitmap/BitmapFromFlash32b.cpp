#include "bitmap.h"

//32 бит BMP с альфа каналом Сохранять как инвертированая альфа и свап  , customAlpha = 1.0 полная альфа
//Формат: [alpha][R][G][B] на пиксель
void BitmapFromFlash32b(TFT * tft, int32_t x0, int32_t y0,	Bitmap *bmp) {
	//Прямая запись в buffer16 - только для 16-битного фреймбуфера
	if (tft->LCD->Bit != 16) return;

	uint32_t sColor;
	uint32_t dColor;

	uint32_t sR, sG, sB;
	uint32_t dR, dG, dB;
	uint32_t R, G, B;

	int32_t alpha;
	int32_t oneminusalpha;

	int32_t X0 = (x0 < 0) ? 0 : x0;
	int32_t Y0 = (y0 < 0) ? 0 : y0;
	int32_t X1 = (int32_t)x0 + bmp->W; //эксклюзивная граница
	int32_t Y1 = (int32_t)y0 + bmp->H;
	int32_t W = tft->LCD->TFT_WIDTH;
	int32_t H = tft->LCD->TFT_HEIGHT;

	if (X1 > W) X1 = W;
	if (Y1 > H) Y1 = H;
	if (X0 >= X1 || Y0 >= Y1) return;

	//Пропущенные столбцы/строки источника
	uint32_t skip_cols = (uint32_t)(X0 - x0);
	uint32_t skip_rows = (uint32_t)(Y0 - y0);

	const uint8_t *row = (const uint8_t *)bmp->data
			+ (skip_rows * bmp->W + skip_cols) * 4;

	for (int32_t pY = Y0; pY < Y1; pY++) {
		const uint8_t *p8 = row;
		row += (uint32_t)bmp->W * 4;
		for (int32_t pX = X0; pX < X1; pX++)
		{
			uint32_t deltaX = (uint32_t)pX + (uint32_t)pY * W;

			alpha = *p8++;
			sR    = *p8++;
			sG    = *p8++;
			sB    = *p8++;

			dColor = tft->LCD->buffer16[deltaX];

			dR = (dColor & 0xF800) >> 8;
			dG = (dColor & 0x7E0)  >> 3;
			dB = (dColor & 0x1F)   << 3;

			oneminusalpha = 255 - alpha;

			R = (uint8_t)(((sR * alpha) + (oneminusalpha * dR)) >> 11 );
			G = (uint8_t)(((sG * alpha) + (oneminusalpha * dG)) >> 10 );
			B = (uint8_t)(((sB * alpha) + (oneminusalpha * dB)) >> 11 );

			sColor = (R << 11) | (G << 5) | B;
			tft->LCD->buffer16[deltaX] = sColor;
		}
	}
};

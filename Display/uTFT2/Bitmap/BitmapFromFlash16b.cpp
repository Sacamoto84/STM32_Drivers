#include "bitmap.h"

//16 бит, без клиппинга экрана не вызывать
void BitmapFromFlash16b(TFT * tft, int16_t X, int16_t Y, Bitmap *bmp) {
	//Прямая запись в buffer16 - только для 16-битного фреймбуфера
	if (tft->LCD->Bit != 16) return;

	int32_t x0 = (X < 0) ? 0 : X;
	int32_t y0 = (Y < 0) ? 0 : Y;
	int32_t x1 = (int32_t)X + bmp->W; //эксклюзивная граница
	int32_t y1 = (int32_t)Y + bmp->H;
	int32_t W = tft->LCD->TFT_WIDTH;
	int32_t H = tft->LCD->TFT_HEIGHT;

	if (x1 > W) x1 = W;
	if (y1 > H) y1 = H;
	if (x0 >= x1 || y0 >= y1) return;

	//Пропущенные столбцы/строки источника
	uint32_t skip_cols = (uint32_t)(x0 - X);
	uint32_t skip_rows = (uint32_t)(y0 - Y);

	const uint16_t *p16 = (const uint16_t *)bmp->data
			+ skip_rows * bmp->W + skip_cols;

	for (int32_t pY = y0; pY < y1; pY++) {
		const uint16_t *src = p16;
		p16 += bmp->W;
		uint16_t *dst = &tft->LCD->buffer16[pY * W + x0];
		for (int32_t pX = x0; pX < x1; pX++)
			*dst++ = *src++;
	}
}

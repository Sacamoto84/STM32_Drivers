#include "TFT.h"
#include <math.h>       /* round, floor, ceil, trunc */

#include "TFT_color.h"

// ----- Line ----
void TFT::Line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t c) {

	int32_t dx, dy, sx, sy, err, e2, i, tmp;

	/* Check for overflow */
	if (x0 >= LCD->TFT_WIDTH) {
		x0 = LCD->TFT_WIDTH - 1;
	}
	if (x1 >= LCD->TFT_WIDTH) {
		x1 = LCD->TFT_WIDTH - 1;
	}
	if (y0 >= LCD->TFT_HEIGHT) {
		y0 = LCD->TFT_HEIGHT - 1;
	}
	if (y1 >= LCD->TFT_HEIGHT) {
		y1 = LCD->TFT_HEIGHT - 1;
	}

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2;

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			SetPixel(x0, i, c);
		}

		/* Return from function */
		return;
	}

	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			SetPixel(i, y0, c);
		}

		/* Return from function */
		return;
	}

	while (1) {
		SetPixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}

void TFT::LineV(int32_t X, int32_t Y1, int32_t Y2, uint16_t color) {

	//Клиппинг
	if (X < 0 || X >= LCD->TFT_WIDTH) return;
	if (Y1 > Y2) { int32_t t = Y1; Y1 = Y2; Y2 = t; }
	if (Y1 < 0) Y1 = 0;
	if (Y2 >= LCD->TFT_HEIGHT) Y2 = LCD->TFT_HEIGHT - 1;
	if (Y1 > Y2) return;

	int32_t i;

	if ((LCD->Bit) == 1) {
		if (color != 0) {
			for (i = Y1; i <= Y2; i++)
				LCD->buffer8[X + (i / 8) * LCD->TFT_WIDTH] |= 1 << (i % 8);
		} else {
			for (i = Y1; i <= Y2; i++)
				LCD->buffer8[X + (i / 8) * LCD->TFT_WIDTH] &= ~(1 << (i % 8));
		}
		return;
	}

	if ((LCD->Bit) == 4) {
		for (i = Y1; i <= Y2; i++)
			SetPixel(X, i, color);
		return;
	}

	if (LCD->Bit == 16) {
		for (i = Y1; i <= Y2; i++)
			LCD->buffer16[X + i * LCD->TFT_WIDTH] = color;
		return;
	}

	if (LCD->Bit == 8) {
		for (i = Y1; i <= Y2; i++)
			LCD->buffer8[X + i * LCD->TFT_WIDTH] = color;
	}

}

void TFT::LineH(int32_t Y, int32_t X1, int32_t X2, uint16_t color) {

	int32_t i;

	//Клиппинг
	if (Y < 0 || Y >= LCD->TFT_HEIGHT) return;
	if (X1 > X2) { int32_t t = X1; X1 = X2; X2 = t; }
	if (X1 < 0) X1 = 0;
	if (X2 >= LCD->TFT_WIDTH) X2 = LCD->TFT_WIDTH - 1;
	if (X1 > X2) return;

	if ((LCD->Bit) == 1) {
		if (color != 0) {
			for (i = X1; i <= X2; i++)
				LCD->buffer8[i + (Y / 8) * LCD->TFT_WIDTH] |= 1 << (Y % 8);
		} else {
			for (i = X1; i <= X2; i++)
				LCD->buffer8[i + (Y / 8) * LCD->TFT_WIDTH] &= ~(1 << (Y % 8));
		}
		return;
	}

	if ((LCD->Bit) == 16) {
		for (i = X1; i <= X2; i++)
		{
			LCD->buffer16[i + Y * LCD->TFT_WIDTH] = color;
		}
		return;
	}

	if ((LCD->Bit) == 4) {
		for (i = X1; i <= X2; i++)
		{
			if (i % 2 == 0) {
				LCD->buffer8[i / 2 + Y * ((LCD->TFT_WIDTH) / 2)] =
						(LCD->buffer8[i / 2 + Y * ((LCD->TFT_WIDTH) / 2)]
								& (0x0F)) | ((color & 0x0F) << 4);
			} else {
				LCD->buffer8[i / 2 + Y * ((LCD->TFT_WIDTH) / 2)] =
						(LCD->buffer8[i / 2 + Y * ((LCD->TFT_WIDTH) / 2)]
								& (0xF0)) | (color & 0x0F);
			}
		}
		return;
	}

	if ((LCD->Bit) == 8) {
		int YY = Y * LCD->TFT_WIDTH;
		for (i = X1; i <= X2; i++)
			LCD->buffer8[i + YY] = color;
		return;
	}

}

void TFT::LineH16(int32_t Y, int32_t X1, int32_t X2, uint16_t color) {

	int32_t i;

	//Клиппинг
	if (Y < 0 || Y >= LCD->TFT_HEIGHT) return;
	if (X1 > X2) { int32_t t = X1; X1 = X2; X2 = t; }
	if (X1 < 0) X1 = 0;
	if (X2 >= LCD->TFT_WIDTH) X2 = LCD->TFT_WIDTH - 1;
	if (X1 > X2) return;

	for (i = X1; i <= X2; i++)
	{
		LCD->buffer16[i + Y * LCD->TFT_WIDTH] = color;
	}

}

void TFT::LineHW(int32_t x, int32_t y, int32_t w, uint16_t color) {
	LineH(y, x, x + w - 1, color);
}

void TFT::LineHW16(int32_t x, int32_t y, int32_t w, uint16_t color) {
	LineH16(y, x, x + w - 1, color);
}

void TFT::LineMoveTo(int32_t x, int32_t y, uint16_t c) {
	Line(LineMoveX, LineMoveY, x, y, c);
	LineMoveX = x;
	LineMoveY = y;
}

void TFT::LineMoveXY(int32_t x, int32_t y) {
	LineMoveX = x;
	LineMoveY = y;
}

// ----- Rectagle ----
//Рамка прямоугольника размером w x h пикселей
void TFT::Rectangle(int32_t x, int32_t y, uint16_t w, uint16_t h, uint16_t c) {
	/* Check input parameters */
	if (w == 0 || h == 0) return;
	if (x >= LCD->TFT_WIDTH || y >= LCD->TFT_HEIGHT) return;

	/* Draw 4 lines */
	LineH(y, x, x + w - 1, c);
	LineH(y + h - 1, x, x + w - 1, c);

	LineV(x, y, y + h - 1, c);
	LineV(x + w - 1, y, y + h - 1, c);
}

//Заливка прямоугольника размером w x h пикселей
void TFT::RectangleFilled(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
		uint16_t c) {
	uint32_t i;

	/* Check input parameters */
	if (x >= LCD->TFT_WIDTH || y >= LCD->TFT_HEIGHT || w == 0 || h == 0) {
		/* Return error */
		return;
	}

	if ((LCD->Bit) == 8) {
		uint32_t X2 = (uint32_t)x + w - 1;
		if (X2 >= (uint32_t)LCD->TFT_WIDTH) X2 = LCD->TFT_WIDTH - 1;
		for (i = 0; i < h; i++) {
			uint32_t Y = y + i;
			if (Y >= (uint32_t)LCD->TFT_HEIGHT) break;
			int YY = Y * LCD->TFT_WIDTH;
			for (uint32_t ii = x; ii <= X2; ii++)
				LCD->buffer8[ii + YY] = c;
		}
		return;
	}

	for (i = 0; i < h; i++) {
		if ((uint32_t)(y + i) >= (uint32_t)LCD->TFT_HEIGHT) break;
		LineH(y + i, x, x + w - 1, c);
	}
}

//void uTFT_DrawRectangle(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t color)
//{
//  ST7735_HLine(X1,X2,Y1,color);
//  ST7735_HLine(X1,X2,Y2,color);
//  ST7735_VLine(X1,Y1,Y2,color);
//  ST7735_VLine(X2,Y1,Y2,color);
//}

//Инверсия прямоугольника размером w x h пикселей
void TFT::InvertRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	for (int32_t i = x; i < (int32_t)x + w; i++)
		for (int32_t ii = y; ii < (int32_t)y + h; ii++) {
			u16 p = GetPixel(i, ii);
			//Для 1 бита - логическая инверсия, для остальных - побитовая
			SetPixel(i, ii, (LCD->Bit == 1) ? (u16)!p : (u16)(~p & 0xFFFF));
		}
}

//Замена цветов в данном прямоугольнике размером w x h пикселей
void TFT::ChangeColorRectangle(int32_t x, int32_t y, uint32_t w, int32_t h,
		uint16_t sColor, uint16_t dColor) {
	for (int32_t i = x; i < x + (int32_t)w; i++)
		for (int32_t ii = y; ii < y + h; ii++) {
			if (GetPixel(i, ii) == sColor)
				SetPixel(i, ii, dColor);
		}
}

void TFT::Circle(int16_t x0, int16_t y0, int16_t r, uint16_t c) {

	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	SetPixel(x0, y0 + r, c);
	SetPixel(x0, y0 - r, c);
	SetPixel(x0 + r, y0, c);
	SetPixel(x0 - r, y0, c);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		SetPixel(x0 + x, y0 + y, c);
		SetPixel(x0 - x, y0 + y, c);
		SetPixel(x0 + x, y0 - y, c);
		SetPixel(x0 - x, y0 - y, c);

		SetPixel(x0 + y, y0 + x, c);
		SetPixel(x0 - y, y0 + x, c);
		SetPixel(x0 + y, y0 - x, c);
		SetPixel(x0 - y, y0 - x, c);
	}
}

void TFT::CircleFilled(int16_t x0, int16_t y0, int16_t r, uint16_t c) {

	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	SetPixel(x0, y0 + r, c);
	SetPixel(x0, y0 - r, c);
	SetPixel(x0 + r, y0, c);
	SetPixel(x0 - r, y0, c);
	Line(x0 - r, y0, x0 + r, y0, c);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		Line(x0 - x, y0 + y, x0 + x, y0 + y, c);
		Line(x0 + x, y0 - y, x0 - x, y0 - y, c);

		Line(x0 + y, y0 + x, x0 - y, y0 + x, c);
		Line(x0 + y, y0 - x, x0 - y, y0 - x, c);
	}
}

void TFT::Triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
		uint16_t x3, uint16_t y3, uint16_t color) {
	/* Draw lines */
	Line(x1, y1, x2, y2, color);
	Line(x2, y2, x3, y3, color);
	Line(x3, y3, x1, y1, color);
}

void TFT::TriangleFilled(int16_t x1, int16_t y1, int16_t x2, int16_t y2,
		int16_t x3, int16_t y3, uint16_t color) {

	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 =
			0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay) {
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		Line(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

//tft.Gradient_Vertical(0, 0, 239, 239, 0x26, 0x4d, 0x59, 0x43, 0x97, 0x8d);
//tft.Gradient_Vertical(0, 0, 239, 239, 0x68, 0x82, 0xa0, 0x2c, 0x69, 0x75);
//tft.Gradient_Vertical(0, 0, 239, 239, 0x26, 0x4d, 0x59, 0x2c, 0x69, 0x75);
//Вертикальный градиент
void TFT::Gradient_Vertical(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h,
  uint8_t otR, uint8_t otG ,uint8_t otB,
  uint8_t doR, uint8_t doG, uint8_t doB){

	if (w == 0 || h == 0) return;

	uint8_t a[3];
	uint16_t color;

	int32_t from[3];
	int32_t to[3];

	from[0] = otR; from[1] = otG; from[2] = otB;
	to[0]   = doR; to[1]   = doG; to[2]   = doB;

	//Линейная интерполяция целыми числами (без float)
	int32_t div = (h > 1) ? (int32_t)(h - 1) : 1;

	for (uint32_t row = 0; row < h; row++) {

		for (uint32_t z = 0; z < 3; z++)
			a[z] = (uint8_t)(from[z] + (((to[z] - from[z]) * (int32_t)row) / div));

		color = RGB565(a[0], a[1], a[2]);

		LineH(row + y0, x0, x0 + w - 1, color);
	}

}

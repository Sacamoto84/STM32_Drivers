#ifndef TFT_H_
#define TFT_H_

#include "TFT_config.h"
#include "TFT_define.h"

#include "Driver/TFT_Driver.h"

#define FontId0 (u8 *)(tft.getResAdressFontID(0))
#define FontId1 (u8 *)(tft.getResAdressFontID(1))
#define FontId2 (u8 *)(tft.getResAdressFontID(2))
#define FontId3 (u8 *)(tft.getResAdressFontID(3))

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define CREATE_SPRITE16(name, x , y) u16 name##_buffer16[x * y + 4];\
TFT_LCD_t name={x , y, SPRITE_RAM, 16, NULL , NULL, 0, &name##_buffer16[0],};

//Выделение памяти
//CREATE_SPRITE16(S1, 32 , 32);
//Sprite.init(&S1);
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//////////////////////////////////////////////

class TFT {
public:

	TFT_LCD_t *LCD;
	TFT_Driver driver;

	uTFT_t uTFT; //Для установка положения и цвета для текста

	int16_t _xPivot;   // TFT x pivot point coordinate for rotated Sprites
	int16_t _yPivot;   // TFT x pivot point coordinate for rotated Sprites

	////////////////////////////////////////////////////////////////////////////////////////////
	void init(TFT_LCD_t *_LCD) {
		LCD = _LCD;
		driver.init(_LCD);
	}

//┌────────────────────────────┬────────────────────┐
	void Fill   (u16 color); //│                    │
	void Fill1  (u16 color); //│                    │
	void Fill4  (u16 color); //│                    │
	void Fill8  (u16 color); //│                    │
	void Fill16 (u16 color); //│ Только для 16 бит  │
//└────────────────────────────┴────────────────────┘
//┌──────────────────────────────────────────────────────────┬──────────────────┐
	void SetPixel   (i32 x, i32 y, u16 color); //│ Для любого цвета │
	void SetPixel1  (i32 x, i32 y, u16 color); //│                  │
	void SetPixel4  (i32 x, i32 y, u16 color); //│                  │
	void SetPixel8  (i32 x, i32 y, u16 color); //│                  │
	void SetPixel16 (i32 x, i32 y, u16 color); //│                  │
//└──────────────────────────────────────────────────────────┴──────────────────┘
	u16 GetPixel   (i32 x, i32 y);
	u16 GetPixel1  (i32 x, i32 y);
	u16 GetPixel16 (i32 x, i32 y);

	void SetColorToPallete(u8 index, u16 color);



	//Установить цвет в ячейку палитры
	void setColorToPalete(int index, u16 c) {
		if (index > 255)
			return;
		LCD->palete[index] = c;
	}

	void Struct_to_Pallete_4b(TFT_LCD_t *LCD, uTFT_LCD_Pallete *in) {
		LCD->palete[0] = in->C0;
		LCD->palete[1] = in->C1;
		LCD->palete[2] = in->C2;
		LCD->palete[3] = in->C3;
		LCD->palete[4] = in->C4;
		LCD->palete[5] = in->C5;
		LCD->palete[6] = in->C6;
		LCD->palete[7] = in->C7;
		LCD->palete[8] = in->C8;
		LCD->palete[9] = in->C9;
		LCD->palete[10] = in->C10;
		LCD->palete[11] = in->C11;
		LCD->palete[12] = in->C12;
		LCD->palete[13] = in->C13;
		LCD->palete[14] = in->C14;
		LCD->palete[15] = in->C15;
	}

	// ----- Line.cpp ----
	void Line    (i32 x0, i32 y0, i32 x1, i32 y1, u16 c);
	void LineV   (i32 X, i32 Y1, i32 Y2, u16 color);
	void LineH   (i32 Y, i32 X1, i32 X2, u16 color);
	void LineH16 (i32 Y, i32 X1, i32 X2, u16 color);
	void LineHW  (i32 x, i32 y, i32 w, u16 color);
	void LineHW16(i32 x, i32 y, i32 w, u16 color);
	void LineMoveTo(i32 x, i32 y, u16 c);



	void LineMoveXY(i32 x, i32 y);

	// ----- Rectagle.cpp ----
	void Rectangle(i32 x, i32 y, u16 w, u16 h, u16 c);
	void RectangleFilled(u16 x, u16 y, u16 w, u16 h,
			u16 c);
	void InvertRectangle(u16 x, u16 y, u16 w, u16 h);
	//Замена цветов в данном прямоугольнике
	void ChangeColorRectangle(i32 x, i32 y, u32 w, i32 h, u16 sColor, u16 dColor);

	// ----- Circle.cpp ----
	void Circle(int16_t x0, int16_t y0, int16_t r, u16 c);
	void CircleFilled(int16_t x0, int16_t y0, int16_t r, u16 c);

	void Triangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 x3, u16 y3, u16 color);
	void TriangleFilled(i16 x1, i16 y1, i16 x2, i16 y2, i16 x3, i16 y3, u16 color);

	void Gradient_Vertical(u32 x0, u32 y0, u32 w, u32 h,
			u8 otR, u8 otG, u8 otB, u8 doR, u8 doG,	u8 doB);

	//Работает на все фонты//////////////////////////////////////////////////////////////
	void SetFontColor(u16 Color, u16 BColor) {
		uTFT.Color = Color;
		uTFT.BColor = BColor;
	}
	void SetColor(u16 Color) {
		uTFT.Color = Color;
	}
	void SetBColor(u16 BColor) {
		uTFT.BColor = BColor;
	}
	void GotoXY(i16 x, i16 y) {
		uTFT.CurrentX = x;
		uTFT.CurrentY = y;
	}

	u16 alphaBlend(u8 alpha, u16 fgc, u16 bgc);

//	//Альфа и транспарент
//Скопировать изображение с другого экрана
	void copy(TFT *_tft, int16_t x, int16_t y) {
		i32 x1, x2, y1, y2;
		x1 = constrain(x, 0, LCD->TFT_WIDTH);
		x2 = constrain(x + _tft->LCD->TFT_WIDTH, 0, LCD->TFT_WIDTH);
		y1 = constrain(y, 0, LCD->TFT_HEIGHT);
		y2 = constrain(y + _tft->LCD->TFT_HEIGHT, 0, LCD->TFT_HEIGHT);

		for (i32 i = x1; i < x2; i++) {
			for (int16_t ii = y1; ii < y2; ii++) {
				SetPixel(i, ii, _tft->GetPixel(i - x, ii - y));
			}
		}
	}

	void copyTr(TFT *_tft, int16_t x, int16_t y, u16 tr_color) {
		int16_t x1, x2, y1, y2;
		u16 color;
		x1 = constrain(x, 0, LCD->TFT_WIDTH);
		x2 = constrain(x + _tft->LCD->TFT_WIDTH, 0, LCD->TFT_WIDTH);
		y1 = constrain(y, 0, LCD->TFT_HEIGHT);
		y2 = constrain(y + _tft->LCD->TFT_HEIGHT, 0, LCD->TFT_HEIGHT);

		for (int16_t i = x1; i < x2; i++) {
			for (int16_t ii = y1; ii < y2; ii++) {
				color = _tft->GetPixel(i - x, ii - y);
				if ( color != tr_color)
				  SetPixel(i, ii, color);
			}
		}
	}


private:

	int16_t LineMoveX; //Для LineTo
	int16_t LineMoveY;

private:

};

#endif /* TFT_H_ */

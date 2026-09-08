#include "FontSmooth.h"

#include <stdbool.h>

#include "../TFT.h"

void Font_Smooth_unloadFont(void);
static int  Font_Smooth_loadMetrics(uint16_t gCount);

//////////////////////////////////////// SMOOTH ////////////////////////////////////////
// This is for the whole font
typedef struct {
	uint16_t gCount;     // Total number of characters
	uint16_t yAdvance;   // Line advance
	uint16_t spaceWidth; // Width of a space character
	int16_t ascent; // Height of top of 'd' above baseline, other characters may be taller
	int16_t descent; // Offset to bottom of 'p', other characters may have a larger descent
	uint16_t maxAscent;  // Maximum ascent found in font
	uint16_t maxDescent; // Maximum descent found in font
} fontMetrics;

/*
 The vlw font format does not appear to be documented anywhere, so some reverse
  engineering has been applied!

 Header of vlw file comprises 6 uint32_t parameters (24 bytes total):
 1. The gCount (number of character glyphs)
 2. A version number (0xB = 11 for the one I am using)
 3. The font size (in points, not pixels)
 4. Deprecated mboxY parameter (typically set to 0)
 5. Ascent in pixels from baseline to top of "d"         Подъем в пикселях от базовой линии до верхней части  "d"
 6. Descent in pixels from baseline to bottom of "p"     Спуск в пикселях от базовой линии до нижней части    "p"

 Next are gCount sets of values for each glyph, each set comprises 7 int32t parameters (28 bytes):
 1. Glyph Unicode stored as a 32 bit value
 2. Height of bitmap bounding box
 3. Width of bitmap bounding box
 4. gxAdvance for cursor (setWidth in Processing)
 5. dY = distance from cursor baseline to top of glyph bitmap (signed value +ve = up)
 6. dX = distance from cursor to left side of glyph bitmap (signed value -ve = left)
 7. padding value, typically 0

 The bitmaps start next at 24 + (28 * gCount) bytes from the start of the file.
 Each pixel is 1 byte, an 8 bit Alpha value which represents the transparency from
 0xFF foreground colour, 0x00 background. The sketch uses a linear interpolation
 between the foreground and background RGB component colours. e.g.
 pixelRed = ((fgRed * alpha) + (bgRed * (255 - alpha))/255
 To gain a performance advantage fixed point arithmetic is used with rounding and
 division by 256 (shift right 8 bits is faster).

 After the bitmaps is:
 1 byte for font name string length (excludes null)
 a zero terminated character string giving the font name
 1 byte for Postscript name string length
 a zero/one terminated character string giving the font name
 last byte is 0 for non-anti-aliased and 1 for anti-aliased (smoothed)


 Glyph bitmap example is:
 // Cursor coordinate positions for this and next character are marked by 'C'
 // C<------- gxAdvance ------->C  gxAdvance is how far to move cursor for next glyph cursor position
 // |                           |
 // |                           |   ascent is top of "d", descent is bottom of "p"
 // +-- gdX --+             ascent
 // |         +-- gWidth--+     |   gdX is offset to left edge of glyph bitmap
 // |   +     x@.........@x  +  |   gdX may be negative e.g. italic "y" tail extending to left of
 // |   |     @@.........@@  |  |   cursor position, plot top left corner of bitmap at (cursorX + gdX)
 // |   |     @@.........@@ gdY |   gWidth and gHeight are glyph bitmap dimensions
 // |   |     .@@@.....@@@@  |  |
 // | gHeight ....@@@@@..@@  +  +    <-- baseline
 // |   |     ...........@@     |
 // |   |     ...........@@     |   gdY is the offset to the top edge of the bitmap
 // |   |     .@@.......@@. descent plot top edge of bitmap at (cursorY + yAdvance - gdY)
 // |   +     x..@@@@@@@..x     |   x marks the corner pixels of the bitmap
 // |                           |
 // +---------------------------+   yAdvance is y delta for the next line, font size or (ascent + descent)
 //                                 some fonts can overlay in y direction so may need a user adjust value

 */

fontMetrics gFont = { 0, 0, 0, 0, 0, 0, 0 };

// These are for the metrics for each individual glyph (so we don't need to seek this in file and waste time)
uint16_t *gUnicode = NULL; //UTF-16 code, the codes are searched so do not need to be sequential
uint8_t *gHeight = NULL;   //cheight
uint8_t *gWidth = NULL;    //cwidth
uint8_t *gxAdvance = NULL; //setWidth
int16_t *gdY = NULL;       //topExtent
int8_t *gdX = NULL;       //leftExtent
uint32_t *gBitmap = NULL;   //file pointer to greyscale bitmap

static const unsigned char *p;
static const unsigned char *p_start; //Стартовый индекс массива для выделения глифа

uint32_t readInt32(void) {
	uint32_t res = 0;
	res  = ((uint32_t)*p++ << 24);
	res |= ((uint32_t)*p++ << 16);
	res |= ((uint32_t)*p++ << 8);
	res |=  (uint32_t)*p++;

	return  res;
}

void Font_Smooth_Load(const unsigned char *massiv) {
	Font_Smooth_unloadFont();

	p = massiv;
	p_start = massiv;
	gFont.gCount = (uint16_t) readInt32(); // glyph count in file
	readInt32(); // vlw encoder version - discard
	gFont.yAdvance = (uint16_t) readInt32(); // Font size in points, not pixels
	readInt32(); // discard
	gFont.ascent = (uint16_t) readInt32(); // top of "d"
	gFont.descent = (uint16_t) readInt32(); // bottom of "p""

	// These next gFont values might be updated when the Metrics are fetched
	gFont.maxAscent = gFont.ascent;   // Determined from metrics
	gFont.maxDescent = gFont.descent;  // Determined from metrics
	gFont.yAdvance = gFont.ascent + gFont.descent;
	gFont.spaceWidth = gFont.yAdvance / 4;  // Guess at space width

	if (!Font_Smooth_loadMetrics(gFont.gCount)) {
		//Не хватило памяти - выгружаем шрифт целиком
		Font_Smooth_unloadFont();
		gFont.gCount = 0;
	}

}

static int Font_Smooth_loadMetrics(uint16_t gCount) {
	uint32_t headerPtr = 24;
	uint32_t bitmapPtr = 24 + gCount * 28;

	gUnicode = (uint16_t*) malloc(gCount * 2); // Unicode 16 bit Basic Multilingual Plane (0-FFFF)
	gHeight = (uint8_t*) malloc(gCount);    // Height of glyph
	gWidth = (uint8_t*) malloc(gCount);    // Width of glyph
	gxAdvance = (uint8_t*) malloc(gCount);    // xAdvance - to move x cursor
	gdY = (int16_t*) malloc(gCount * 2); // offset from bitmap top edge from lowest point in any character
	gdX = (int8_t*) malloc(gCount); // offset for bitmap left edge relative to cursor X
	gBitmap = (uint32_t*) malloc(gCount * 4); // seek pointer to glyph bitmap in the file

	//Проверка выделения памяти
	if (gUnicode == NULL || gHeight == NULL || gWidth == NULL ||
		gxAdvance == NULL || gdY == NULL || gdX == NULL || gBitmap == NULL)
		return 0;

	uint16_t gNum = 0;

	while (gNum < gCount) {
		gUnicode[gNum]  = (uint16_t) readInt32(); // Unicode code point value
		gHeight[gNum]   = (uint8_t) readInt32(); // Height of glyph
		gWidth[gNum]    = (uint8_t) readInt32(); // Width of glyph
		gxAdvance[gNum] = (uint8_t) readInt32(); // xAdvance - to move x cursor
		gdY[gNum] = (int16_t) readInt32(); // y delta from baseline
		gdX[gNum] = (int8_t) readInt32(); // x delta from cursor
		readInt32(); // ignored

		// Different glyph sets have different descent values not always based on "p", so get maximum glyph descent
		if (((int16_t) gHeight[gNum] - (int16_t) gdY[gNum])
				> gFont.maxDescent) {
			// Avoid UTF coding values and characters that tend to give duff values
			if (((gUnicode[gNum] > 0x20) && (gUnicode[gNum] < 0xA0)
					&& (gUnicode[gNum] != 0x7F)) || (gUnicode[gNum] > 0xFF)) {
				gFont.maxDescent = gHeight[gNum] - gdY[gNum];
			}
		}

		gBitmap[gNum] = bitmapPtr;
		headerPtr += 28;
		bitmapPtr += gWidth[gNum] * gHeight[gNum];
		gNum++;
	}
	gFont.yAdvance = gFont.maxAscent + gFont.maxDescent;
	gFont.spaceWidth = (gFont.ascent + gFont.descent) * 2 / 7; // Guess at space width
	return 1;
}

void Font_Smooth_unloadFont(void) {
	if (gUnicode) {
		free(gUnicode);
		gUnicode = NULL;
	}

	if (gHeight) {
		free(gHeight);
		gHeight = NULL;
	}

	if (gWidth) {
		free(gWidth);
		gWidth = NULL;
	}

	if (gxAdvance) {
		free(gxAdvance);
		gxAdvance = NULL;
	}

	if (gdY) {
		free(gdY);
		gdY = NULL;
	}

	if (gdX) {
		free(gdX);
		gdX = NULL;
	}

	if (gBitmap) {
		free(gBitmap);
		gBitmap = NULL;
	}
}


bool Font_Smooth_getUnicodeIndex(uint16_t unicode, uint16_t *index) {

    if (unicode == 0xD081) {        //Ё (UTF-8 пара D0 81)
        unicode = 0x401;
    } else if (unicode == 0xD181) { //ё (UTF-8 пара D1 81)
        unicode = 0x451;
    } else {
		if (unicode == 0xA8) unicode = 0x401; //Ё (cp1251)
		else if (unicode == 0xB8) unicode = 0x451; //ё (cp1251)
		else if (unicode >= 0xD090) unicode -= 0xCC80;

		if ((unicode >= 192) && (unicode < 256))
			unicode = unicode + 1040 - 192;

		if (unicode > 0x44F) unicode -= 0xC0; //Глюк Эклипса
    }

	for (uint16_t i = 0; i < gFont.gCount; i++) {
		if (gUnicode[i] == unicode) {
			*index = i;
			return true;
		}
	}
	return false;
}

//TFT_HEIGHT
void Font_Smooth_drawGlyph(TFT * tft, uint16_t code) {

	const unsigned char *p_gBitmap; //Указатель на графические данные
	uint8_t pixel;
	uint8_t getColor;
	getColor = tft->uTFT.GetColor;

	if (code < 0x21) {
		if (code == 0x20) {
			tft->uTFT.CurrentX += gFont.spaceWidth;
			return;
		}

		if (code == '\n') {
			tft->uTFT.CurrentX = 0;
			tft->uTFT.CurrentY += gFont.yAdvance;
			if (tft->uTFT.CurrentY >= tft->LCD->TFT_HEIGHT)
				tft->uTFT.CurrentY = 0;
			return;
		}
	}

	uint16_t gNum = 0;
	bool found = Font_Smooth_getUnicodeIndex(code, &gNum); //Поиск символа в списке

	uint16_t fg = tft->uTFT.Color;
	uint16_t bg = tft->uTFT.BColor;

	if (found) //Если нашли символ
	{

//    if (textwrapX && (uTFT.CurrentX + gWidth[gNum] + gdX[gNum] > TFT_WIDTH))
//    {
//      uTFT.CurrentY += gFont.yAdvance;
//      uTFT.CurrentX = 0;
//    }
		//   if (textwrapY && ((uTFT.CurrentY + gFont.yAdvance) >= TFT_WIDTH)) uTFT.CurrentY = 0;

		if (tft->uTFT.CurrentX == 0)
			tft->uTFT.CurrentX -= gdX[gNum];

		p_gBitmap = p_start + gBitmap[gNum]; //Начало графических данных

		int16_t xs = 0;
		uint32_t dl = 0;

		int16_t cy = tft->uTFT.CurrentY + gFont.maxAscent - gdY[gNum];
		int16_t cx = tft->uTFT.CurrentX + gdX[gNum];

		for (int y = 0; y < gHeight[gNum]; y++) {
			for (int x = 0; x < gWidth[gNum]; x++) {
				pixel = *p_gBitmap++; //pbuffer[x]; //<//

				if (pixel) {
					if (pixel != 0xFF) {
						if (dl) {
							if (dl == 1)
							{

#ifdef USE_NOTSAVE_FONT
							//Прямая запись в buffer16 - обязателен клиппинг
							if (xs >= 0 && xs < tft->LCD->TFT_WIDTH &&
								y + cy >= 0 && y + cy < tft->LCD->TFT_HEIGHT)
								tft->LCD->buffer16[xs + (y + cy) * tft->LCD->TFT_WIDTH] = fg;
#else
							tft->SetPixel(xs, y + cy, fg);
#endif
							}
							else
								tft->LineHW16(xs, y + cy, dl, fg);

							dl = 0;
						}

						if (getColor)
						{
#ifdef USE_NOTSAVE_FONT
							if (x + cx >= 0 && x + cx < tft->LCD->TFT_WIDTH &&
								y + cy >= 0 && y + cy < tft->LCD->TFT_HEIGHT)
								bg = tft->LCD->buffer16[x + cx + (y + cy) * tft->LCD->TFT_WIDTH];
#else
							bg = tft->GetPixel(x + cx, y + cy);
#endif
						}


#ifdef USE_NOTSAVE_FONT
						if (x + cx >= 0 && x + cx < tft->LCD->TFT_WIDTH &&
							y + cy >= 0 && y + cy < tft->LCD->TFT_HEIGHT)
							tft->LCD->buffer16[x + cx + (y + cy) * tft->LCD->TFT_WIDTH] = tft->alphaBlend(pixel, fg, bg);
#else
						tft->SetPixel(x + cx, y + cy, tft->alphaBlend(pixel, fg, bg));
#endif

					} else {
						if (dl == 0)
							xs = x + cx;
						dl++;
					}

				} else {
					if (dl) {
						tft->LineHW16(xs, y + cy, dl, fg);
						dl = 0;
					}
				}
			}
			if (dl) {
				tft->LineHW16(xs, y + cy, dl, fg);
				dl = 0;
			}
		}
		tft->uTFT.CurrentX += gxAdvance[gNum];
	} else {
		// Not a Unicode in font so draw a rectangle and move on cursor
		tft->RectangleFilled(tft->uTFT.CurrentX,
				tft->uTFT.CurrentY + gFont.maxAscent - gFont.ascent,
				gFont.spaceWidth, gFont.ascent, fg);
		tft->uTFT.CurrentX += gFont.spaceWidth + 1;
	}

}

//Тест вывод на экран всего алфавита
void Font_Smooth_showFont(TFT * tft, uint32_t _timeDelay) {
	int16_t cursorX = tft->LCD->TFT_WIDTH; // Force start of new page to initialise cursor
	int16_t cursorY = tft->LCD->TFT_HEIGHT; // for the first character

	uint32_t timeDelay = 0;    // No delay before first page

	tft->Fill(tft->uTFT.BColor);

	for (uint16_t i = 0; i < gFont.gCount; i++) {
		// Check if this will need a new screen
		if (cursorX + gdX[i] + gWidth[i] >= tft->LCD->TFT_WIDTH) {
			cursorX = -gdX[i];

			cursorY += gFont.yAdvance;
			if (cursorY + gFont.maxAscent + gFont.descent >= tft->LCD->TFT_HEIGHT) {
				cursorX = -gdX[i];
				cursorY = 0;
				tft->driver.Update();
				HAL_Delay(timeDelay);
				timeDelay = _timeDelay;
				tft->Fill(tft->uTFT.BColor);
			}
		}

		tft->GotoXY(cursorX, cursorY);
		Font_Smooth_drawGlyph(tft, gUnicode[i]);
		cursorX += gxAdvance[i];
	}
	tft->driver.Update();
	HAL_Delay(timeDelay);
	tft->Fill(tft->uTFT.BColor);
}

void Font_Smooth_drawStr(TFT * tft, const char *str) {
	while (*str != 0) {
		uint16_t code = 0;

		if ((uint8_t)*str >= 0xD0){
			code  = ((uint8_t)*str++)<<8;
			if (*str == 0) break; //Обрыв 2-байтной пары UTF-8 в конце строки
			code |= (uint8_t)*str;}
		else
			code = (uint8_t)*str;

		Font_Smooth_drawGlyph(tft, code);
		str++;
	}

}

void Font_Smooth_drawStr(TFT * tft, int x, int y, const char *str) {
	tft->uTFT.CurrentX = x;
	tft->uTFT.CurrentY = y;
	while (*str != 0) {
		uint16_t code = 0;

		if ((uint8_t)*str >= 0xD0){
			code  = ((uint8_t)*str++)<<8;
			if (*str == 0) break; //Обрыв 2-байтной пары UTF-8 в конце строки
			code |= (uint8_t)*str;}
		else
			code = (uint8_t)*str;

		Font_Smooth_drawGlyph(tft, code);
		str++;
	}

}

void Font_Smooth_drawStr(TFT * tft, int x, int y, const char *str, uint16_t color) {
	tft->uTFT.CurrentX = x;
	tft->uTFT.CurrentY = y;
	tft->uTFT.Color = color;

	while (*str != 0) {
		uint16_t code = 0;

		if ((uint8_t)*str >= 0xD0){
			code  = ((uint8_t)*str++)<<8;
			if (*str == 0) break; //Обрыв 2-байтной пары UTF-8 в конце строки
			code |= (uint8_t)*str;}
		else
			code = (uint8_t)*str;

		Font_Smooth_drawGlyph(tft, code);
		str++;
	}
}

//Для поддержки русского шрифта
void Font_Smooth_drawStr1251(TFT * tft ,int x, int y, const char *str, uint16_t color) {
	tft->uTFT.CurrentX = x;
	tft->uTFT.CurrentY = y;
	tft->uTFT.Color = color;

	while (*str != 0) {
		uint16_t code = 0;
		code = (uint8_t)*str;
		Font_Smooth_drawGlyph(tft, code);
		str++;
	}
}





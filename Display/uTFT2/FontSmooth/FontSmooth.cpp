#include "FontSmooth.h"

#include <stdbool.h>

#include "../TFT.h"

//extern uTFT_t uTFT;

//макс. размер буфера для чтения
#define BUFSIZE 32

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned long long u64;

//u8 *CP1251 = cp1251_cyrillic_table;
//u16 *UTF8 = utf8_cyrillic_table;

//кириллическая таблица CP1251
//
const u8 cp1251_cyrillic_table[] = {

        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xA8, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB,
		0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
		0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,

		0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xB8, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA,
		0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6,
		0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF

};

//кириллическая таблица UTF8
//
const u16 utf8_cyrillic_table[] = {

        0xD090, 0xD091, 0xD092, 0xD093, 0xD094, 0xD095, 0xD081, 0xD096, 0xD097, 0xD098,
		0xD099, 0xD09A, 0xD09B, 0xD09C, 0xD09D, 0xD09E, 0xD09F, 0xD0A0, 0xD0A1,
		0xD0A2, 0xD0A3, 0xD0A4, 0xD0A5, 0xD0A6, 0xD0A7, 0xD0A8, 0xD0A9, 0xD0AA,
		0xD0AB, 0xD0AC, 0xD0AD, 0xD0AE, 0xD0AF,

		0xD0B0, 0xD0B1, 0xD0B2, 0xD0B3, 0xD0B4, 0xD0B5, 0xD191, 0xD0B6, 0xD0B7,
		0xD0B8, 0xD0B9, 0xD0BA, 0xD0BB, 0xD0BC, 0xD0BD, 0xD0BE, 0xD0BF, 0xD180,
		0xD181, 0xD182, 0xD183, 0xD184, 0xD185, 0xD186, 0xD187, 0xD188, 0xD189,
		0xD18A, 0xD18B, 0xD18C, 0xD18D, 0xD18E, 0xD18F,

};

void Font_Smooth_unloadFont(void);

//буффер для хранения данных
u8 utf8_buffer[BUFSIZE + BUFSIZE];

//общее число символов, предназначенных для записи в файл перед каждым очередным чтением исходного файла

u64 total_utf8_symbols = BUFSIZE;

//
u8* is_cyrillic(u8 code) {

	u8 i;
	u8 *pos = NULL;

	if (code != 0xA8 && code != 0xB8 && (code < 0xC0 || code > 0xFF))
		return pos = NULL;

	for (i = 0;	i < sizeof(cp1251_cyrillic_table) / sizeof(cp1251_cyrillic_table[0]);++i)

		if (code == cp1251_cyrillic_table[i])
		{
			pos = (u8*)&cp1251_cyrillic_table[i];//?
		}

	return pos;
}

void cp1251_to_utf8(u8 *cp1251_pointer, u8 *to_replace) {

	u8 i;

//так определяем индекс cp1251-кода
	for (i = 0; *cp1251_pointer != 0xC0; --cp1251_pointer, ++i)
		;

//вычленяем левый и правый байты utf8-кода, ибо буфер для записи имеет тип элементов u8

	//u8 utf8_left_code  = (utf8_cyrillic_table >> 8);
	//u8 utf8_right_code = (utf8_cyrillic_table & 0x00ff);

	//*to_replace = utf8_left_code;
	//*(to_replace + 1) = utf8_right_code;

}

void replace_array(u8 *array, int pos) {

	int i;

	for (i = total_utf8_symbols - 1; i >= pos; --i)
		*(array + i + 1) = *(array + i);

}
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

void Font_Smooth_loadMetrics(uint16_t gCount);
uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc);

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
	res  = (*p++ << 24);
	res |= (*p++ << 16);
	res |= (*p++ << 8);
	res |= (*p++);

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

	Font_Smooth_loadMetrics(gFont.gCount);

}

void Font_Smooth_loadMetrics(uint16_t gCount) {
	uint32_t headerPtr = 24;
	uint32_t bitmapPtr = 24 + gCount * 28;

	gUnicode = (uint16_t*) malloc(gCount * 2); // Unicode 16 bit Basic Multilingual Plane (0-FFFF)
	gHeight = (uint8_t*) malloc(gCount);    // Height of glyph
	gWidth = (uint8_t*) malloc(gCount);    // Width of glyph
	gxAdvance = (uint8_t*) malloc(gCount);    // xAdvance - to move x cursor
	gdY = (int16_t*) malloc(gCount * 2); // offset from bitmap top edge from lowest point in any character
	gdX = (int8_t*) malloc(gCount); // offset for bitmap left edge relative to cursor X
	gBitmap = (uint32_t*) malloc(gCount * 4); // seek pointer to glyph bitmap in the file

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

    if (unicode == 0xD081) unicode = 0x401; //Ё

	if (unicode >= 0xD090)
		unicode -= 0xCC80;


	if ((unicode >= 192) && (unicode < 256))
		unicode = unicode + 1040 - 192;

	if (unicode > 0x44F) unicode -= 0xC0; //Глюк Эклипса

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
			if (tft->uTFT.CurrentY >= tft->LCD->TFT_WIDTH)
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
							bg = tft->LCD->buffer16[x + cx + (y + cy) * tft->LCD->TFT_WIDTH];
#else
							bg = tft->GetPixel(x + cx, y + cy);
#endif
						}


#ifdef USE_NOTSAVE_FONT
						tft->LCD->buffer16[x + cx + (y + cy) * tft->LCD->TFT_WIDTH] = alphaBlend(pixel, fg, bg);
#else
						tft->SetPixel(x + cx, y + cy, alphaBlend(pixel, fg, bg));
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

uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc) {
	// For speed use fixed point maths and rounding to permit a power of 2 division
	uint16_t fgR = ((fgc >> 10) & 0x3E) + 1;
	uint16_t fgG = ((fgc >> 4) & 0x7E) + 1;
	uint16_t fgB = ((fgc << 1) & 0x3E) + 1;

	uint16_t bgR = ((bgc >> 10) & 0x3E) + 1;
	uint16_t bgG = ((bgc >> 4) & 0x7E) + 1;
	uint16_t bgB = ((bgc << 1) & 0x3E) + 1;

	// Shift right 1 to drop rounding bit and shift right 8 to divide by 256
	uint16_t r = (((fgR * alpha) + (bgR * (255 - alpha))) >> 9);
	uint16_t g = (((fgG * alpha) + (bgG * (255 - alpha))) >> 9);
	uint16_t b = (((fgB * alpha) + (bgB * (255 - alpha))) >> 9);

	// Combine RGB565 colours into 16 bits
	return (r << 11) | (g << 5) | (b << 0);
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

		if (*str >= 0xD0){
			code  = (*str++)<<8;
			code |= (*str);}
		else
			code = *str;

		Font_Smooth_drawGlyph(tft, code);
		str++;
		//uTFT.CurrentX +=  gFont.spaceWidth ;
		//cursorX + gdX[i] + gWidth[i]
		//cursorX + gdX[i] + gWidth[i]
	}

}

void Font_Smooth_drawStr(TFT * tft, int x, int y, const char *str) {
	tft->uTFT.CurrentX = x;
	tft->uTFT.CurrentY = y;
	while (*str != 0) {
		uint16_t code = 0;

		if (*str >= 0xD0){
			code  = (*str++)<<8;
			code |= (*str);}
		else
			code = *str;

		Font_Smooth_drawGlyph(tft, code);
		str++;
		//uTFT.CurrentX +=  gFont.spaceWidth ;
		//cursorX + gdX[i] + gWidth[i]
		//cursorX + gdX[i] + gWidth[i]
	}

}

void Font_Smooth_drawStr(TFT * tft, int x, int y, const char *str, uint16_t color) {
	tft->uTFT.CurrentX = x;
	tft->uTFT.CurrentY = y;
	tft->uTFT.Color = color;

	while (*str != 0) {
		uint16_t code = 0;

		if (*str >= 0xD0){
			code  = (*str++)<<8;
			code |= (*str);}
		else
			code = *str;

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
		code = *str;
		Font_Smooth_drawGlyph(tft, code);
		str++;
	}
}





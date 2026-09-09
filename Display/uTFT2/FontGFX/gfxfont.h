// Font structures for newer Adafruit_GFX (1.1 and later).
// Example fonts are included in 'Fonts' directory.
// To use a font in your Arduino sketch, #include the corresponding .h
// file and pass address of GFXfont struct to setFont().  Pass NULL to
// revert to 'classic' fixed-space bitmap font.

#ifndef _GFXFONT_H_
#define _GFXFONT_H_

#include "main.h"
#include "TFT.h"

#include "string.h"

#define PROGMEM

/// Font data stored PER GLYPH
typedef struct {
	uint16_t bitmapOffset; ///< Pointer into GFXfont->bitmap
	uint8_t width;         ///< Bitmap dimensions in pixels
	uint8_t height;        ///< Bitmap dimensions in pixels
	uint8_t xAdvance;      ///< Distance to advance cursor (x axis)
	int8_t xOffset;        ///< X dist from cursor pos to UL corner
	int8_t yOffset;        ///< Y dist from cursor pos to UL corner
} GFXglyph;

/// Data stored for FONT AS A WHOLE
typedef struct {
	uint8_t *bitmap;  ///< Glyph bitmaps, concatenated
	GFXglyph *glyph;  ///< Glyph array
	uint16_t first;   ///< ASCII extents (first char)
	uint16_t last;    ///< ASCII extents (last char)
	uint8_t yAdvance; ///< Newline distance (y axis)
} GFXfont;

class GFXFONT {
public:

	TFT *_tft;
	GFXfont *gfxFont;

	void set_delta_x(int8_t x) {
		delta_x = x;
	}

	void init(TFT *tft) {
		_tft = tft;
	}


	void Puts(char *str) {
		while (*str) {
			drawChar(*str);
			str++;

		}
	}

	void Puts(int x, int y, char *str) {
		_tft->uTFT.CurrentX = x;
		_tft->uTFT.CurrentY = y;

		while (*str) {
			drawChar(*str);
			str++;

		}
	}

	void Puts(int x, int y, char *str, int limit) {
		_tft->uTFT.CurrentX = x;
		_tft->uTFT.CurrentY = y;

		int count = 0;

		while (*str) {
			drawChar(*str);
			str++;
            count++;
            if(count >= limit )
            	break;
		}
	}

	void Puts(int x, int y, char *str, uint16_t color) {
		_tft->uTFT.CurrentX = x;
		_tft->uTFT.CurrentY = y;
		_tft->uTFT.Color = color;
		while (*str) {
			drawChar(*str);
			str++;

		}
	}

	void setFont(const GFXfont *f) {
		gfxFont = (GFXfont*) f;
	}

	void drawChar(unsigned char c, uint8_t size = 1) {

		if (c == '\r') return;
		if (c == '\n') {
			_tft->uTFT.CurrentX = 0;
			_tft->uTFT.CurrentY += gfxFont ? gfxFont->yAdvance : 16;
			return;
		}

		//Защита: шрифт не установлен или символ вне диапазона глифов
		//(иначе - чтение чужой памяти через glyph[])
		if (gfxFont == NULL || c < gfxFont->first || c > gfxFont->last) {
			//Продвигаем курсор на ширину пробела
			_tft->uTFT.CurrentX += gfxFont ? (gfxFont->yAdvance / 2) : 6;
			return;
		}

		int16_t x = _tft->uTFT.CurrentX;
		int16_t y = _tft->uTFT.CurrentY;
		uint16_t color = _tft->uTFT.Color;

		c -= gfxFont->first;

		GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c);
		uint8_t *bitmap = pgm_read_bitmap_ptr(gfxFont);

		uint16_t bo = glyph->bitmapOffset;

		uint8_t w = glyph->width, h = glyph->height;
		int8_t xo = glyph->xOffset, yo = glyph->yOffset;

		uint8_t xx, yy, bits = 0, bit = 0;
		int16_t xo16 = 0, yo16 = 0;

		if (size > 1) {
			xo16 = xo;
			yo16 = yo;
		}

		for (yy = 0; yy < h; yy++) {
			for (xx = 0; xx < w; xx++) {
				if (!(bit++ & 7)) {
					bits = bitmap[bo++];
				}
				if (bits & 0x80) {
					if (size == 1) {
						_tft->SetPixel(x + xo + xx, y + yo + yy, color);
					} else {

						_tft->RectangleFilled(x + (xo16 + xx) * size,
								y + (yo16 + yy) * size, size, size, color);
					}
				}
				bits <<= 1;
			}
		}

		//_tft->uTFT.CurrentX += glyph->width + delta_x;
		_tft->uTFT.CurrentX += glyph->xAdvance + delta_x;// +glyph->xOffset + delta_x;

	}



#define GFX_MAX_STRING 255
char target[GFX_MAX_STRING + 1] = "";

char *utf8rus2(char *source)
{
  int i = 0, j = 0;
  target[0] = '\0';
  if (source == NULL) return target;

  while (source[i] != '\0' && j < GFX_MAX_STRING) {
    uint8_t n = (uint8_t)source[i++];

    if (n >= 0x80) {
      if (n == 0xD0) {
        if (source[i] == '\0') break;
        uint8_t n2 = (uint8_t)source[i++];
        if (n2 == 0x81) { // Ё
          n = 0xC0;
        } else if (n2 >= 0x90 && n2 <= 0xBF) { // А..п
          n = (uint8_t)(n2 - 0x10);
        } else {
          n = n2;
        }
      } else if (n == 0xD1) {
        if (source[i] == '\0') break;
        uint8_t n2 = (uint8_t)source[i++];
        if (n2 == 0x91) { // ё
          n = 0xC1;
        } else if (n2 >= 0x80 && n2 <= 0x8F) { // р..я
          n = (uint8_t)(n2 + 0x30);
        } else {
          n = n2;
        }
      }
    }

    target[j++] = (char)n;
  }
  target[j] = '\0';
  return target;
}

private:
	inline GFXglyph* pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c) {
		return gfxFont->glyph + c;
	}

	inline uint8_t* pgm_read_bitmap_ptr(const GFXfont *gfxFont) {
		return gfxFont->bitmap;
	}

	int8_t delta_x = 0; //Добавочное растояние между символами

};

#endif // _GFXFONT_H_

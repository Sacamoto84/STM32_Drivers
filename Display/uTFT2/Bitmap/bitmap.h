#ifndef BITMAP_BITMAP_H_
#define BITMAP_BITMAP_H_

#include "TFT.h"
#include "TFT_color.h"

  // Bitmap
  typedef struct {
	uint16_t  W;
	uint16_t  H;
	uint32_t  bit;     //1,8,16,24,32
	uint32_t * data;
	uint32_t * palette;

	//Алиасы data (заполняются getResBitmapID, оставлены для совместимости
	//с кодом, который строил Bitmap вручную)
	unsigned short * steam16;
	uint32_t       * steam32;

  } Bitmap;

  extern void BitmapFromFlashBackground16bit(TFT * tft, Bitmap *bmp);

  extern void BitmapFromFlash16b(TFT * tft, int16_t X, int16_t Y, Bitmap *bmp);
  extern void BitmapFromFlash24b(TFT * tft, int16_t X, int16_t Y, Bitmap *bmp);
  extern void BitmapFromFlash32b(TFT * tft, int16_t X, int16_t Y, Bitmap *bmp);
  extern void BitmapFromFlash1b(TFT * tft, int16_t X, int16_t Y, Bitmap *bmp);

  //32 бит BMP с альфа каналом Сохранять как инвертированая альфа и свап  , customAlpha = 1.0 полная альфа
  extern void BitmapFromFlashAlpha(TFT * tft, int32_t x0, int32_t y0, Bitmap *bmp, float customAlpha);
  extern void BitmapFromFlashTransparent(TFT * tft, uint16_t X, uint16_t Y,	Bitmap bmp, uint16_t TrColor);

#if (FAT_FS)
  #include "fatfs.h"

  //Разбор 54-байтового заголовка BMP с проверками.
  //Возвращает 1 при успехе. clrUsed гарантированно <= 256.
  static inline int BMP_ParseHeader(const uint8_t *buf, UINT rd,
      uint32_t *offBits, uint32_t *width, uint32_t *height,
      uint16_t *bitCount, uint32_t *clrUsed)
  {
    uint32_t biSize, compression;
    if (rd != 54) return 0;
    if (buf[0] != 0x42 || buf[1] != 0x4D) return 0; //Сигнатура "BM"

    *offBits  = (uint32_t)buf[10] | ((uint32_t)buf[11] << 8) | ((uint32_t)buf[12] << 16) | ((uint32_t)buf[13] << 24);
    biSize    = (uint32_t)buf[14] | ((uint32_t)buf[15] << 8) | ((uint32_t)buf[16] << 16) | ((uint32_t)buf[17] << 24);
    *width    = (uint32_t)buf[18] | ((uint32_t)buf[19] << 8) | ((uint32_t)buf[20] << 16) | ((uint32_t)buf[21] << 24);
    *height   = (uint32_t)buf[22] | ((uint32_t)buf[23] << 8) | ((uint32_t)buf[24] << 16) | ((uint32_t)buf[25] << 24);
    *bitCount = (uint16_t)(buf[28] | (buf[29] << 8));
    compression = (uint32_t)buf[30] | ((uint32_t)buf[31] << 8) | ((uint32_t)buf[32] << 16) | ((uint32_t)buf[33] << 24);
    *clrUsed  = (uint32_t)buf[46] | ((uint32_t)buf[47] << 8) | ((uint32_t)buf[48] << 16) | ((uint32_t)buf[49] << 24);

    if (biSize != 40) return 0; //Только BITMAPINFOHEADER (V4/V5 не поддержаны)
    if (*width == 0 || *height == 0) return 0;
    if (*width > 32767 || *height > 32767) return 0;
    if (*offBits < 54) return 0;
    if (compression != 0) return 0; //Сжатые BMP (RLE, BITFIELDS) не поддержаны
    if (*clrUsed > 256) return 0;
    return 1;
  }

  //Чтение палитры BMP (BGRA после заголовка) с обнулением хвоста.
  //Возвращает 1 при успехе.
  static inline int BMP_ReadPalette(FIL *file, uint32_t count, uint16_t *pal)
  {
    uint32_t buf[256];
    UINT rd = 0;
    uint32_t i;

    if (count == 0 || count > 256) return 0;
    for (i = 0; i < 256; i++) pal[i] = 0;
    if (f_read(file, buf, count * 4, &rd) != FR_OK || rd != count * 4)
      return 0;
    for (i = 0; i < count; i++)
      pal[i] = RGB888_RGB565(buf[i]);
    return 1;
  }

  //Построчный вывод несжатого BMP (1/4/8/24 бита) из файла на экран.
  //Учитывает выравнивание строк до 4 байт (stride), читает ровно строку
  //за seek-ом. trValid != 0 - пропускать пиксели цвета trColor.
  //Возвращает 1 при успехе, 0 - файл не соответствует заголовку.
  static inline int BMP_DrawFile(TFT *tft, FIL *file,
      uint32_t offBits, uint32_t width, uint32_t height, uint16_t bitCount,
      uint32_t clrUsed, int32_t x0, int32_t y0,
      int trValid, uint16_t trColor, uint8_t *lineBuf, uint32_t lineBufSize)
  {
    uint16_t pal[256];
    uint32_t palBytes = 0; //0 = не палитровый формат
    uint32_t rowBytes;     //полезные байты строки
    uint32_t stride;       //шаг строки с выравниванием до 4 байт
    UINT rd = 0;

    if (bitCount == 24) {
      rowBytes = width * 3;
    } else if (bitCount == 8 || bitCount == 4 || bitCount == 1) {
      palBytes = (clrUsed != 0) ? clrUsed : (1u << bitCount);
      rowBytes = (width * bitCount + 7) / 8;
    } else {
      return 0; //Неподдержанная глубина цвета
    }

    stride = (rowBytes + 3) & ~3u;
    if (rowBytes == 0 || rowBytes > lineBufSize) return 0; //Слишком широкая картинка

    if (palBytes != 0) {
      //Палитра расположена сразу после 54-байтового заголовка
      if (f_lseek(file, 54) != FR_OK) return 0;
      if (BMP_ReadPalette(file, palBytes, pal) == 0) return 0;
    }

    for (uint32_t row = 0; row < height; row++) {
      //Положительная высота: строки хранятся снизу вверх
      int32_t y = (int32_t)height - 1 - (int32_t)row + y0;

      if (f_lseek(file, offBits + row * stride) != FR_OK) return 0;
      if (f_read(file, lineBuf, rowBytes, &rd) != FR_OK || rd != rowBytes)
        return 0; //Файл короче, чем заявляет заголовок

      for (uint32_t col = 0; col < width; col++) {
        uint16_t c;

        if (bitCount == 24) {
          c = RGB565(lineBuf[col*3+2], lineBuf[col*3+1], lineBuf[col*3]);
        } else if (bitCount == 8) {
          c = pal[lineBuf[col]];
        } else if (bitCount == 4) {
          uint8_t b = lineBuf[col >> 1];
          c = pal[(col & 1) ? (b & 0x0F) : (b >> 4)];
        } else { //1 бит
          uint8_t b = lineBuf[col >> 3];
          c = pal[(b >> (7 - (col & 7))) & 0x01];
        }

        if (trValid && c == trColor) continue;
        tft->SetPixel((int32_t)col + x0, y, c);
      }
    }
    return 1;
  }

  extern void BMPFromFile(TFT * tft, int32_t x0, int32_t y0, const char * Name);
  extern void BMPFromFileTransparent(TFT * tft, int32_t x0, int32_t y0, const char * Name, uint16_t tr_color);
  //32 бит BMP с альфа каналом
  extern List_Update_Particle BMPFromFile32b(TFT * tft, int32_t x0, int32_t y0, const char * Name, int offset, int swap);

#endif

#endif /* BITMAP_BITMAP_H_ */

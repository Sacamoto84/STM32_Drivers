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

	//unsigned char  * steam8;  //Убрать
	unsigned short * steam16; //Убрать
	uint32_t       * steam32; //Убрать

  } Bitmap;

  extern void BitmapFromFlash(TFT * tft, int16_t X, int16_t Y, Bitmap *bmp);
  extern void BitmapFromFlashBackground16bit(TFT * tft, Bitmap *bmp);

  extern void BitmapFromFlash16b(TFT * tft, int16_t X, int16_t Y, Bitmap *bmp);
  extern void BitmapFromFlash24b(TFT * tft, int16_t X, int16_t Y, Bitmap *bmp);
  extern void BitmapFromFlash32b(TFT * tft, int16_t X, int16_t Y, Bitmap *bmp);

  //32 бит BMP с альфа каналом Сохранять как инвертированая альфа и свап  , customAlpha = 1.0 полная альфа
  extern void BitmapFromFlashAlpha(TFT * tft, int32_t x0, int32_t y0, Bitmap *bmp, float customAlpha);
  extern void BitmapFromFlashTransparent(TFT * tft, uint16_t X, uint16_t Y,	Bitmap bmp, uint16_t TrColor);

#if (FAT_FS)
  #include "fatfs.h"

  #define WORD  uint16_t
  #define DWORD uint32_t
  #define LONG  uint64_t

  //структура BITMAPINFOHEADER
 typedef struct tagBITMAPINFOHEADER
 {
   uint16_t *   bfType;      //Сигнатура "BM"
   uint32_t *   bfSize;      //Размер файла
   uint32_t *   bfOffBits;   //Смещение изображения от начала файл
   uint32_t * biWidth;      //Ширина изображения, точки
   uint32_t *  biHeight;     //Высота изображения, точки
   uint16_t  *  biBitCount;  //Глубина цвета, бит на точку
   uint32_t   biCompression; //Тип компрессии (0 - несжатое изображение)
   uint32_t   biSizeImage;   //Размер изображения, байт
   uint32_t *  biClrUsed;    //Число используемых цветов (0 - максимально возможное для данной глубины цвета)
 } BITMAPINFOHEADER, *PBITMAPINFOHEADER;

  //Разбор 54-байтового заголовка BMP с проверками.
  //Возвращает 1 при успехе. clrUsed гарантированно <= 256.
  static inline int BMP_ParseHeader(const uint8_t *buf, UINT rd,
      uint32_t *offBits, uint32_t *width, uint32_t *height,
      uint16_t *bitCount, uint32_t *clrUsed)
  {
    if (rd != 54) return 0;
    if (buf[0] != 0x42 || buf[1] != 0x4D) return 0; //Сигнатура "BM"

    *offBits  = (uint32_t)buf[10] | ((uint32_t)buf[11] << 8) | ((uint32_t)buf[12] << 16) | ((uint32_t)buf[13] << 24);
    *width    = (uint32_t)buf[18] | ((uint32_t)buf[19] << 8) | ((uint32_t)buf[20] << 16) | ((uint32_t)buf[21] << 24);
    *height   = (uint32_t)buf[22] | ((uint32_t)buf[23] << 8) | ((uint32_t)buf[24] << 16) | ((uint32_t)buf[25] << 24);
    *bitCount = (uint16_t)(buf[28] | (buf[29] << 8));
    *clrUsed  = (uint32_t)buf[46] | ((uint32_t)buf[47] << 8) | ((uint32_t)buf[48] << 16) | ((uint32_t)buf[49] << 24);

    if (*width == 0 || *height == 0) return 0;
    if (*offBits < 54) return 0;
    if (*clrUsed > 256) return 0;
    return 1;
  }

  extern void BMPFromFile(TFT * tft, int32_t x0, int32_t y0, char * Name);
  extern void BMPFromFileTransparent(TFT * tft, int32_t x0, int32_t y0, char * Name, uint16_t tr_color);
  //32 бит BMP с альфа каналом
  extern List_Update_Particle BMPFromFile32b(TFT * tft, int32_t x0, int32_t y0, char * Name, int offset, int swap);

#endif

#endif /* BITMAP_BITMAP_H_ */

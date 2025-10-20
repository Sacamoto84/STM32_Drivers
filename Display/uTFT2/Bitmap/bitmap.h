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
  extern void BitmapFromFlashBackground16bit(TFT * tft, Bitmap *bmp);
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

  //BITMAPINFOHEADER bmp_header;

  extern void BMPFromFile(TFT * tft, int32_t x0, int32_t y0, char * Name);
  extern void BMPFromFileTransparent(TFT * tft, int32_t x0, int32_t y0, char * Name, uint16_t tr_color);
  //32 бит BMP с альфа каналом
  extern List_Update_Particle BMPFromFile32b(TFT * tft, int32_t x0, int32_t y0, char * Name, int offset, int swap);

#endif

#endif /* BITMAP_BITMAP_H_ */

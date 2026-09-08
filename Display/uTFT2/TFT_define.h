/*
 * TFT_define.h
 *
 *  Created on: 1 янв. 2023 г.
 *      Author: Ivan
 */

#ifndef TFT_DEFINE_H_
#define TFT_DEFINE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#include "TFT_config.h"

#define ABS(x)   ((x) > 0 ? (x) : -(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#define  u8  uint8_t
#define  u16 uint16_t
#define  u32 uint32_t
#define  i8  int8_t
#define  i16 int16_t
#define  i32 int32_t


typedef struct {
	int16_t  CurrentX;
	int16_t  CurrentY;
	uint16_t Color;
	uint16_t BColor;
	uint8_t  Inverted;
	uint8_t  GetColor; //1-Нужно читать цвет фона, 0-Использовать цвет BColor
} uTFT_t;

typedef struct {
	int16_t x0;
	int16_t y0;
	int16_t x1;
	int16_t y1;
	int16_t W;
	int16_t H;
} List_Update_Particle;

//_MADCTL
typedef struct {
	uint8_t MY;
	uint8_t MX;
	uint8_t MV;
	uint8_t ML;
	uint8_t RGB;
	uint8_t MH;
} _MADCTL;

//_ROTATE_DELTA
typedef struct {
	uint16_t W_0;
	uint16_t H_0;
	uint8_t  DX_0;
	uint8_t  DY_0;
	uint16_t W_90;
	uint16_t H_90;
	uint8_t  DX_90;
	uint8_t  DY_90;
	uint16_t W_180;
	uint16_t H_180;
	uint8_t  DX_180;
	uint8_t  DY_180;
	uint16_t W_270;
	uint16_t H_270;
	uint8_t  DX_270;
	uint8_t  DY_270;
} _ROTATE_DELTA;

//Совместимость: старое имя макроса содержало кириллическую "С".
//Новое каноническое имя - TFT_USE_I2C (латиница).
#if defined(TFT_USE_I2С)
  #define TFT_USE_I2C
#endif

//LCD_DRIVER
typedef enum {
	ST7735 = 0, /* (0) */
	ST7789,     /* (1) */
	ILI9225,    /* (2) */
	SSD1306,    /* (3) */
	PCD8544,    /* (4) */
	ST7735S,
	LCD_USB,
	SPRITE_RAM
} LCD_DRIVER;

//TFT_LCD_t
typedef struct {
	int32_t TFT_WIDTH;       //Ширина экрана в пикселях
	int32_t TFT_HEIGHT;      //Высота экрана в пикселях
	LCD_DRIVER LCD_Driver; // 0-ST7735, 1-ST7789, 2-ILI9225 3-SSD1306 4-PCD8544
	uint8_t Bit;             //Битность цвета: 16 8 4 1

#ifdef TFT_USE_SPI
	  SPI_HandleTypeDef *hspi;
    #else
	uint32_t *notUse;
#endif

#ifdef TFT_USE_I2C
	I2C_HandleTypeDef *hi2c;
#else
	  uint32_t * notUse2;
    #endif

	uint8_t I2C_Adress;

	uint16_t *buffer16;        //Буфер кадра для 16 бит
	uint8_t *buffer8;         //Буфер кадра для 8/4/1 бит
	uint16_t *palete;          //Палитра для 4/8 бит

	uint16_t dx;               //Смещение активной области по X
	uint16_t dy;               //Смещение активной области по Y
	GPIO_TypeDef *GPIO_CS;
	uint16_t GPIO_Pin_CS;
	GPIO_TypeDef *GPIO_DC;
	uint16_t GPIO_Pin_DC;
	GPIO_TypeDef *GPIO_RESET;
	uint16_t GPIO_Pin_RESET;
	_MADCTL MADCTL;
	_ROTATE_DELTA ROTATE_DELTA; //

} TFT_LCD_t;

//uTFT_LCD_Pallete
typedef struct {
	uint16_t C0;
	uint16_t C1;
	uint16_t C2;
	uint16_t C3;
	uint16_t C4;
	uint16_t C5;
	uint16_t C6;
	uint16_t C7;
	uint16_t C8;
	uint16_t C9;
	uint16_t C10;
	uint16_t C11;
	uint16_t C12;
	uint16_t C13;
	uint16_t C14;
	uint16_t C15;
} uTFT_LCD_Pallete;

#ifdef __cplusplus
}
#endif

#endif /* TFT_DEFINE_H_ */

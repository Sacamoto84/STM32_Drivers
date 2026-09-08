#ifndef FONTCLASSIC_FONTCLASSIC_H_
#define FONTCLASSIC_FONTCLASSIC_H_

#include "main.h"

#include "../TFT.h"

#include "stdlib.h"
#include "string.h"

// Classic
typedef struct {
	uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */ //ENG
} FontDef_t;

typedef struct {
	uint16_t Length;
	uint16_t Height;
} FONTS_SIZE_t;

extern FontDef_t Font_7x10;
extern FontDef_t Font_11x18;
extern FontDef_t Font_16x26;

//extern char   FontClassicPutc(TFT * tft, char ch, FontDef_t* Font, uint8_t NoBack );
//extern char   FontClassicPuts(TFT * tft, char * str, FontDef_t* Font, uint8_t NoBack);
//extern char * FontClassicGetStringSize(char * str, FONTS_SIZE_t* SizeStruct, FontDef_t* Font);

/**
  * @brief  Вывод символа
  * @param  ch символ.
  * @param  Font указатель на структуру
  * @param  NoBack true если не нужен задний фон за символом, по умолчанию 0
  * @retval ch status
  */
static inline char FontClassicPutc(TFT * tft, char ch, FontDef_t* Font, uint8_t NoBack ) {
	uint32_t i, b, j;

	//Символы до пробела не входят в таблицу данных шрифта (индекс < 0),
	//127 (DEL) - тоже: в таблице 95 глифов (32..126), иначе чтение за границей
	if ((uint8_t)ch < 32 || (uint8_t)ch > 126) {
		tft->uTFT.CurrentX += Font->FontWidth - 1;
		return ch;
	}

	if (NoBack)
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				tft->SetPixel(tft->uTFT.CurrentX + j, (tft->uTFT.CurrentY + i), tft->uTFT.Color);}
		}
	}
	else
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				tft->SetPixel(tft->uTFT.CurrentX + j, (tft->uTFT.CurrentY + i), tft->uTFT.Color);}
			else {
				tft->SetPixel(tft->uTFT.CurrentX + j, (tft->uTFT.CurrentY + i), tft->uTFT.BColor);}
		}
	}

	/* Increase pointer */
	tft->uTFT.CurrentX += Font->FontWidth-1;

	/* Return character written */
	return ch;
}

/**
  * @brief  Вывод строки
  * @param  htim TIM handle.
  * @param  sMasterConfig pointer to a TIM_MasterConfigTypeDef structure that
  *         contains the selected trigger output (TRGO) and the Master/Slave
  *         mode.
  * @retval HAL status
  */
static inline char FontClassicPuts(TFT * tft, char* str, FontDef_t* Font, uint8_t NoBack) {
	while (*str) {
		if (FontClassicPutc(tft, *str, Font, NoBack) != *str) {
			return *str;
		}
		str++;
	}
	/* Everything OK, zero should be returned */
	return *str;
}

// Длина строки
static inline char* FontClassicGetStringSize(char* str, FONTS_SIZE_t* SizeStruct, FontDef_t* Font) {
	/* Fill settings */
	SizeStruct->Height = Font->FontHeight;
	SizeStruct->Length = Font->FontWidth * strlen(str);

	/* Return pointer */
	return str;
}




















#endif /* FONTCLASSIC_FONTCLASSIC_H_ */

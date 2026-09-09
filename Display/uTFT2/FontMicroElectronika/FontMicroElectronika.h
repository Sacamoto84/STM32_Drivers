/*
 * FontMicroElectronika.h
 *
 *  Created on: 1 янв. 2023 г.
 *      Author: Ivan
 */

#ifndef FONTMICROELECTRONIKA_FONTMICROELECTRONIKA_H_
#define FONTMICROELECTRONIKA_FONTMICROELECTRONIKA_H_

#include "main.h"

#include "stdlib.h"
#include "string.h"

#include "TFT.h"

///////////////////////////////////////////////////////////////////
//  Microelectronika
typedef struct {
	uint8_t FontWidthEng; /*!< Font width in pixels */
	uint8_t FontHeightEng; /*!< Font height in pixels */
	const uint8_t *dataEng; /*!< Pointer to data font data array */
	uint8_t FontWidthRus; /*!< Font width in pixels */
	uint8_t FontHeightRus;
	const uint8_t *dataRus; /*!< Pointer to data font data array */
} FontDefMicroElectronika_t;

// Устновна фонта
extern void FontMicroSetFont(FontDefMicroElectronika_t *uFont);
// Вывод символа
extern void FontMicroPutc(TFT * tft, uint8_t ch, uint8_t dx = 1, uint8_t transparrent = 0);
extern void FontMicroPuts(TFT * tft, char *str, uint8_t dx = 1, uint8_t transparrent = 0);
//Определение длинны строки в пикселях по типу текущего шрифта
uint16_t FontMicroFindLenStr(char *str, FontDefMicroElectronika_t *uFont, uint8_t dx = 1);

extern FontDefMicroElectronika_t Unispace_10x19;
extern FontDefMicroElectronika_t Wingdings2_31x26;
extern FontDefMicroElectronika_t Courier_12x18; //������� �����
extern FontDefMicroElectronika_t Fewture_12x12Eng;
extern FontDefMicroElectronika_t Arial_Narrow9x15;
extern FontDefMicroElectronika_t Verdana39x32;
extern FontDefMicroElectronika_t MicrosoftYaHeiUI24_41x45;
extern FontDefMicroElectronika_t MicrosoftYaHeiUI12_20x22;
extern FontDefMicroElectronika_t Microsoft_YaHeiUI16_27x30;



#endif /* FONTMICROELECTRONIKA_FONTMICROELECTRONIKA_H_ */

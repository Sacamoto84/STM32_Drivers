#include "FontMicroElectronika.h"

FontDefMicroElectronika_t Font;

// Устновна фонта
void FontMicroSetFont(FontDefMicroElectronika_t * uFont) {

	Font.FontHeightEng = uFont->FontHeightEng;
	Font.FontWidthEng = uFont->FontWidthEng;
	Font.dataEng = uFont->dataEng;

	Font.FontHeightRus = uFont->FontHeightRus;
	Font.FontWidthRus = uFont->FontWidthRus;
	Font.dataRus = uFont->dataRus;

}

// Вывод символа
void FontMicroPutc(TFT * tft, uint8_t ch, uint8_t dx, uint8_t transparrent) {
	int32_t i, j, index;
	const uint8_t *peng = &Font.dataEng[0];
	const uint8_t *prus = &Font.dataRus[0];
	const uint8_t *p;
	const uint8_t *offset;

	int16_t temp;
	int16_t Height;
	uint8_t maxWidth;

	//Символы до пробела не входят в таблицы шрифта (отрицательный индекс)
	if (ch < 32) {
		return;
	}

	if (ch < 0x80) //ENG (таблица содержит глифы 32..127)
			{

		if (Font.dataEng == NULL)
			return;
		p = peng;
		p += (ch - 32) * ((Font.FontHeightEng) / 8 + 1) * Font.FontWidthEng
				+ (ch - 32);
		offset = p;
		p++;
		Height = Font.FontHeightEng;
		maxWidth = Font.FontWidthEng;

	} else if (ch >= 0xC0) //RUS
	{
		if (Font.dataRus == NULL)
			return;
		p = prus;
		p += (ch - 0xC0) * ((Font.FontHeightRus) / 8 + 1) * Font.FontWidthRus
				+ (ch - 0xC0);
		offset = p;
		p++;
		Height = Font.FontHeightRus;
		maxWidth = Font.FontWidthRus;
	} else {
		//0x80..0xBF: спецсимволы cp1251, глифов в шрифте нет -
		//раньше здесь читали за границей ENG-таблицы
		tft->uTFT.CurrentX += 1 + dx;
		return;
	}

	//Защита от битых данных: ширина глифа не больше заявленной ширины шрифта
	if (*offset > maxWidth) {
		tft->uTFT.CurrentX += dx;
		return;
	}

	for (i = 0; i < *offset; i++) {
		for (j = 0; j <= Height / 8; j++) {
			for (index = 0; index < 8; index++) {
				temp = *p;
				if ((temp >> index) & 0x01) {
					tft->SetPixel(tft->uTFT.CurrentX + i, (tft->uTFT.CurrentY + j * 8 + index),
							tft->uTFT.Color);
				} else {
					if (transparrent == 0)
						tft->SetPixel(tft->uTFT.CurrentX + i,
								(tft->uTFT.CurrentY + j * 8 + index), tft->uTFT.BColor);
				}
			}
			p++;
		}
	}

	tft->uTFT.CurrentX += *offset + dx;
}

void FontMicroPuts(TFT * tft, char *str, uint8_t dx, uint8_t transparrent) {
	while (*str) {
		FontMicroPutc(tft, *str, dx, transparrent);
		str++;
	}
}

//Определение длинны строки в пикселях по типу текущего шрифта
uint16_t FontMicroFindLenStr(char *str, FontDefMicroElectronika_t * uFont)
{
	const uint8_t *p;

	uint16_t lenSum = 0;
	uint32_t lengs = strlen(str); //Количество символов (без обрезки на 255)

	if (uFont->dataEng == NULL)
		return 0;

	//Перебираем каждый символ
	for (uint32_t i = 0; i < lengs; i++) {
		uint8_t ch = (uint8_t)str[i];

		//Символ вне таблиц шрифта - ширина одного столбца
		if (ch < 32) {
			lenSum += 1;
			continue;
		}

		if (ch < 0x80) { //ENG
			p = uFont->dataEng;
			p += (ch - 32) * ((uFont->FontHeightEng) / 8 + 1)
					* uFont->FontWidthEng + (ch - 32);
		} else if (ch >= 0xC0) { //RUS
			if (uFont->dataRus == NULL) {
				lenSum += 1;
				continue;
			}
			p = uFont->dataRus;
			p += (ch - 0xC0) * ((uFont->FontHeightRus) / 8 + 1)
					* uFont->FontWidthRus + (ch - 0xC0);
		} else {
			//0x80..0xBF: спецсимволы cp1251, глифов в шрифте нет
			lenSum += 1;
			continue;
		}

		lenSum += *p + 1;

	}
	if (lenSum) lenSum -= 1;

	return lenSum;
}

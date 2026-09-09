#ifndef TFT_CONVERT_H_
#define TFT_CONVERT_H_

#include <stdint.h>
#include <string.h>

// Конвертирование строки из DOS(866) в Win1251
static inline void ConvertStringDosTo1251 ( char *str )
{
	static const uint8_t table[128] = {
			0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,	// 00 - 0F
			0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, // 10 - 1F
			0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, // 20 - 2F
			0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, // 30 - 3F
			0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, // 40 - 4F
			0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, // 50 - 5F
			0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, // 60 - 6F
			0xA8, 0xB8, 0xAA, 0xBA, 0xAF, 0xDF, 0xA1, 0xA2, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF  // 70 - 7F
	};

	for ( int i = 0; str[i] != '\0'; i++ )
		if ( (uint8_t)str [ i ] > 127 )
			str [ i ] = table [ (uint8_t)str [ i ] - 128 ];
} // ConvertStringDosTo1251

// Конвертирование строки из Win1251 в DOS(866)
static inline void ConvertString1251ToDos ( char *str )
{
	static const uint8_t table[128] = {
			0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, // 00 - 0F
			0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, // 10 - 1F
			0xD0, 0xF6, 0xF7, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xF0, 0xD9, 0xF2, 0xDB, 0xDC, 0xDD, 0xDE, 0xF4, // 20 - 2F
			0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF1, 0xF9, 0xF3, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, // 30 - 3F
			0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, // 40 - 4F
			0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0xF5, // 50 - 5F
			0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, // 60 - 6F
			0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF  // 70 - 7F
	};

	for ( int i = 0; str[i] != '\0'; i++ ){
		if ( (uint8_t)str [ i ] > 127 )
			str [ i ] = table [ (uint8_t)str [ i ] - 128 ];}

} // ConvertString1251ToDos

//Определение ниже (после таблицы g_letters)
static uint16_t C1251toUTF8(char c);

// Конвертирование строки из Win1251 в UTF8
static inline void ConvertString1251ToUTF8 ( char *in_str, char *out_str )
{
	uint16_t charUTF8;
    char * pUTF8 = out_str;

	for ( uint16_t i = 0; in_str[i] != '\0'; i++ ){

		if ((uint8_t)in_str [ i ] <= 127)
		{
			*pUTF8++ = in_str[i];
		}
		else
		{
			charUTF8 = C1251toUTF8(in_str[i]);
			if (charUTF8 > 0xFF) {
				*pUTF8++ = (char)(charUTF8 >> 8);
				*pUTF8++ = (char)(charUTF8 & 0xFF);
			} else {
				*pUTF8++ = (char)charUTF8;
			}
		}
	}
	*pUTF8 = 0;
}

typedef struct ConvLetter {
        char    win1251;
        int     unicode;
} Letter;

//const: таблица только читается, иначе копия уходит в RAM каждого TU
static const Letter g_letters[] = {
        {0x82, 0x201A}, // SINGLE LOW-9 QUOTATION MARK
        {0x83, 0x0453}, // CYRILLIC SMALL LETTER GJE
        {0x84, 0x201E}, // DOUBLE LOW-9 QUOTATION MARK
        {0x85, 0x2026}, // HORIZONTAL ELLIPSIS
        {0x86, 0x2020}, // DAGGER
        {0x87, 0x2021}, // DOUBLE DAGGER
        {0x88, 0x20AC}, // EURO SIGN
        {0x89, 0x2030}, // PER MILLE SIGN
        {0x8A, 0x0409}, // CYRILLIC CAPITAL LETTER LJE
        {0x8B, 0x2039}, // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
        {0x8C, 0x040A}, // CYRILLIC CAPITAL LETTER NJE
        {0x8D, 0x040C}, // CYRILLIC CAPITAL LETTER KJE
        {0x8E, 0x040B}, // CYRILLIC CAPITAL LETTER TSHE
        {0x8F, 0x040F}, // CYRILLIC CAPITAL LETTER DZHE
        {0x90, 0x0452}, // CYRILLIC SMALL LETTER DJE
        {0x91, 0x2018}, // LEFT SINGLE QUOTATION MARK
        {0x92, 0x2019}, // RIGHT SINGLE QUOTATION MARK
        {0x93, 0x201C}, // LEFT DOUBLE QUOTATION MARK
        {0x94, 0x201D}, // RIGHT DOUBLE QUOTATION MARK
        {0x95, 0x2022}, // BULLET
        {0x96, 0x2013}, // EN DASH
        {0x97, 0x2014}, // EM DASH
        {0x99, 0x2122}, // TRADE MARK SIGN
        {0x9A, 0x0459}, // CYRILLIC SMALL LETTER LJE
        {0x9B, 0x203A}, // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
        {0x9C, 0x045A}, // CYRILLIC SMALL LETTER NJE
        {0x9D, 0x045C}, // CYRILLIC SMALL LETTER KJE
        {0x9E, 0x045B}, // CYRILLIC SMALL LETTER TSHE
        {0x9F, 0x045F}, // CYRILLIC SMALL LETTER DZHE
        {0xA0, 0x00A0}, // NO-BREAK SPACE
        {0xA1, 0x040E}, // CYRILLIC CAPITAL LETTER SHORT U
        {0xA2, 0x045E}, // CYRILLIC SMALL LETTER SHORT U
        {0xA3, 0x0408}, // CYRILLIC CAPITAL LETTER JE
        {0xA4, 0x00A4}, // CURRENCY SIGN
        {0xA5, 0x0490}, // CYRILLIC CAPITAL LETTER GHE WITH UPTURN
        {0xA6, 0x00A6}, // BROKEN BAR
        {0xA7, 0x00A7}, // SECTION SIGN
        {0xA8, 0x0401}, // CYRILLIC CAPITAL LETTER IO
        {0xA9, 0x00A9}, // COPYRIGHT SIGN
        {0xAA, 0x0404}, // CYRILLIC CAPITAL LETTER UKRAINIAN IE
        {0xAB, 0x00AB}, // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
        {0xAC, 0x00AC}, // NOT SIGN
        {0xAD, 0x00AD}, // SOFT HYPHEN
        {0xAE, 0x00AE}, // REGISTERED SIGN
        {0xAF, 0x0407}, // CYRILLIC CAPITAL LETTER YI
        {0xB0, 0x00B0}, // DEGREE SIGN
        {0xB1, 0x00B1}, // PLUS-MINUS SIGN
        {0xB2, 0x0406}, // CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
        {0xB3, 0x0456}, // CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
        {0xB4, 0x0491}, // CYRILLIC SMALL LETTER GHE WITH UPTURN
        {0xB5, 0x00B5}, // MICRO SIGN
        {0xB6, 0x00B6}, // PILCROW SIGN
        {0xB7, 0x00B7}, // MIDDLE DOT
        {0xB8, 0x0451}, // CYRILLIC SMALL LETTER IO
        {0xB9, 0x2116}, // NUMERO SIGN
        {0xBA, 0x0454}, // CYRILLIC SMALL LETTER UKRAINIAN IE
        {0xBB, 0x00BB}, // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
        {0xBC, 0x0458}, // CYRILLIC SMALL LETTER JE
        {0xBD, 0x0405}, // CYRILLIC CAPITAL LETTER DZE
        {0xBE, 0x0455}, // CYRILLIC SMALL LETTER DZE
        {0xBF, 0x0457} // CYRILLIC SMALL LETTER YI
};

// Конвертирование символа Win1251 в упакованную пару байтов UTF-8.
// Возвращает (first_byte << 8) | second_byte.
static uint16_t C1251toUTF8(char c){
	uint8_t u = (uint8_t)c;

	if (u == 0xA8) return 0xD081; //Ё
	if (u == 0xB8) return 0xD191; //ё

	if (u >= 240) //р..я (0xF0-0xFF)
	{
		return 0xD100 + 0x0080 + (u - 240);
	}
	if (u >= 192) //А..Я, а..п (0xC0-0xEF)
	{
		return 0xD000 + 0x0090 + (u - 192);
	}

	//0x80-0xBF: спецсимволы cp1251 через таблицу g_letters
	{
		uint32_t count = sizeof(g_letters) / sizeof(g_letters[0]);
		for (uint32_t k = 0; k < count; k++) {
			if ((uint8_t)g_letters[k].win1251 == u) {
				uint32_t uni = (uint32_t)g_letters[k].unicode;
				if (uni < 0x800) {
					//Кодируется двумя байтами UTF-8
					return (uint16_t)(((0xC0 | (uni >> 6)) << 8) | (0x80 | (uni & 0x3F)));
				}
				//Требуется 3 байта UTF-8 - не влезает в упаковку,
				//оставляем символ как есть (как раньше)
				break;
			}
		}
	}

	return u;
}

// Конвертирование строки из UTF8 в Win1251
static inline int ConvertStringUTF8to1251 (const char* utf8, char* windows1251, size_t n)
{
	 int i = 0;
	        int j = 0;
	        for(; i < (int)n && utf8[i] != 0; ++i) {
	                char prefix = utf8[i];
	                char suffix = utf8[i+1];
	                if ((prefix & 0x80) == 0) {
	                        windows1251[j] = (char)prefix;
	                        ++j;
	                } else if ((~prefix) & 0x20) {
	                        int first5bit = prefix & 0x1F;
	                        first5bit <<= 6;
	                        int sec6bit = suffix & 0x3F;
	                        int unicode_char = first5bit + sec6bit;


	                        if ( unicode_char >= 0x410 && unicode_char <= 0x44F ) {
	                                windows1251[j] = (char)(unicode_char - 0x350);
	                        } else if (unicode_char >= 0x80 && unicode_char <= 0xFF) {
	                                windows1251[j] = (char)(unicode_char);
	                        } else if (unicode_char >= 0x402 && unicode_char <= 0x403) {
	                                windows1251[j] = (char)(unicode_char - 0x382);
	                        } else {
	                                int count = sizeof(g_letters) / sizeof(Letter);
	                                for (int k = 0; k < count; ++k) {
	                                        if (unicode_char == g_letters[k].unicode) {
	                                                windows1251[j] = g_letters[k].win1251;
	                                                goto NEXT_LETTER;
	                                        }
	                                }
	                                // can't convert this char
	                                return 0;
	                        }
	NEXT_LETTER:
	                        ++i;
	                        ++j;
	                } else {
	                        // can't convert this chars
	                        return 0;
	                }
	        }
	        windows1251[j] = 0;
	        return 1;
}




#endif /* TFT_CONVERT_H_ */

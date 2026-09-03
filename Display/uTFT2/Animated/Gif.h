/*

 //Все гиф находятся в папке /Gif/%s/%d.bmp
 //Имя папки не более 8 символов
 
 Gif gif;
 gif.init(&tft);
 gif.setName((char *)"tA");  Откуда брать gif
 gif.setMaxIndex(44);        ?
 gif.setDelay(0);
 gif.useAplpha(1);
 gif.setSwap(0);
 gif.setXY(10, 10);
 gif.trigger  = HOVER;
 gif2.trigger = LOOP;
 gif3.trigger = MORPH;
 gif4.trigger = BOOMERANG;

 TFT_gif gif2;
 gif2.init(&tft);
 gif2.setName("Q1");
 gif2.setMaxIndex(23);
 gif2.setDelay(0);
 gif2.useAplpha(1);
 gif2.setSwap(0);
 gif2.SetXY(120, 120);
 gif2.setOffset(4*21);


 gif .command(STOP);
 gif .command(PLAY);

 gif.run(); Обработчик пеперисовки

 tft.ST7789_Update(gif.info());

  Триггер
  HOVER
  LOOP
  MORPH
  BOOMERANG
  ONCE
 */

#ifndef TFT_GIF_H_
#define TFT_GIF_H_

#include "main.h"
#include "TFT.h"

#include "fatfs.h"

#include <logUART.h>
extern classLog rtt;

#include "HiSpeedDWT.h"

extern HiSpeedDWT TimerT5;
extern HiSpeedDWT TimerDWT;
//https://lordicon.com/

#include "../Bitmap/bitmap.h"

enum ANIMATION_TRIGGERS {
	HOVER,      //Запуск без повторения застываем на последнем кадре
	LOOP,       //Запуск с повторнением
	MORPH,      //Запуск вперед пока есть поздействие и возврат
	BOOMERANG,  //Запуск приводит к движению в перед и назад
	ONCE        //Запуск Сначала невидимого обьекта с последующим исчезновением в конце
};

enum ANIMATION_COMMAND_STATE {
	STOP,    //Отобразить первый кадр
	PLAY,    //Начать анимацию
	PLAYMORPH,
	REPLAY,
	PAUSE,   //Остановить на текущем кадре
	END
};

#define BIT32 3
#define BIT16 1

class Gif {
public:

	//┌──────────────────────────────────┐
	//│ Инициализация                    │
	//└──────────────────────────────────┤
	void init(TFT *_TFT) {tft = _TFT;} //│
	//───────────────────────────────────┘

	//─────────────────────────────────────┐
	void setIndexMax(void)
	{
	  index_current = index_max;
	}
	//─────────────────────────────────────┘

	void command(ANIMATION_COMMAND_STATE c) {

		switch (c) {

		case PLAY:
			if (state_animation != PLAY) {
				index_current = 0;
				state_animation = PLAY;
				field.vector = 0;
			}
			break;


		case PLAYMORPH:
			if (state_animation != PLAY) {
				state_animation = PLAY;
			}
			break;

		case REPLAY:
			index_current = 0;
			state_animation = PLAY;
			field.vector = 0;
			break;

		case STOP:
			state_animation = STOP;
			field.vector = 0;
			break;

		default:
			break;
		}

	};

	void run(void) {calculate();}

	void setName(char *name) {
		sprintf(name_gif, "%s", name);

		//Читаем настройки
		char current_patch[32]; //Полный путь к файлу
		sprintf(current_patch, "/Gif/%s/i.txt", name_gif); //Собираем полный путь в файлу
		int res = f_open(&SDFile, current_patch, FA_READ);
		char BMP_From_File_buf[64];

		int b = 0;

		field.bit = 0;

		if (res == FR_OK) {
			f_gets(BMP_From_File_buf, 16, &SDFile);
			H = atoi(BMP_From_File_buf);
			f_gets(BMP_From_File_buf, 16, &SDFile);
			W = atoi(BMP_From_File_buf);

			f_gets(BMP_From_File_buf, 16, &SDFile);

		    b = atoi(BMP_From_File_buf);

			if (b == 32)
				field.bit = BIT32;

			if (b == 24)
				field.bit = 0;

			if (b == 16)
				field.bit = BIT16;

			f_gets(BMP_From_File_buf, 16, &SDFile);
			index_max = atoi(BMP_From_File_buf) - 1;

			rtt.print("(+) GIF name: %s  H: %d  W: %d  bit: %d  frame: %d\n", name_gif, H, W, field.bit, index_max + 1);
		}
		f_close(&SDFile);

	}

	void setDelay(int i) {delay = i;}
	void setXY(int X, int Y) {x = X; y = Y;}
	void setX(int X) {x = X;}
	void setY(int Y) {y = Y;}

	List_Update_Particle info(void) {
		List_Update_Particle U;
		U.x0 = x;
		U.y0 = y;
		U.x1 = x + W - 1;
		U.y1 = y + H - 1;
		return U;
	} //Информация используемая для того чтобы отрисовать кадр

	ANIMATION_TRIGGERS      trigger = HOVER; //Выбор типа анимации
	ANIMATION_COMMAND_STATE state_animation = STOP;

	struct fieldbite {
		unsigned int vector :1; //uint8_t vector = 0; //направление анимации 0- прямое 1- обратное
		unsigned int needUpdate :1;
		unsigned int bit :2; //1-16 3-32 0-error
	} field;

	uint8_t  delay    = 100; //Задержка
	Bitmap   bmpStop  = {0}; //Картинка отображаемая при отсуствии анимации
	Bitmap   bmpStart = {0}; //Картинка отображаемая при отсуствии анимации


private:
	//──────────────────────────────┬───────────────────────────────────┬──────────┐
	uint16_t   index_max     = 0; //│ Максимальный индекс               │ private: │
	int16_t    index_current = 0; //│ Текущий индекс                    └──────────┤
	                              //│                                              │
	uint8_t H = 0;                //│                                              │
	uint8_t W = 0;                //│                                              │
	                              //│                                              │
	int16_t x = 0;                //│                                              │
	int16_t y = 0;                //│                                              │
	                              //│                                              │
	TFT *tft;                     //│                                              │
	uint32_t start_time = 0;      //│ Записываем время начала проприсовки анимации │
	char name_gif[8] = { 0 };     //│ Название Gif папки                           │
	//──────────────────────────────┴──────────────────────────────────────────────┘

	//┌─────────────────────────────────────────────────────────────────┬──────────┐
    //│ Открыть картирку по индексу 16 и 24 бит                         │ private: │
	//└─────────────────────────────────────────────────────────────────┴──────────┤
	void openBMPfromIndex(uint8_t i) {

		if (i>index_max) return;

		char current_patch[32]; //Полный путь к файлу
		sprintf(current_patch, "/Gif/%s/res.bin", name_gif); //Собираем полный путь в файлу

		int res = f_open(&SDFile, current_patch, FA_READ); //1667us -Of Gen off Fat32 2к  1360 Fat16 16к

		//Ошибка открытия картинки с microSD
		if(res != FR_OK)		{
			rtt.print("\033[01;38;05;51mGif>\033[01;38;05;196mERROR open>\033[01;38;05;46m%s\n", current_patch);
			f_close(&SDFile);  //7uS
		    return;
		}

		uint8_t BMP_From_File_buf[4100] __attribute__((aligned (4)));

		uint32_t index;
		UINT bytesread;

		uint32_t max = H * W;
		int32_t _x, _y;

		float sAlpha_Float;
		float oneminusalpha_Float;

		uint32_t dColor;
		uint32_t sColor;

		uint32_t sR, sG, sB;
		uint32_t dR, dG, dB;
		uint32_t R, G, B;

		uint16_t *p16;

		if (field.bit == BIT32) {

			res = f_lseek(&SDFile, i * max * 4); //656us -Of Gen off
			if(res != FR_OK)
			{
				rtt.print("Gif>32>ERROR f_lseek\r\n");
				f_close(&SDFile);  //7uS
			    return;
			}

			for (index = 0; index < max; index++)
			{
				if (index % 1024 == 0) {
					f_read(&SDFile, &BMP_From_File_buf[0], 4096,
							&bytesread); ////915us -Of Gen off
					//p = (uint32_t *)&BMP_From_File_buf[0];
				}

				_x = (index % W) + x;
				_y = (index / W) + y;

				sAlpha_Float = BMP_From_File_buf[(index % 1024) * 4 ] / 255.0F;
				sR           = BMP_From_File_buf[(index % 1024) * 4 + 1];
				sG           = BMP_From_File_buf[(index % 1024) * 4 + 2];
				sB           = BMP_From_File_buf[(index % 1024) * 4 + 3];

				dColor = tft->LCD->buffer16[_x + _y * tft->LCD->TFT_WIDTH];

				dR = (dColor & 0xF800) >> 8;
				dG = (dColor & 0x7E0)  >> 3;
				dB = (dColor & 0x1F)   << 3;

				oneminusalpha_Float = 1.0F - sAlpha_Float;

				R = ((sR * sAlpha_Float) + (oneminusalpha_Float * dR));
				G = ((sG * sAlpha_Float) + (oneminusalpha_Float * dG));
				B = ((sB * sAlpha_Float) + (oneminusalpha_Float * dB));

				sColor = RGB565(R, G, B);
				tft->LCD->buffer16[_x + _y * tft->LCD->TFT_WIDTH] = sColor;
				}
			f_close(&SDFile);  //7uS
			return;
		}



		if (field.bit == BIT16) {
			res = f_lseek(&SDFile, i * max * 2); //656us -Of Gen off
			if(res != FR_OK)
			{
				rtt.print("Gif>16>ERROR f_lseek\n");
				f_close(&SDFile);  //7uS
			    return;
			}

			p16 = (uint16_t *)&BMP_From_File_buf[0];

			for (index = 0; index < max; index++)
			{
				if (index % 2048 == 0) {
					f_read(&SDFile, &BMP_From_File_buf[0], 4096,
							&bytesread); ////915us -Of Gen off
					p16 = (uint16_t *)&BMP_From_File_buf[0];
				}
				_x = (index % W) + x;
				_y = (index / W) + y;
				sColor = *p16++;
				tft->LCD->buffer16[_x + _y * tft->LCD->TFT_WIDTH] = sColor;
			}

			f_close(&SDFile);  //7uS
			return;
		}

	}
	//─────────────────────────────────────────────────────────────────────────────┘

	//┌─────────────────────────────────────────────────────────────────┬──────────┐
    //│ Логика отрисовки                                                │ private: │
	//└─────────────────────────────────────────────────────────────────┴──────────┤
	void calculate(void) {

		//Расчет следующего интекса с учетом времени кадра
		//if ((uwTick - start_time) > delay)
		{

			if (trigger == MORPH) {

				if (state_animation == STOP) {

					index_current--;
					if (index_current < 0)
					{
						index_current = 0;

						switch (bmpStart.bit) {
							case 32: Bitmap_From_Flash_32b(x, y, &bmpStart);	break;
							case 24: Bitmap_From_Flash_24b(x, y, &bmpStart);	break;
							case 16: Bitmap_From_Flash_16b(x, y, &bmpStart);	break;
							default: openBMPfromIndex(0); break; //Когда нет картинки в ресурсах
						}

					}
					else
					  openBMPfromIndex(index_current--);

					start_time = uwTick; //Запомнили начало
					return;
				}

				if (state_animation == PLAY) {

					index_current++;
					if (index_current >= index_max) {
						index_current = index_max;

						switch (bmpStop.bit) {
							case 32: Bitmap_From_Flash_32b(x, y, &bmpStop);	break;
							case 24: Bitmap_From_Flash_24b(x, y, &bmpStop);	break;
							case 16: Bitmap_From_Flash_16b(x, y, &bmpStop);	break;
							default: openBMPfromIndex(index_max); break; //Когда нет картинки в ресурсах
						}

					}
					else
					  openBMPfromIndex(index_current);

					start_time = uwTick; //Запомнили начало
					return;

				}

			}

            //////////////////////////////////////////////////////
			if (trigger == BOOMERANG) {

				if (state_animation == STOP) {

					switch (bmpStart.bit) {
						case 32: Bitmap_From_Flash_32b(x, y, &bmpStart);	break;
						case 24: Bitmap_From_Flash_24b(x, y, &bmpStart);	break;
						case 16: Bitmap_From_Flash_16b(x, y, &bmpStart);	break;
						default: openBMPfromIndex(0); break; //Когда нет картинки в ресурсах
					}


					start_time = uwTick; //Запомнили начало
					return;
				}

				if (state_animation == PLAY) {

					if (field.vector == 0) {

						openBMPfromIndex(index_current++);

						if (index_current > index_max) {
							field.vector = 1;
							index_current = index_max - 1;
						}

						start_time = uwTick; //Запомнили начало
						return;

					} else {

						openBMPfromIndex(index_current--);

						if (index_current < 0) {
							field.vector = 0;
							state_animation = STOP;
						}


						start_time = uwTick; //Запомнили начало
						return;

					}

				}

			}
			//////////////////////////////////////////////////////

			if (trigger == LOOP) {

				if (state_animation == PLAY) {
					openBMPfromIndex(index_current);
					index_current++;
					if (index_current > index_max)
						index_current = 0;

					start_time = uwTick; //Запомнили начало
					return;
				}

				if (state_animation == STOP) {

					switch (bmpStart.bit) {
						case 32: Bitmap_From_Flash_32b(x, y, &bmpStart);	break;
						case 24: Bitmap_From_Flash_24b(x, y, &bmpStart);	break;
						case 16: Bitmap_From_Flash_16b(x, y, &bmpStart);	break;
						default: openBMPfromIndex(0); break; //Когда нет картинки в ресурсах
					}

					start_time = uwTick; //Запомнили начало
					return;
				}

			}
			//////////////////////////////////////////////////////


			//Один раз воспроизвести и остановиться на последнем кадре
			if (trigger == HOVER) {
				if (state_animation == STOP) {
					//SEGGER_RTT_WriteString(0, "STOP\n");

					switch (bmpStop.bit) {
						case 32: Bitmap_From_Flash_32b(x, y, &bmpStop);	break;
						case 24: Bitmap_From_Flash_24b(x, y, &bmpStop);	break;
						case 16: Bitmap_From_Flash_16b(x, y, &bmpStop);	break;
						default: openBMPfromIndex(index_max); break; //Когда нет картинки в ресурсах
					}

					start_time = uwTick; //Запомнили начало
					return;
				}

				if (state_animation == PLAY) {

					//SEGGER_RTT_WriteString(0, "PLAY\n");

					TimerDWT.Start();
					openBMPfromIndex(index_current);
					TimerDWT.Loger((char*)"openBMPfromIndex");

					index_current++;
					if (index_current > index_max) {
						index_current = index_max;
						state_animation = STOP;
					}

					start_time = uwTick; //Запомнили начало
					return;
				}
			}


			//Один раз воспроизвести и остановиться на последнем кадре
			if (trigger == ONCE) {
				if (state_animation == STOP) {
					return;
				}

				if (state_animation == PLAY) {

					openBMPfromIndex(index_current);

					index_current++;
					if (index_current > index_max) {
						index_current = index_max;
						state_animation = STOP;
					}

					return;
				}
			}




		}
		start_time = uwTick; //Запомнили начало
	}

	//┌─────────────────────────────────────────────────────────────────┬──────────┐
    //│ Открыть картирку из флеш                                        │ private: │
	//└─────────────────────────────────────────────────────────────────┴──────────┤
	void Bitmap_From_Flash_16b(int16_t X, int16_t Y, Bitmap *bmp) {

			const uint16_t *p16;
			p16 = (uint16_t *)bmp->data;
			int32_t pX;
			int32_t pY;
		    int _H = bmp->H + Y;
		    int _W = bmp->W + X;

			for ( pY = Y; pY < _H; pY++) {
				for ( pX = X; pX < _W; pX++)
				    tft->LCD->buffer16[pX + pY * tft->LCD->TFT_WIDTH] = *p16++;
			}
	}

	void Bitmap_From_Flash_24b(int16_t X, int16_t Y, Bitmap *bmp) {

		    rtt.print("Bitmap_From_Flash_24b\n");

			const uint8_t *p8;
			p8 = (uint8_t *)bmp->data;
			int32_t pX;
			int32_t pY;
		    int _H = bmp->H + Y;
		    int _W = bmp->W + X;

		    uint8_t A, HI;
			uint16_t Color;
			uint16_t dColor;
			uint32_t delta;

			for ( pY = Y; pY < _H; pY++) {
				for ( pX = X; pX < _W; pX++)
				{
					A = *p8++;
				    HI = *p8++;
				    Color = HI | ( *p8++ << 8 );
				    delta = pX + pY * 240;
				    dColor = tft->LCD->buffer16[delta];
				    tft->LCD->buffer16[delta] = tft->alphaBlend(A, Color, dColor);
				}
			}

	}

	//32 бит BMP с альфа каналом Сохранять как инвертированая альфа и свап, customAlpha = 1.0 полная альфа
	void Bitmap_From_Flash_32b(int16_t x0, int16_t y0,	Bitmap *bmp) {

		uint32_t sColor;
		uint32_t dColor;

		uint32_t sR, sG, sB;
		uint32_t dR, dG, dB;
		uint32_t R, G, B;

		int32_t alpha;
		int32_t oneminusalpha;

		int _H = bmp->H + y0;
		int _W = bmp->W + x0;

		int32_t pX;
		int32_t pY;

		uint32_t deltaX;

		uint8_t *p8;
		p8 = (uint8_t *)&bmp->data[0];

		for ( pY = y0; pY < _H; pY++) {
			for ( pX = x0; pX < _W; pX++)
			{
				deltaX = pX + pY * 240;

				alpha = *p8++;
				sR    = *p8++;
				sG    = *p8++;
				sB    = *p8++;

				dColor = tft->LCD->buffer16[deltaX]; //GetPixel(x, y);

				dR = (dColor & 0xF800) >> 8;
				dG = (dColor & 0x7E0)  >> 3;
				dB = (dColor & 0x1F)   << 3;

				oneminusalpha = 255 - alpha;

				R = (uint8_t)(((sR * alpha) + (oneminusalpha * dR)) >> 11 );
				G = (uint8_t)(((sG * alpha) + (oneminusalpha * dG)) >> 10 );
				B = (uint8_t)(((sB * alpha) + (oneminusalpha * dB)) >> 11 );

				sColor = (R << 11) | (G << 5) | B; //tft->RGB565(R, G, B); //(R << 11) | (G << 5) | (B << 0); //
				tft->LCD->buffer16[deltaX] = sColor;

			}

		}
	}
	//─────────────────────────────────────────────────────────────────────────────┘
};

#endif /* TFT_GIF_H_ */

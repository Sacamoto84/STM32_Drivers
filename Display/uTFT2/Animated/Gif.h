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

#include "TFT_config.h"

#if (FAT_FS)

#include "main.h"
#include "TFT.h"

#include "fatfs.h"

#include <stdio.h> //snprintf

//Логирование (опционально, define TFT_USE_TIMBER в TFT_config.h)
#ifdef TFT_USE_TIMBER
#include <timber.h>
#define TFT_GIF_LOG(...) timber.print(__VA_ARGS__)
#else
#define TFT_GIF_LOG(...) do {} while (0)
#endif

//Замер времени отрисовки (опционально, define TFT_USE_DWT в TFT_config.h)
#ifdef TFT_USE_DWT
#include "HiSpeedDWT.h"
extern HiSpeedDWT TimerT5;
extern HiSpeedDWT TimerDWT;
#define TFT_GIF_DWT_START() TimerDWT.Start()
#define TFT_GIF_DWT_LOG(s) TimerDWT.Loger(s)
#else
#define TFT_GIF_DWT_START() do {} while (0)
#define TFT_GIF_DWT_LOG(s) do {} while (0)
#endif

//https://lordicon.com/

#include "../Bitmap/bitmap.h"

enum ANIMATION_TRIGGERS {
	HOVER,      //Запуск без повторения застываем на последнем кадре
	LOOP,       //Запуск с повторнением
	MORPH,      //Запуск вперед пока есть поздействование и возврат
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
		snprintf(name_gif, sizeof(name_gif), "%s", name);

		//Читаем настройки
		char current_patch[64]; //Полный путь к файлу
		snprintf(current_patch, sizeof(current_patch), "/Gif/%s/i.txt", name_gif); //Собираем полный путь в файлу
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

			TFT_GIF_LOG("(+) GIF name: %s  H: %d  W: %d  bit: %d  frame: %d\n", name_gif, H, W, field.bit, index_max + 1);
			f_close(&SDFile);
		}

	}

	void setDelay(uint32_t i) {delay = i;}
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

	uint32_t delay    = 100; //Задержка кадра, мс
	Bitmap   bmpStop  = {0, 0, 0, NULL, NULL, NULL, NULL}; //Картинка отображаемая при отсутствии анимации
	Bitmap   bmpStart = {0, 0, 0, NULL, NULL, NULL, NULL}; //Картинка отображаемая при отсутствии анимации


private:
	//──────────────────────────────┬───────────────────────────────────┬──────────┐
	uint16_t   index_max     = 0; //│ Максимальный индекс               │ private: │
	int32_t    index_current = 0; //│ Текущий индекс                    └──────────┤
	                              //│                                              │
	uint16_t H = 0;                //│                                              │
	uint16_t W = 0;                //│                                              │
	                              //│                                              │
	int16_t x = 0;                //│                                              │
	int16_t y = 0;                //│                                              │
	                              //│                                              │
	TFT *tft;                     //│                                              │
	uint32_t start_time = 0;      //│ Записываем время начала проприсовки анимации │
	char name_gif[9] = { 0 };     //│ Название Gif папки                           │
	//──────────────────────────────┴──────────────────────────────────────────────┘

	//┌─────────────────────────────────────────────────────────────────┬──────────┐
    //│ Открыть картирку по индексу 16 и 24 бит                         │ private: │
	//└─────────────────────────────────────────────────────────────────┴──────────┤
	void openBMPfromIndex(uint16_t i) {

		if (i > index_max) return;

		char current_patch[32]; //Полный путь к файлу
		snprintf(current_patch, sizeof(current_patch), "/Gif/%s/res.bin", name_gif); //Собираем полный путь в файлу

		int res = f_open(&SDFile, current_patch, FA_READ); //1667us -Of Gen off Fat32 2к  1360 Fat16 16к

		//Ошибка открытия картинки с microSD
		if(res != FR_OK)		{
			TFT_GIF_LOG("\033[01;38;05;51mGif>\033[01;38;05;196mERROR open>\033[01;38;05;46m%s\n", current_patch);
		    return;
		}

		//Общий статический буфер (не на стеке). Функция не реентерабельна (использует SDFile)
		static uint8_t BMP_From_File_buf[4096] __attribute__((aligned (4)));

		uint32_t index;
		UINT bytesread;

		uint32_t max = (uint32_t)H * W;
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

			res = f_lseek(&SDFile, (uint32_t)i * max * 4); //656us -Of Gen off
			if(res != FR_OK)
			{
				TFT_GIF_LOG("Gif>32>ERROR f_lseek\r\n");
				f_close(&SDFile);  //7uS
			    return;
			}

			for (index = 0; index < max; index++)
			{
				if (index % 1024 == 0) {
					f_read(&SDFile, &BMP_From_File_buf[0], 4096,
							&bytesread); ////915us -Of Gen off
				}

				_x = (index % W) + x;
				_y = (index / W) + y;

				//Клиппинг: не выходим за границы фреймбуфера
				if (_x < 0 || _x >= tft->LCD->TFT_WIDTH ||
					_y < 0 || _y >= tft->LCD->TFT_HEIGHT)
					continue;

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
			res = f_lseek(&SDFile, (uint32_t)i * max * 2); //656us -Of Gen off
			if(res != FR_OK)
			{
				TFT_GIF_LOG("Gif>16>ERROR f_lseek\n");
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
				//Читаем пиксель ДО проверки клиппинга: иначе при выходе
				//картинки за экран поток байтов рассинхронизируется
				//и весь кадр смещается/рвется
				sColor = *p16++;

				_x = (index % W) + x;
				_y = (index / W) + y;

				//Клиппинг: не выходим за границы фреймбуфера
				if (_x < 0 || _x >= tft->LCD->TFT_WIDTH ||
					_y < 0 || _y >= tft->LCD->TFT_HEIGHT)
					continue;

				tft->LCD->buffer16[_x + _y * tft->LCD->TFT_WIDTH] = sColor;
			}

			f_close(&SDFile);  //7uS
			return;
		}

		//Битность не поддержана (24 бит или ошибка конфигурации) - закрываем файл
		f_close(&SDFile);
	}
	//─────────────────────────────────────────────────────────────────────────────┘

	//┌─────────────────────────────────────────────────────────────────┬──────────┐
    //│ Логика отрисовки                                                │ private: │
	//└─────────────────────────────────────────────────────────────────┴──────────┤
	void calculate(void) {

		//Расчет следующего интекса с учетом времени кадра
		if ((uwTick - start_time) <= delay)
			return;

		{

			if (trigger == MORPH) {

				if (state_animation == STOP) {

					index_current--;
					if (index_current < 0)
					{
						index_current = 0;

						switch (bmpStart.bit) {
							case 32: BitmapFromFlash32b(tft, x, y, &bmpStart);	break;
							case 24: BitmapFromFlash24b(tft, x, y, &bmpStart);	break;
							case 16: BitmapFromFlash16b(tft, x, y, &bmpStart);	break;
							default: openBMPfromIndex(0); break; //Когда нет картинки в ресурсах
						}

					}
					else
					  openBMPfromIndex(index_current);

					start_time = uwTick; //Запомнили начало
					return;
				}

				if (state_animation == PLAY) {

					index_current++;
					if (index_current >= index_max) {
						index_current = index_max;

						switch (bmpStop.bit) {
							case 32: BitmapFromFlash32b(tft, x, y, &bmpStop);	break;
							case 24: BitmapFromFlash24b(tft, x, y, &bmpStop);	break;
							case 16: BitmapFromFlash16b(tft, x, y, &bmpStop);	break;
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
						case 32: BitmapFromFlash32b(tft, x, y, &bmpStart);	break;
						case 24: BitmapFromFlash24b(tft, x, y, &bmpStart);	break;
						case 16: BitmapFromFlash16b(tft, x, y, &bmpStart);	break;
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
						case 32: BitmapFromFlash32b(tft, x, y, &bmpStart);	break;
						case 24: BitmapFromFlash24b(tft, x, y, &bmpStart);	break;
						case 16: BitmapFromFlash16b(tft, x, y, &bmpStart);	break;
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

					switch (bmpStop.bit) {
						case 32: BitmapFromFlash32b(tft, x, y, &bmpStop);	break;
						case 24: BitmapFromFlash24b(tft, x, y, &bmpStop);	break;
						case 16: BitmapFromFlash16b(tft, x, y, &bmpStop);	break;
						default: openBMPfromIndex(index_max); break; //Когда нет картинки в ресурсах
					}

					start_time = uwTick; //Запомнили начало
					return;
				}

				if (state_animation == PLAY) {

					TFT_GIF_DWT_START();
					openBMPfromIndex(index_current);
					TFT_GIF_DWT_LOG((char*)"openBMPfromIndex");

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

};

#endif /* FAT_FS */

#endif /* TFT_GIF_H_ */

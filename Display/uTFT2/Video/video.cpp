#include "TFT_config.h"

#if defined(TFT_UNIT_VIDEO) && (FAT_FS)

#include "video.h"

#include "fatfs.h"
#include <stdio.h>

#include "../TFT.h"
#include "../TFT_convert.h"
extern FIL SDFile;

uint32_t (*videoCallBackFunc)(uint32_t);          //Некая функция, используется для остановки видео по событию во время проигрования
uint32_t video_stop;                              //Для user как некая временная переменная (прекратить повторный запуск)

//Video только для экранов с 16-битным фреймбуфером, размер кадра = TFT_WIDTH * TFT_HEIGHT * 2
void video_load(TFT * tft, uint8_t delay)
{
    uint32_t index;
	uint32_t fsize = f_size(&SDFile);
	//Размер кадра в байтах - из параметров экрана (без хардкода 115200)
	uint32_t frame_size = (uint32_t)tft->LCD->TFT_WIDTH * (uint32_t)tft->LCD->TFT_HEIGHT * 2;
	UINT bytesread;
	uint32_t start_time = uwTick;    //Запоменаем время начала кадра
	int32_t delta;                   //Время которое нужно подождать до delay
	video_stop = 0;                  //Сброс временной переменной

	if (frame_size == 0 || tft->LCD->Bit != 16 || tft->LCD->buffer16 == NULL) return;

#if defined(TFT_DRIVER_ST7789)
	tft->driver.ST7789_Update_DMA_Cicle_On();    //Запуск кольцевого буфера
#endif

    //11ms delay=0;
	//─ Цикл ────────────────────────────────────────────────────────────────────┐
	for(index = 0; index + frame_size <= fsize; index += frame_size){         //│
	                                                                       //│
		//─ Читаем полный кадр ─                                               //│
		f_read (&SDFile, (uint8_t*)(tft->LCD->buffer16), frame_size, &bytesread);  //│
		if (bytesread != frame_size) break; //Обрыв файла                     //│
		                                                                       //│
		//─ Проверка на выход из цикла ─────────────────────────────────────────┐│
		if (videoCallBackFunc) {                                              //││
	     	uint32_t (*fcnPtr)(uint32_t) = videoCallBackFunc;                 //││
			if(fcnPtr(0)) break; //if callback возвращает 1, то выйти из цикла  ││
		}                                                                     //││
		//──────────────────────────────────────────────────────────────────────┘│
		                                                                       //│
		//─ Задержка кадра ─────────────────────────────────────────────────────┐│
		delta =  delay - (uwTick - start_time) - 1;    //Время до времени кадра ││
		                                                                      //││
		if ((delay) && (delta > 0)){                                          //││
			  uint32_t tickstart = uwTick;                                    //││
			  while((uwTick - tickstart) < (uint32_t)delta){                  //││
				if (videoCallBackFunc){                                       //││
				    uint32_t (*fcnPtr)(uint32_t) = videoCallBackFunc;         //││
					if(fcnPtr(0)){                                            //││
#if defined(TFT_DRIVER_ST7789)
						tft->driver.ST7789_Update_DMA_Cicle_Off();            //││
#endif
						return;       //Выходим из функции если callback == 1   ││
					}                                                         //││
				}                                                             //││
			  }                                                               //││
		}                                                                     //││
		start_time = uwTick;                                                  //││
		//──────────────────────────────────────────────────────────────────────┘│
	}                                                                          //│
	//───────────────────────────────────────────────────────────────────────────┘
#if defined(TFT_DRIVER_ST7789)
	tft->driver.ST7789_Update_DMA_Cicle_Off();
#endif
}

void video_play(TFT * tft, char * Name, uint8_t delay){
	char str[32];
	snprintf(str, sizeof(str), "%s", Name);
	ConvertStringDosTo1251 ( str );
	//Каждый кириллический символ разворачивается в 2 байта UTF-8
	char strUTF8[sizeof(str) * 2];
	ConvertString1251ToUTF8(str, strUTF8);
	TFT_VIDEO_LOG("\033[04;38;05;226;48;05;24mvideo_play '%s' %dms\x1B[0m\n", strUTF8, delay );
	int res;
	res = f_open(&SDFile, Name, FA_READ);
    if (res == FR_OK)
	{
    	video_load(tft, delay);
        f_close(&SDFile);
	}
	else
      TFT_VIDEO_LOG("video_play == Ошибка открытия файла\n");
}

#endif

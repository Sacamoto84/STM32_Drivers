#include "TFT_config.h"

#if defined(TFT_UNIT_VIDEO)

#include "video.h"

#include "fatfs.h"
#include <stdio.h>

#include "../TFT.h"
#include "logUART.h"
#include "../TFT_convert.h"

extern classLog rtt;
extern FIL SDFile;

uint32_t (*videoCallBackFunc)(uint32_t);          //Некая функция, используется для остановки видео по событию во время проигрования
uint32_t video_stop;                              //Для user как некая временная переменная (прекратить повторный запуск)

//Video только для 240x240 ST7789
void video_load(TFT * tft, uint8_t delay)
{
    uint32_t index;
	uint32_t fsize = f_size(&SDFile);
	tft->driver.ST7789_Update_DMA_Cicle_On();    //Запуск кольцевого буфера
	UINT bytesread;
	uint32_t start_time = uwTick;    //Запоменаем время начала кадра
	int32_t delta;                   //Время которое нужно подождать до delay
	video_stop = 0;                  //Сброс временной переменной
    //11ms delay=0;
	//─ Цикл ────────────────────────────────────────────────────────────────────┐
	for(index = 0; index < fsize; index+=115200){                              //│
		                                                                       //│
		//─ Читаем полный кадр ─                                               //│
		f_read (&SDFile, (uint8_t*)(tft->LCD->buffer16), 115200, &bytesread);       //│
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
						tft->driver.ST7789_Update_DMA_Cicle_Off();                        //││
						return;       //Выходим из функции если callback == 1   ││
					}                                                         //││
				}                                                             //││
			  }                                                               //││
		}                                                                     //││
		start_time = uwTick;                                                  //││
		//──────────────────────────────────────────────────────────────────────┘│
	}                                                                          //│
	//───────────────────────────────────────────────────────────────────────────┘
	tft->driver.ST7789_Update_DMA_Cicle_Off();
}

void video_play(TFT * tft, char * Name, uint8_t delay){
	char str[32];
	sprintf(str,"%s", Name);
	ConvertStringDosTo1251 ( str );
	char strUTF8[48];
	ConvertString1251ToUTF8(str, strUTF8);
	rtt.print("\033[04;38;05;226;48;05;24mvideo_play '%s' %dms\x1B[0m\n", strUTF8, delay );
	int res;
	res = f_open(&SDFile, Name, FA_READ);
    if (res == FR_OK)
	{
    	video_load(tft, delay);
        f_close(&SDFile);
	}
	else
      rtt.print("video_play == Ошибка открытия файла\n");
}

#endif

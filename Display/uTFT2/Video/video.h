/*
 * video.h
 *
 *  Created on: 2 янв. 2023 г.
 *      Author: Ivan
 */

#ifndef VIDEO_VIDEO_H_
#define VIDEO_VIDEO_H_

#include "TFT_config.h"

#if defined(TFT_UNIT_VIDEO) && (FAT_FS)

#if defined(TFT_PLATFORM_STM32)
#include "main.h"
#endif

#include "../TFT.h"

#include "fatfs.h"

//Логирование (опционально, define TFT_USE_TIMBER в TFT_config.h)
#ifdef TFT_USE_TIMBER
#include "Timber.h"
#define TFT_VIDEO_LOG(...) timber.print(__VA_ARGS__)
#else
#define TFT_VIDEO_LOG(...) do {} while (0)
#endif

#ifndef TFT_VIDEO_GET_TICK
#if defined(TFT_PLATFORM_PICO)
#define TFT_VIDEO_GET_TICK() to_ms_since_boot(get_absolute_time())
#else
#define TFT_VIDEO_GET_TICK() HAL_GetTick()
#endif
#endif

extern uint32_t (*videoCallBackFunc)(uint32_t);          //Некая функция, используется для остановки видео по событию во время проигрования
extern volatile uint32_t video_stop;                     //Для user как некая временная переменная (прекратить повторный запуск)

extern void video_load(TFT * tft, uint8_t delay);
extern void video_play(TFT * tft, const char * Name, uint8_t delay);

#endif /* TFT_UNIT_VIDEO && FAT_FS */

#endif /* VIDEO_VIDEO_H_ */



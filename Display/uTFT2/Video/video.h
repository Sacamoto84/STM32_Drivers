/*
 * video.h
 *
 *  Created on: 2 янв. 2023 г.
 *      Author: Ivan
 */

#ifndef VIDEO_VIDEO_H_
#define VIDEO_VIDEO_H_

#include "TFT_config.h"

#if defined(TFT_UNIT_VIDEO)

#include "main.h"

#include "../TFT.h"

#include "fatfs.h"

#include "logUART.h"
extern classLog rtt;

extern uint32_t (*videoCallBackFunc)(uint32_t);          //Некая функция, используется для остановки видео по событию во время проигрования
extern uint32_t video_stop;                              //Для user как некая временная переменная (прекратить повторный запуск)

extern void video_play(TFT * tft, char * Name, uint8_t delay);

#endif /* TFT_UNIT_VIDEO */

#endif /* VIDEO_VIDEO_H_ */



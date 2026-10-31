/*
 * TFTWiget.h
 *
 *  Created on: 24 мар. 2022 г.
 *      Author: Ivan
 */

#ifndef TFT_WIGET_ANIMATED_RECTAGLE_H_
#define TFT_WIGET_ANIMATED_RECTAGLE_H_

#include "../TFT_define.h"

#if defined(TFT_PLATFORM_STM32)
#include "main.h"
#define TFT_GET_TICK() HAL_GetTick()
#elif defined(TFT_PLATFORM_PICO)
#define TFT_GET_TICK() to_ms_since_boot(get_absolute_time())
#else
#define TFT_GET_TICK() 0
#endif

#include "../Animated/EasingProvider.h"
#include "../TFT.h"

class TFT_Animated_Rectagle {
public:

	void init_tft(TFT * _TFT)             { tft = _TFT;    }
    void setHW(uint16_t _H, uint16_t _W ) { H = _H; W =_W; }
    void setH (uint16_t _H)               { H = _H;        }
	void setW (uint16_t _W )              { W =_W;         }
	void setXY(uint16_t X, uint16_t Y )   { setX(X); setY(Y);  }
	void setX (uint16_t X)                { if (tft && tft->LCD) x = (tft->LCD->TFT_WIDTH > 0) ? constrain(X, (uint16_t)0, (uint16_t)(tft->LCD->TFT_WIDTH - 1)) : X; else x = X; }
	void setY (uint16_t Y)                { if (tft && tft->LCD) y = (tft->LCD->TFT_HEIGHT > 0) ? constrain(Y, (uint16_t)0, (uint16_t)(tft->LCD->TFT_HEIGHT - 1)) : Y; else y = Y; }
	List_Update_Particle info(void)       { List_Update_Particle U;  U.H = H; U.W = W; U.x0 = x; U.y0 = y; U.x1 = (W > 0) ? (x + W - 1) : x; U.y1 = (H > 0) ? (y + H - 1) : y; return U; }
    void setEaseP(Ease e)                 { easeP = e; } //Выбор типа движения в анимации
    void setEaseN(Ease e)                 { easeN = e; } //Выбор типа движения в анимации
    void setColor(uint16_t c)             { color = c; } //Цвет закраски
    void setDurationP(float ms)           { if (ms > 0.0F) animation_max_time_P = ms; }
    void setDurationN(float ms)           { if (ms > 0.0F) animation_max_time_N = ms; }
    void setDuration(float ms)            { setDurationP(ms); setDurationN(ms); }

    uint8_t needUpdate(void)              {
    	if (_needUpdate)
    	{
    		_needUpdate = 0;
    		return 1;
    	}
    	return 0;};

    void run()
    {
       if (tft == NULL || tft->LCD == NULL) return;

       float temp;
       float elapsedTimeRate; //Состояние анимации от 0.0 до 1.0
       float max_time_P = (animation_max_time_P > 0.0F) ? animation_max_time_P : 1.0F;
       float max_time_N = (animation_max_time_N > 0.0F) ? animation_max_time_N : 1.0F;

    	if (P)
    	{
    	  temp = (float)( (float)( TFT_GET_TICK() - animation_start_time_P ) / max_time_P ); //Получаем время 0..1 анимации
    	  if (temp >= 1.0F) {P = 0; temp = 1.0F; } //Анимацию больше не использовать
    	  Gloabal_percent = temp;

    	  elapsedTimeRate = Easing.get(easeP, temp ); //Получаем коеффициент смещения 0..1 от времени c учетом типа анимации
    	  //Overshoot-анимации (BACK/ELASTIC) дают значения вне [0..1]:
    	  //отрицательная ширина после приведения к uint16_t - это UB
    	  if (elapsedTimeRate < 0.0F) elapsedTimeRate = 0.0F;
    	  if (elapsedTimeRate > 1.0F) elapsedTimeRate = 1.0F;

    	  if (typeAnimation == 0){  tft->RectangleFilled(x, y, (uint16_t)(W*elapsedTimeRate), H, color);  }

    	  if (typeAnimation == 1){
    		  uint16_t curW = (uint16_t)(W * elapsedTimeRate);
    		  tft->RectangleFilled(x + (W - curW) / 2, y, curW, H, color);
    	  }

    	  if (typeAnimation == 2){
              uint16_t col = tft->GetPixel(x, y);
              uint16_t newcolor = tft->alphaBlend( (uint8_t)(255.0F*elapsedTimeRate), color, col);
    		  tft->RectangleFilled(x, y, W, H, newcolor);  }

    	  _needUpdate = 1;
    	}


    	if (N)
    	{
    	  temp = (1.0F - Gloabal_percent) + (float)( (float)( TFT_GET_TICK() - animation_start_time_N ) / max_time_N ); //Получаем время 0..1 анимации

    	  if (temp >= 1.0F) {N = 0; temp = 1.0F; } //Анимацию больше не использовать
    	  temp = constrain(temp, 0.0F, 1.0F);
    	  elapsedTimeRate = Easing.get(easeN, (1.0F - temp) ); //Получаем коеффициент смещения 0..1 от времени c учетом типа анимации
    	  if (elapsedTimeRate < 0.0F) elapsedTimeRate = 0.0F;
    	  if (elapsedTimeRate > 1.0F) elapsedTimeRate = 1.0F;


    	  if (typeAnimation == 0)
    	  {
    		  tft->RectangleFilled(x, y, (uint16_t)(W*elapsedTimeRate), H, color);
    	  }

    	  if (typeAnimation == 1)
    	  {
    		  uint16_t curW = (uint16_t)(W * elapsedTimeRate);
    		  tft->RectangleFilled(x + (W - curW) / 2, y, curW, H, color);
    	  }

    	  if (typeAnimation == 2)
    	  {
              uint16_t col = tft->GetPixel(x, y);
              uint16_t newcolor = tft->alphaBlend( (uint8_t)(255.0F*elapsedTimeRate), color, col);

    		  tft->RectangleFilled(x, y, W, H, newcolor);
    	  }

    	  _needUpdate = 1;
    	}

    }


	void select(uint8_t s)
	{

		_select = s;

		if ((_select == 1) && (_select_last ==  0))
		{
		  P = 1;
		  N = 0;
		  animation_start_time_P = TFT_GET_TICK();
		  _select_last = s;
		  return;
		}

		if ((_select == 0) && (_select_last ==  1))
		{
           P = 0;
		   N = 1;
		   animation_start_time_N =  TFT_GET_TICK();
		}

		_select_last = s;

	}

  //https://tympanus.net/Development/CreativeButtons/
  uint8_t typeAnimation = 0; //Тип анимации 0-слева линейное 1-из середины в 2 стороны 2-закраска квадрата

private:
	TFT * tft = NULL;

	uint16_t W = 1;    //Ширина кнопки
	uint16_t H = 1;    //Высота кнопки
	uint16_t color = 0xFFFF; //Цвет закраски (вместо внешней palitra[])
	uint16_t x = 0;
	uint16_t y = 0;

	EasingProvider Easing;
	Ease easeP = BOUNCE_OUT; //Выбор типа анимации
	Ease easeN = QUAD_IN_OUT;

	uint8_t _select = 0;
	uint8_t _select_last = 0;

    uint8_t _needUpdate = 0; //Флаг того что нужно обновить обьект

	int8_t P = 0; //Флаг того что положительный фронт
	int8_t N = 0; //

	uint32_t animation_start_time_P = 0; //Время старта анимации
	uint32_t animation_start_time_N = 0; //Время старта анимации
    float    animation_max_time_P =  500.0F; //Длинна анимации в ms
    float    animation_max_time_N =  500.0F; //Длинна анимации в ms
    float    Gloabal_percent = 0.0F;

};

#endif /* TFT_WIGET_ANIMATED_RECTAGLE_H_ */

#ifndef HISPEEDTIMER_H_
#define HISPEEDTIMER_H_

//#define RTT_CTRL_RESET                "\x1B[0m"         // Reset to default colors

//#define RTT_CTRL_TEXT_BLACK           "\x1B[2;30m"
//#define RTT_CTRL_TEXT_RED             "\x1B[2;31m"
//#define RTT_CTRL_TEXT_GREEN           "\x1B[2;32m"
//#define RTT_CTRL_TEXT_YELLOW          "\x1B[2;33m"
//#define RTT_CTRL_TEXT_BLUE            "\x1B[2;34m"
//#define RTT_CTRL_TEXT_MAGENTA         "\x1B[2;35m"
//#define RTT_CTRL_TEXT_CYAN            "\x1B[2;36m"
//#define RTT_CTRL_TEXT_WHITE           "\x1B[2;37m"

//#define RTT_CTRL_TEXT_BRIGHT_BLACK    "\x1B[1;30m"
//#define RTT_CTRL_TEXT_BRIGHT_RED      "\x1B[1;31m"
//#define RTT_CTRL_TEXT_BRIGHT_GREEN    "\x1B[1;32m"
//#define RTT_CTRL_TEXT_BRIGHT_YELLOW   "\x1B[1;33m"
//#define RTT_CTRL_TEXT_BRIGHT_BLUE     "\x1B[1;34m"
//#define RTT_CTRL_TEXT_BRIGHT_MAGENTA  "\x1B[1;35m"
//#define RTT_CTRL_TEXT_BRIGHT_CYAN     "\x1B[1;36m"
//#define RTT_CTRL_TEXT_BRIGHT_WHITE    "\x1B[1;37m"

//#define RTT_CTRL_BG_BLACK             "\x1B[24;40m"
//#define RTT_CTRL_BG_RED               "\x1B[24;41m"
//#define RTT_CTRL_BG_GREEN             "\x1B[24;42m"
//#define RTT_CTRL_BG_YELLOW            "\x1B[24;43m"
//#define RTT_CTRL_BG_BLUE              "\x1B[24;44m"
//#define RTT_CTRL_BG_MAGENTA           "\x1B[24;45m"
//#define RTT_CTRL_BG_CYAN              "\x1B[24;46m"
//#define RTT_CTRL_BG_WHITE             "\x1B[24;47m"

//#define RTT_CTRL_BG_BRIGHT_BLACK      "\x1B[4;40m"
//#define RTT_CTRL_BG_BRIGHT_RED        "\x1B[4;41m"
//#define RTT_CTRL_BG_BRIGHT_GREEN      "\x1B[4;42m"
//#define RTT_CTRL_BG_BRIGHT_YELLOW     "\x1B[4;43m"
//#define RTT_CTRL_BG_BRIGHT_BLUE       "\x1B[4;44m"
//#define RTT_CTRL_BG_BRIGHT_MAGENTA    "\x1B[4;45m"
//#define RTT_CTRL_BG_BRIGHT_CYAN       "\x1B[4;46m"
//#define RTT_CTRL_BG_BRIGHT_WHITE      "\x1B[4;47m"


//Используется для подсчета времени
//Таймера используем из списка APB1 медленные, тайминг на них SystemCoreClock/2 , наиболее подходят таймера 32бит
//Internal clock
//PSR=0 Up ARR-Максимальное 4294967295 Остальное по умолчанию
//Для использования с 16 бит таймерами нужно использовать void init(&htim5, 1); для 32b init(&htim5, true);
//16 битовые таймера настраиваем на отсчет на 1us за тик значением PSR, максимальное время 65mS

#include "main.h"

#include <stdio.h>

//#include <math.h>

#include "timber.h"

#define    DWT_CYCCNT    *(volatile uint32_t*)0xE0001004
#define    DWT_CONTROL   *(volatile uint32_t*)0xE0001000
#define    SCB_DEMCR     *(volatile uint32_t*)0xE000EDFC

extern __IO uint32_t uwTick;
extern uint32_t SystemCoreClock;

typedef char __attribute__((aligned (4))) char4;

class HiSpeedDWT{
private:

    bool bit16 = false; //признак тогочто это 16 бит таймер

	TIM_HandleTypeDef * htim;

    __IO uint32_t * Count; //Текущий счсетчик

    uint32_t StartCount;
    uint32_t EndCount;

    uint32_t tickToMs;

    uint32_t result;
public:

    uint32_t tickToUs;// получаем кол-во тактов за 1 мкс

	void     Start(void)  {*Count = 0;}
	uint32_t End  (void)  {EndCount = *Count; return EndCount;}

	//Без параметров использует DWT
	void init(void)
	{
		//SEGGER_RTT_printf(0, "\x1B[38;05;0;48;05;11mDWT>");
		//SEGGER_RTT_printf(0, "\x1B[48;05;10m Init SystemCoreClock %d %s\r\n", SystemCoreClock, RTT_CTRL_RESET);
		tickToUs = SystemCoreClock / 1000000; //получаем кол-во тактов за 1 мкс
		tickToMs = tickToUs * 1000;
		SCB_DEMCR   |= CoreDebug_DEMCR_TRCENA_Msk; // разрешаем использовать счётчик
        DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk;   // запускаем счётчик
		Count = (&DWT->CYCCNT); // обнуляем счётчик
		*Count = 0;
	}

    //Используем Таймера 32bit и шина APB1 половина частоты
	void init(TIM_HandleTypeDef * htimer, bool timer16 = false)
	{

		timber.print("\x1B[38;05;0;48;05;11mDWT>");
		timber.print("\x1B[48;05;10m SystemCoreClock %d \33[0m\n", SystemCoreClock);

		bit16 = timer16;

		if (timer16)
		  tickToUs = 1;
		else

		  tickToUs = SystemCoreClock / 1000000 / 2; //получаем кол-во тактов за 1 мкс

		tickToMs = tickToUs * 1000;
		htim = htimer;
		volatile uint32_t * temp;
		temp = &htim->Instance->CNT;
		Count = temp;
		*Count = 0;
		HAL_TIM_Base_Start(htim);

	}

    //Используем Таймера 16bit и задание количество тиков на 1us
	void Loger(char * str)
	{
		result = *Count - 2; //Поправка на вызов
		timber.print("\33[38;05;0;48;05;11mDWT>\33[38;05;15;48;05;0m [ %s ]>", str);
		if (bit16) result = *Count + 2; //Вернем поправку на место
		uint32_t timeus =  result/ tickToUs;
		timber.print("\33[38;05;10;48;05;0m %lu Tick > %lu us \33[0m\n", result, timeus);
	}




};

#endif /* HISPEEDTIMER_H_ */


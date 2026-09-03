#ifndef UTILS_TIMBER_H_
#define UTILS_TIMBER_H_

#include "stdio.h"
#include "stdarg.h"
#include "string.h"

#if defined(TIMBER_UART)
#include "usart.h"
#elif defined(TIMBER_USB)
    #include "usb.h"
#elif defined(TIMBER_SPI)
    #include "spi.h"
#elif defined(TIMBER_RTT)
    #include "rtt.h"
#else

#endif

#define COLOR_WARNING "\33[38;05;11m"
#define COLOR_ERROR   "\33[38;05;9m"
#define COLOR_RESET   "\33[0m"

class classLog {
public:


/*---- init ----*/
#if defined(TIMBER_UART)
	void init(UART_HandleTypeDef *_huart) {  huart = _huart;  }
#elif defined(TIMBER_USB)
	void init() {	 }
#elif defined(TIMBER_SPI)
	void init() {	 }
#elif defined(TIMBER_RTT)
	void init() {	 }
#else
	void init() {	 }
#endif


	void clear(void) {
		print("\33[1m\n");
	}   //Очистка терминала

	void color(int color = 15);                //Задать цвет текста

	void bgcolor(int color = 0);               //Задать цвет фона

	void reset(void) {
		print("\33[0m");
	};

	void resetln(void) {
		print("\33[0m\n");
	};

	void setBold(void);
	void setItalic(void);
	void setUnderline(void);
	void setRevers(void);
	void setFlash(void);

	//---------------------------------------------------
	void print(char const *format) {
		HAL_UART_Transmit(huart, (uint8_t*) format, strlen(format), 1000);
	}

	template<typename ... Args> void print(char const *const format,
			Args const &... args) noexcept {
		sprintf(str, format, args ...);
		HAL_UART_Transmit(huart, (uint8_t*) str, strlen(str), 1000);
	}

	template<typename ... Args>
	void println(char const *const format, Args const &... args) noexcept {
		sprintf(str, format, args ...);
		strcat(str, "\n");
		HAL_UART_Transmit(huart, (uint8_t*) str, strlen(str), 1000);

	}

	void println(char const *format) {
		HAL_UART_Transmit(huart, (uint8_t*) format, strlen(format), 1000);
	}
	//---------------------------------------------------

	//Вывести строку с нужным цветом
	void colorString(int color, char const *const format);
	void colorString(int color, int bgcolor, char const *const format);
	void colorStringln(int color, char const *const format);
	void colorStringln(int color, int bgcolor, char const *const format);

	template<typename ... Args>
	void w(char const *const format, Args const &... args) noexcept {
		sprintf(str, format, args ...);
		colorStringln(11, str);
	}

	template<typename ... Args>
	void e(char const *const format, Args const &... args) noexcept {
		sprintf(str, format, args ...);
		colorStringln(9, str);
	}

	template<typename ... Args>
	void i(char const *const format, Args const &... args) noexcept {
		sprintf(str, format, args ...);
		colorStringln(45, str);
	}

	template<typename ... Args>
	void s(char const *const format, Args const &... args) noexcept {
		sprintf(str, format, args ...);
		colorStringln(10, str);
	}

	char str[256];

	UART_HandleTypeDef *huart;

	void w(char const *const format) {
		colorStringln(11, format);
	}
	void e(char const *const format) {
		colorStringln(9, format);
	}
	void i(char const *const format) {
		colorStringln(45, format);
	}
	void s(char const *const format) {
		colorStringln(10, format);
	}
};
#endif /* UTILS_LOGUART_H_ */

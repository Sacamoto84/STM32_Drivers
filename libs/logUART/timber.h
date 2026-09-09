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
#include "SEGGER_RTT.h"
#else

#endif

#define COLOR_WARNING "\33[38;05;11m"
#define COLOR_ERROR   "\33[38;05;9m"
#define COLOR_RESET   "\33[0m"



class Timber {
public:

	/*---- init ----*/
#if defined(TIMBER_UART)
	void init(UART_HandleTypeDef *_huart) {  huart = _huart;  }
#elif defined(TIMBER_USB)
	void init() {	 }
#elif defined(TIMBER_SPI)
	void init() {	 }
#elif defined(TIMBER_RTT)
	void init() {
		SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
	}

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
	}
	;

	void setBold(void);
	void setItalic(void);
	void setUnderline(void);
	void setRevers(void);
	void setFlash(void);

	//---------------------------------------------------
	void print(char const *format) {
#if defined(TIMBER_UART)
		HAL_UART_Transmit(huart, (uint8_t*) format, strlen(str), 1000);
#endif

#if defined(TIMBER_RTT)
		SEGGER_RTT_WriteString(0, format);
#endif
	}

	template<typename ... Args> void print(char const *const format,
			Args const &... args) noexcept {
		sprintf(str, format, args ...);

#if defined(TIMBER_UART)
		HAL_UART_Transmit(huart, (uint8_t*) str, strlen(str), 1000);
#endif

#if defined(TIMBER_RTT)
		SEGGER_RTT_WriteString(0, str);
#endif

	}

	template<typename ... Args>
	void println(char const *const format, Args const &... args) noexcept {
		sprintf(str, format, args ...);
		strcat(str, "\n");

#if defined(TIMBER_UART)
		HAL_UART_Transmit(huart, (uint8_t*) str, strlen(str), 1000);
#endif

#if defined(TIMBER_RTT)
		SEGGER_RTT_WriteString(0, str);
#endif

	}

	void println(char const *format) {

#if defined(TIMBER_UART)
		HAL_UART_Transmit(huart, (uint8_t*) format, strlen(str), 1000);
#endif

#if defined(TIMBER_RTT)
		SEGGER_RTT_WriteString(0, format);
#endif

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

	template<typename ... Args>
	void warning(char const *const format, Args const &... args) noexcept { w(format, args ...); }
	void warning(char const *const format) { w(format); }

	template<typename ... Args>
	void error(char const *const format, Args const &... args) noexcept { e(format, args ...); }
	void error(char const *const format) { e(format); }

	template<typename ... Args>
	void info(char const *const format, Args const &... args) noexcept { i(format, args ...); }
	void info(char const *const format) { i(format); }

	template<typename ... Args>
	void successful(char const *const format, Args const &... args) noexcept { s(format, args ...); }
	void successful(char const *const format) { s(format); }

private:
#if defined(TIMBER_UART)
	UART_HandleTypeDef *huart;
#endif

	char str[256];

};

extern Timber timber;

#endif /* UTILS_LOGUART_H_ */

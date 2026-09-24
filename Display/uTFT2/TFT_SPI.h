#ifndef TFT_SPI_H_
#define TFT_SPI_H_

#ifdef TFT_USE_SPI

#include "TFT_define.h"

#if defined(TFT_PLATFORM_STM32)

#include "main.h"
#include "spi.h"

#define CS_0    HAL_GPIO_WritePin(LCD->GPIO_CS, LCD->GPIO_Pin_CS, GPIO_PIN_RESET) //CS -> 0
#define CS_1    HAL_GPIO_WritePin(LCD->GPIO_CS, LCD->GPIO_Pin_CS, GPIO_PIN_SET) //CS -> 1

#define RESET_0 HAL_GPIO_WritePin(LCD->GPIO_RESET, LCD->GPIO_Pin_RESET, GPIO_PIN_RESET)
#define RESET_1 HAL_GPIO_WritePin(LCD->GPIO_RESET, LCD->GPIO_Pin_RESET, GPIO_PIN_SET)

#define CMD     HAL_GPIO_WritePin(LCD->GPIO_DC, LCD->GPIO_Pin_DC, GPIO_PIN_RESET)
#define DATA    HAL_GPIO_WritePin(LCD->GPIO_DC, LCD->GPIO_Pin_DC, GPIO_PIN_SET)

#else // TFT_PLATFORM_PICO

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#define CS_0    gpio_put(LCD->pin_cs, 0)
#define CS_1    gpio_put(LCD->pin_cs, 1)

#define RESET_0 gpio_put(LCD->pin_reset, 0)
#define RESET_1 gpio_put(LCD->pin_reset, 1)

#define CMD     gpio_put(LCD->pin_dc, 0)
#define DATA    gpio_put(LCD->pin_dc, 1)

#endif

 typedef struct                                                                   //│
 {

	    TFT_LCD_t * LCD;

	 	void init (TFT_LCD_t * _LCD)
	 	{
			LCD = _LCD;
			if (LCD == NULL) return;
#if defined(TFT_PLATFORM_PICO)
			if (LCD->baudrate == 0) LCD->baudrate = 31250000;
			spi_init(LCD->spi, LCD->baudrate);
			spi_set_format(LCD->spi, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
			gpio_set_function(LCD->pin_sck, GPIO_FUNC_SPI);
			gpio_set_function(LCD->pin_mosi, GPIO_FUNC_SPI);
			gpio_pull_up(LCD->pin_sck);
			gpio_init(LCD->pin_cs);
			gpio_set_dir(LCD->pin_cs, GPIO_OUT);
			gpio_put(LCD->pin_cs, 1);
			gpio_init(LCD->pin_dc);
			gpio_set_dir(LCD->pin_dc, GPIO_OUT);
			gpio_put(LCD->pin_dc, 1);
			if (LCD->pin_reset != 0xFF) {
				gpio_init(LCD->pin_reset);
				gpio_set_dir(LCD->pin_reset, GPIO_OUT);
				gpio_put(LCD->pin_reset, 1);
			}
#endif
		}

	 	//Сброс экрана
	 	void LCD_Reset(void) {
			if (LCD == NULL) return;
#if defined(TFT_PLATFORM_STM32)
	 		HAL_GPIO_WritePin(LCD->GPIO_RESET, LCD->GPIO_Pin_RESET, GPIO_PIN_SET);
	 		HAL_Delay(200);
	 		HAL_GPIO_WritePin(LCD->GPIO_RESET, LCD->GPIO_Pin_RESET, GPIO_PIN_RESET);
	 		HAL_Delay(200);
	 		HAL_GPIO_WritePin(LCD->GPIO_RESET, LCD->GPIO_Pin_RESET, GPIO_PIN_SET);
	 		HAL_Delay(50);
#else
			RESET_1;
			sleep_ms(200);
			RESET_0;
			sleep_ms(200);
			RESET_1;
			sleep_ms(50);
#endif
	 	}

	 	void SendData(uint8_t data) {
			if (LCD == NULL) return;
	 		DATA;
#if defined(TFT_PLATFORM_STM32)
	 		HAL_SPI_Transmit(LCD->hspi, &data, 1, 1);
#else
			spi_write_blocking(LCD->spi, &data, 1);
#endif
	 	}

	 	void SendCmd(uint8_t cmd) {
			if (LCD == NULL) return;
	 		CMD;
#if defined(TFT_PLATFORM_STM32)
	 		HAL_SPI_Transmit(LCD->hspi, &cmd, 1, 1);
#else
			spi_write_blocking(LCD->spi, &cmd, 1);
#endif
	 	}

	 	// Только для порта настроенного на 16 бит
	 	void SendCmd16(uint16_t cmd)
	 	{
			if (LCD == NULL) return;
	 	    CMD;
#if defined(TFT_PLATFORM_STM32)
			if (LCD->hspi == NULL || LCD->hspi->Instance == NULL) return;
	 	    __NOP();
	 		LCD->hspi->Instance->DR = cmd;
	 		uint32_t wait_start = HAL_GetTick();
	 		while( (LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0 ) {
	 			if (HAL_GetTick() - wait_start > 50) break;
	 		}
	 		wait_start = HAL_GetTick();
	 		while( (LCD->hspi->Instance->SR & SPI_FLAG_BSY) != 0 ) {
	 			if (HAL_GetTick() - wait_start > 50) break;
	 		}
#else
			if (LCD->spi == NULL) return;
			spi_write16_blocking(LCD->spi, &cmd, 1);
#endif
	 	}

	 	// Только для порта настроенного на 16 бит
	 	void SendData16(uint16_t data)
	 	{
			if (LCD == NULL) return;
	 	    DATA;
#if defined(TFT_PLATFORM_STM32)
			if (LCD->hspi == NULL || LCD->hspi->Instance == NULL) return;
	 	    __NOP();
	 		LCD->hspi->Instance->DR = data;
	 		uint32_t wait_start = HAL_GetTick();
	 		while( (LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0 ) {
	 			if (HAL_GetTick() - wait_start > 50) break;
	 		}
	 		wait_start = HAL_GetTick();
	 		while( (LCD->hspi->Instance->SR & SPI_FLAG_BSY) != 0 ) {
	 			if (HAL_GetTick() - wait_start > 50) break;
	 		}
#else
			if (LCD->spi == NULL) return;
			spi_write16_blocking(LCD->spi, &data, 1);
#endif
	 	}

	 #ifndef NOSPI16
	 	// Перевод SPI на 16 бит слово
	 	void Spi8to16(void)
	 	{
			if (LCD == NULL) return;
#if defined(TFT_PLATFORM_STM32)
			if (LCD->hspi == NULL || LCD->hspi->Instance == NULL) return;
	 		LCD->hspi->Instance->CR1 &= ~SPI_CR1_SPE; //OFF SPI
#if defined(SPI_CR1_DFF)
	 		LCD->hspi->Instance->CR1 |=  SPI_CR1_DFF;
#elif defined(SPI_CR2_DS)
	 		LCD->hspi->Instance->CR2 = (LCD->hspi->Instance->CR2 & ~SPI_CR2_DS)
	 				| (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_DS_3);
#endif
	 		LCD->hspi->Instance->CR1 |=  SPI_CR1_SPE; //ON  SPI
#else
			if (LCD->spi == NULL) return;
			spi_set_format(LCD->spi, 16, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
#endif
	 	}

	 	// Перевод SPI на 8 бит слово
	 	void Spi16to8(void)
	 	{
			if (LCD == NULL) return;
#if defined(TFT_PLATFORM_STM32)
			if (LCD->hspi == NULL || LCD->hspi->Instance == NULL) return;
	 	    LCD->hspi->Instance->CR1 &= ~SPI_CR1_SPE; //OFF SPI
#if defined(SPI_CR1_DFF)
	 		LCD->hspi->Instance->CR1 &= ~SPI_CR1_DFF;
#elif defined(SPI_CR2_DS)
	 		LCD->hspi->Instance->CR2 = (LCD->hspi->Instance->CR2 & ~SPI_CR2_DS)
	 				| (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2);
#endif
	 		LCD->hspi->Instance->CR1 |=  SPI_CR1_SPE; //ON  SPI
#else
			if (LCD->spi == NULL) return;
			spi_set_format(LCD->spi, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
#endif
	 	}
	 #endif

	 void LCD_Select   (void)   {
		if (LCD == NULL) return;
#if defined(TFT_PLATFORM_STM32)
	 	if (LCD->GPIO_CS != NULL)
	 		HAL_GPIO_WritePin(LCD->GPIO_CS, LCD->GPIO_Pin_CS, GPIO_PIN_RESET);
#else
		if (LCD->pin_cs != 0xFF)
			gpio_put(LCD->pin_cs, 0);
#endif
	 } //CS -> 0

	 void LCD_UnSelect (void)   {
		if (LCD == NULL) return;
#if defined(TFT_PLATFORM_STM32)
	 	if (LCD->GPIO_CS != NULL)
	 		HAL_GPIO_WritePin(LCD->GPIO_CS, LCD->GPIO_Pin_CS, GPIO_PIN_SET);
#else
		if (LCD->pin_cs != 0xFF)
			gpio_put(LCD->pin_cs, 1);
#endif
	 } //CS -> 1

 } TFT_SPI;

#endif /* TFT_USE_SPI */

#endif /* TFT_SPI_H_ */

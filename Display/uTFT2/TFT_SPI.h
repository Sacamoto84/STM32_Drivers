#ifndef TFT_SPI_H_
#define TFT_SPI_H_

#ifdef TFT_USE_SPI

#include "main.h"
#include "spi.h"

#define CS_0    HAL_GPIO_WritePin(LCD->GPIO_CS, LCD->GPIO_Pin_CS, GPIO_PIN_RESET) //CS -> 0
#define CS_1    HAL_GPIO_WritePin(LCD->GPIO_CS, LCD->GPIO_Pin_CS, GPIO_PIN_SET) //CS -> 1

#define RESET_0 HAL_GPIO_WritePin(LCD->GPIO_RESET, LCD->GPIO_Pin_RESET, GPIO_PIN_RESET)
#define RESET_1 HAL_GPIO_WritePin(LCD->GPIO_RESET, LCD->GPIO_Pin_RESET, GPIO_PIN_SET)

#define CMD     HAL_GPIO_WritePin(LCD->GPIO_DC, LCD->GPIO_Pin_DC, GPIO_PIN_RESET)
#define DATA    HAL_GPIO_WritePin(LCD->GPIO_DC, LCD->GPIO_Pin_DC, GPIO_PIN_SET)

 typedef struct                                                                   //│
 {

	    TFT_LCD_t * LCD;

	 	void init (TFT_LCD_t * _LCD)
	 	{LCD = _LCD;}

	 	//Сброс экрана
	 	void LCD_Reset(void) {
	 		HAL_GPIO_WritePin(LCD->GPIO_RESET, LCD->GPIO_Pin_RESET, GPIO_PIN_SET);
	 		HAL_Delay(200);
	 		HAL_GPIO_WritePin(LCD->GPIO_RESET, LCD->GPIO_Pin_RESET, GPIO_PIN_RESET);
	 		HAL_Delay(200);
	 		HAL_GPIO_WritePin(LCD->GPIO_RESET, LCD->GPIO_Pin_RESET, GPIO_PIN_SET);
	 		HAL_Delay(50);
	 	}

	 	void SendData(uint8_t data) {
	 		DATA;
	 		HAL_SPI_Transmit(LCD->hspi, &data, 1, 1);
	 	}

	 	void SendCmd(uint8_t cmd) {
	 		CMD;
	 		HAL_SPI_Transmit(LCD->hspi, &cmd, 1, 1);
	 	}

	 	// Только для порта настроенного на 16 бит
	 	void SendCmd16(uint16_t cmd)
	 	{
	 	    CMD;
	 	    __NOP();
	 		LCD->hspi->Instance->DR = cmd;
	 		while( (LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0 );
	 		while( (LCD->hspi->Instance->SR & SPI_FLAG_BSY) != 0 );
	 	}

	 	// Только для порта настроенного на 16 бит
	 	void SendData16(uint16_t data)
	 	{
	 	    DATA;
	 	    __NOP();
	 		LCD->hspi->Instance->DR = data;
	 		while( (LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0 );
	 		while( (LCD->hspi->Instance->SR & SPI_FLAG_BSY) != 0 );
	 	}

	 #ifndef NOSPI16
	 	// Перевод SPI на 16 бит слово
	 	void Spi8to16(void)
	 	{
	 		LCD->hspi->Instance->CR1 &= ~SPI_CR1_SPE; //OFF SPI
	 		LCD->hspi->Instance->CR1 |=  SPI_CR1_DFF;
	 		LCD->hspi->Instance->CR1 |=  SPI_CR1_SPE; //ON  SPI
	 	}

	 	// Перевод SPI на 8 бит слово
	 	void Spi16to8(void)
	 	{
	 	    LCD->hspi->Instance->CR1 &= ~SPI_CR1_SPE; //OFF SPI
	 		LCD->hspi->Instance->CR1 &= ~SPI_CR1_DFF;
	 		LCD->hspi->Instance->CR1 |=  SPI_CR1_SPE; //ON  SPI
	 	}
	 #endif

	 void LCD_Select   (void)   {
	 	HAL_GPIO_WritePin(LCD->GPIO_CS, LCD->GPIO_Pin_CS, GPIO_PIN_RESET);
	 } //CS -> 0

	 void LCD_UnSelect (void)   {
	 	HAL_GPIO_WritePin(LCD->GPIO_CS, LCD->GPIO_Pin_CS, GPIO_PIN_SET);
	 } //CS -> 1

 } TFT_SPI;

#endif /* TFT_USE_SPI */

#endif /* TFT_SPI_H_ */

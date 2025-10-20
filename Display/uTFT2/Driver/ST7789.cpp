#include "TFT_config.h"

#if defined(TFT_DRIVER_ST7789)

//#include "../TFT.h"
#include "TFT_Driver.h"
#include "../TFT_SPI.h"
#include "ST7789.h"

void TFT_Driver::ST7789_Init(void) {
	if (LCD->GPIO_CS != NULL) {
		CS_0;
	}

	if (LCD->GPIO_RESET != NULL) {
		SPI.LCD_Reset();
	}

	SPI.SendCmd(0x01);   //SWRESET
	HAL_Delay(150);

	SPI.SendCmd(0x11);   //SLPOUT
	HAL_Delay(150);

	SPI.SendCmd(0x3A);   //COLMOD RGB444(12bit) 0x03, RGB565(16bit) 0x05,
	SPI.SendData(0x05);  //RGB666(18bit) 0x06
	SPI.SendCmd(0x36);   //MADCTL
	SPI.SendData(0x08);  //0x08 B-G-R, 0x14 R-G-B  14

	SPI.SendCmd(ST77XX_GAMSET);               // ST77XX_GAMSET         0x26
	SPI.SendData(0x02); // Gamma curve 2 (G1.8)  ST77XX_DGMEN          0xBA
	//SPI.SendCmd(ST77XX_DGMEN);
	//SPI.SendData(0x04); // Enable gamma

	//SPI.SendCmd(0xe0);   // Positive Voltage Gamma Control
	//SPI.SendData(0xd0);
	//SPI.SendData(0x00);
	//SPI.SendData(0x03);
	//	SPI.SendData(0x08);
	//SPI.SendData(0x0a);
	//SPI.SendData(0x17);
	//SPI.SendData(0x2e);
	//SPI.SendData(0x44);
	//SPI.SendData(0x3f);
	//SPI.SendData(0x29);
	//SPI.SendData(0x10);
	//SPI.SendData(0x0e);
	//SPI.SendData(0x14);
	//SPI.SendData(0x18);

	//SPI.SendCmd(0xe1);   // Negative Voltage Gamma Control
	//See datasheet for more information
	//SPI.SendData(0xd0);
	//SPI.SendData(0x00);
	//SPI.SendData(0x03);
	//SPI.SendData(0x08);
	//SPI.SendData(0x0a);
	//SPI.SendData(0x17);
	//SPI.SendData(0x2e);
	//SPI.SendData(0x44);
	//SPI.SendData(0x3f);
	//SPI.SendData(0x29);
	//SPI.SendData(0x10);
	//SPI.SendData(0x0e);
	//SPI.SendData(0x14);
	//SPI.SendData(0x18);


	SPI.SendCmd(0xe0);   // Positive Voltage Gamma Control
	SPI.SendData(0xd0);
	SPI.SendData(0x04);
	SPI.SendData(0x0D);
	SPI.SendData(0x11);
	SPI.SendData(0x13);
	SPI.SendData(0x2b);
	SPI.SendData(0x3f);
	SPI.SendData(0x54);
	SPI.SendData(0x4c);
	SPI.SendData(0x18);
	SPI.SendData(0x0d);
	SPI.SendData(0x0b);
	SPI.SendData(0x1f);
	SPI.SendData(0x23);

	SPI.SendCmd(0xe1);   // Negative Voltage Gamma Control
	//See datasheet for more information
	SPI.SendData(0xd0);
	SPI.SendData(0x00);
	SPI.SendData(0x0C);
	SPI.SendData(0x11);
	SPI.SendData(0x13);
	SPI.SendData(0x2C);
	SPI.SendData(0x3F);
	SPI.SendData(0x44);
	SPI.SendData(0x51);
	SPI.SendData(0x2F);
	SPI.SendData(0x1F);
	SPI.SendData(0x1F);
	SPI.SendData(0x20);
	SPI.SendData(0x23);






	SPI.SendCmd(0x11); // Exit Sleep Mode
	//HAL_Delay(120);
	HAL_Delay(350);

	SPI.SendCmd(0x29); // Display on
	HAL_Delay(350); //HAL_Delay(120);

	SPI.SendCmd(0x21);   //INVON
	SPI.SendCmd(0x13);   //NORON
	SPI.SendCmd(0x29);   //DISPON

	if (LCD->GPIO_CS != NULL) {
		CS_1;
	}
}

void TFT_Driver::ST7789_Update(void)
{
	ST7789_Update(0, 0, LCD->TFT_WIDTH - 1, LCD->TFT_HEIGHT - 1);
}

void TFT_Driver::ST7789_Update(List_Update_Particle U)
{

	ST7789_Update( constrain(U.x0,0, LCD->TFT_WIDTH - 1) , constrain(U.y0,0,LCD->TFT_HEIGHT - 1), constrain(U.x1,0,LCD->TFT_WIDTH - 1) , constrain(U.y1,0,LCD->TFT_HEIGHT - 1));
}

void TFT_Driver::ST7789_Update(int x0, int y0, int x1, int y1) {

	if (blockUpdate) return;

	//uint16_t line_bufferActive[LCD->TFT_WIDTH];
	//uint16_t line_bufferNext[LCD->TFT_WIDTH];

	if (LCD->GPIO_CS != NULL) {
		CS_0;
	}

	int i;
	int ii;

    int fullscreen = 0;
    if ((x0 == 0) && (y0 == 0) && (x1 == LCD->TFT_WIDTH - 1) && (y1==LCD->TFT_HEIGHT - 1)) fullscreen = 1;


	SPI.SendCmd(0x2A);         // Column addr set
	SPI.SendData((LCD->dx + x0) >> 8);        // XSTART
	SPI.SendData((LCD->dx + x0) & 0xFF);      // XSTART
	SPI.SendData((LCD->dx + x1) >> 8);          // XEND
	SPI.SendData((LCD->dx + x1) & 0xFF);        // XEND
	SPI.SendCmd(0x2B); // Row addr set
	SPI.SendData((LCD->dy + y0) >> 8);
	SPI.SendData((LCD->dy + y0) & 0xFF); //TFT_YSTART
	SPI.SendData((LCD->dy + y1) >> 8);
	SPI.SendData((LCD->dy + y1) & 0xFF); //TFT_YSTART
	SPI.SendCmd(0x2C); //Memory write

	//LCD->hspi->Instance->CR1 |= SPI_CR1_DFF;
	SPI.Spi8to16();
	DATA;

	/////////////////Spi8to16(LCD); //16bit mode
	if (LCD->Bit == 4) {
		for (int i = 0; i < (LCD->TFT_HEIGHT * LCD->TFT_WIDTH) - 1; i++) {
			if (i % 2) {
				//while ((LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0);
				//LCD->hspi->Instance->DR = LCD->palete[(LCD->buffer8[i / 2]) & 0x0F]; //4 bit
				//while ((LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0);

				LCD->hspi->Instance->DR =  LCD->palete[(LCD->buffer8[i / 2]) & 0x0F]; // write data to be transmitted to the SPI data register
				while( !(SPI5->SR & SPI_FLAG_TXE) );  // wait until transmit complete
				while( !(SPI5->SR & SPI_FLAG_RXNE) ); // wait until receive complete
				while( SPI5->SR & SPI_FLAG_BSY ); // wait until SPI is not busy anymore
				LCD->hspi->Instance->DR; // return received data from SPI data register
			} else {
				//while ((LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0);
				//LCD->hspi->Instance->DR = LCD->palete[(LCD->buffer8[i / 2]) >> 4]; //4 bit

				LCD->hspi->Instance->DR =  LCD->palete[(LCD->buffer8[i / 2]) >> 4]; // write data to be transmitted to the SPI data register
				while( !(SPI5->SR & SPI_FLAG_TXE) );  // wait until transmit complete
				while( !(SPI5->SR & SPI_FLAG_RXNE) ); // wait until receive complete
				while( SPI5->SR & SPI_FLAG_BSY ); // wait until SPI is not busy anymore
				LCD->hspi->Instance->DR; // return received data from SPI data register
			}
		}
	}

	if (LCD->Bit == 16) {
		if (fullscreen)
		{
			int count = (LCD->TFT_HEIGHT * LCD->TFT_WIDTH);
			for (i = 0; i < count; i++) {
			  while (!(LCD->hspi->Instance->SR & SPI_SR_TXE));
			  LCD->hspi->Instance->DR = LCD->buffer16[i];
			}
		}
		else
		{
			uint16_t * p;
			for ( i = y0 ; i <= y1 ; i++)
			{
				p = &LCD->buffer16[0] + i * LCD->TFT_WIDTH + (long int)x0;

				for(ii = x0 ;  ii <= x1 ; ii++)
				{
					while (!(LCD->hspi->Instance->SR & SPI_SR_TXE));
					LCD->hspi->Instance->DR = *p++;
				}
			}
		}
	}

////	uTFT_DMA_completed = 0;
////
////	HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)LCD->buffer16[0], LCD->TFT_HEIGHT*LCD->TFT_WIDTH);
////
////	while (uTFT_DMA_completed == 0)
////	{
////	}
	while (!(LCD->hspi->Instance->SR & SPI_SR_TXE))
		;

	SPI.Spi16to8(); //8bit mode

	if (LCD->GPIO_CS != NULL) {
		CS_1;
	}

}

void TFT_Driver::ST7789_AddrSet(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	SPI.SendCmd(0x2A);
	SPI.SendData(0x00);
	SPI.SendData(x0 + LCD->dx);
	SPI.SendData(0x00);
	SPI.SendData(x1 + LCD->dx);
	SPI.SendCmd(0x2B);
	SPI.SendData(0x00);
	SPI.SendData(y0 + LCD->dy);
	SPI.SendData(0x00);
	SPI.SendData(y1 + LCD->dy);
	SPI.SendCmd(0x2C);
}

// Поворот экрана
void TFT_Driver::ST77XX_Rotate(uint16_t depth) {
	if (depth == 0) {
		LCD->MADCTL.MV = 0;
		LCD->MADCTL.MX = 0;
		LCD->MADCTL.MY = 0;
		LCD->dx = LCD->ROTATE_DELTA.DX_0;
		LCD->dy = LCD->ROTATE_DELTA.DY_0;
		LCD->TFT_WIDTH = LCD->ROTATE_DELTA.W_0;
		LCD->TFT_HEIGHT = LCD->ROTATE_DELTA.H_0;
	}
	if (depth == 90) {
		LCD->MADCTL.MV = 1;
		LCD->MADCTL.MX = 1;
		LCD->MADCTL.MY = 0;
		LCD->dx = LCD->ROTATE_DELTA.DX_90;
		LCD->dy = LCD->ROTATE_DELTA.DY_90;
		LCD->TFT_WIDTH = LCD->ROTATE_DELTA.W_90;
		LCD->TFT_HEIGHT = LCD->ROTATE_DELTA.H_90;
	}
	if (depth == 180) {
		LCD->MADCTL.MV = 0;
		LCD->MADCTL.MX = 1;
		LCD->MADCTL.MY = 1;
		LCD->dx = LCD->ROTATE_DELTA.DX_180;
		LCD->dy = LCD->ROTATE_DELTA.DY_180;
		LCD->TFT_WIDTH = LCD->ROTATE_DELTA.W_180;
		LCD->TFT_HEIGHT = LCD->ROTATE_DELTA.H_180;
	}
	if (depth == 270) {
		LCD->MADCTL.MV = 1;
		LCD->MADCTL.MX = 0;
		LCD->MADCTL.MY = 1;
		LCD->dx = LCD->ROTATE_DELTA.DX_270;
		LCD->dy = LCD->ROTATE_DELTA.DY_270;
		LCD->TFT_WIDTH = LCD->ROTATE_DELTA.W_270;
		LCD->TFT_HEIGHT = LCD->ROTATE_DELTA.H_270;
	}
	ST77XX_Update_MADCTL();
}

//Обновление регистра MADCTL
void TFT_Driver::ST77XX_Update_MADCTL(void) {
	uint8_t data;

	if (LCD->GPIO_CS != NULL)
		CS_0;

	//data = 0;
	data = (LCD->MADCTL.MY << 7) | (LCD->MADCTL.MX << 6) | (LCD->MADCTL.MV << 5)
			| (LCD->MADCTL.ML << 4) | (LCD->MADCTL.RGB << 3)
			| (LCD->MADCTL.MH << 2);
	SPI.SendCmd(0x36);
	SPI.SendData(data);

	if (LCD->GPIO_CS != NULL)
		CS_1;
}

void TFT_Driver::ST7789_Update_Window(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {

	if (blockUpdate) return;

	if (LCD->GPIO_CS != NULL) {
		CS_0;
	}

	uint8_t x1c = constrain(x1, 0, LCD->TFT_WIDTH - 1);
	uint8_t x2c = constrain(x2, 0, LCD->TFT_WIDTH - 1);

	uint8_t y1c = constrain(y1, 0, LCD->TFT_HEIGHT - 1);
	uint8_t y2c = constrain(y2, 0, LCD->TFT_HEIGHT - 1);

	int32_t x = 0;
	int32_t y, i;

	SPI.SendCmd(0x2A);     // Column addr set
	SPI.SendData(0);     //??????? ??? XSTART
	SPI.SendData(x1c + LCD->dx);     //??????? ??? XSTART
	SPI.SendData(0);     //???????  ??? XEND
	SPI.SendData(x2c + LCD->dx);    //??????? ??? XEND

	SPI.SendCmd(0x2B); // Row addr set
	SPI.SendData(0x00);
	SPI.SendData(y1c + LCD->dy); //TFT_YSTART
	SPI.SendData(0x00);
	SPI.SendData(y2c + LCD->dy); //TFT_YSTART

	SPI.SendCmd(0x2C); //Memory write

	//LCD->hspi->Instance->CR1 |= SPI_CR1_DFF;
	SPI.Spi8to16();

	DATA;

	//uint32_t count = (x2c - x1c) * (y2c - y1c);

	/////////////////Spi8to16(LCD); //16bit mode

	if (LCD->Bit == 4) {
		for (i = 0; i < (LCD->TFT_HEIGHT * LCD->TFT_WIDTH) - 1; i++) {
			if (i % 2) {
				//while ((LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0);
				//LCD->hspi->Instance->DR = LCD->palete[(LCD->buffer8[i / 2])& 0x0F]; //4 bit

				//SPI1->DR = data; // write data to be transmitted to the SPI data register
				//while( !(SPI1->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
				//while( !(SPI1->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
				//while( SPI1->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
				//return SPI1->DR; // return received data from SPI data register






			} else {
				//while ((LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0);
				//LCD->hspi->Instance->DR = LCD->palete[(LCD->buffer8[i / 2]) >> 4]; //4 bit
			}
		}
	}

	if (LCD->Bit == 16) {
		uint16_t *p;

		for (y = y1c; y < y2c; y++) {
			p = &LCD->buffer16[0] + y * LCD->TFT_WIDTH + x1c;
			for (x = 0; x < (x2c - x1c) + 1; x++) {

				while (!(LCD->hspi->Instance->SR & SPI_SR_TXE))
					;
				LCD->hspi->Instance->DR = *p++;

			}
		}

	}

////	uTFT_DMA_completed = 0;
////
////	HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)LCD->buffer16[0], LCD->TFT_HEIGHT*LCD->TFT_WIDTH);
////
////	while (uTFT_DMA_completed == 0)
////	{
////	}
	while (!(LCD->hspi->Instance->SR & SPI_SR_TXE))
		;

	SPI.Spi16to8(); //8bit mode

	if (LCD->GPIO_CS != NULL) {
		CS_1;
	}
}

void TFT_Driver::ST7789_UpdateDMA4bit(void) {

	if (blockUpdate) return;

	uint16_t line_buffer0[LCD->TFT_WIDTH];
	//uint16_t line_buffer1[LCD->TFT_WIDTH];

	if (LCD->GPIO_CS != NULL) {
		CS_0;
	}

	//int32_t i = 0;
	uint8_t HI;
	uint8_t LO;

	HI = (LCD->dx + LCD->TFT_WIDTH - 1) >> 8;
	LO = (LCD->dx + LCD->TFT_WIDTH - 1) & 0xFF;

	SPI.SendCmd(0x2A);
	SPI.SendData(0x00);
	SPI.SendData(LCD->dx);
	SPI.SendData(HI);
	SPI.SendData(LO);
	SPI.SendCmd(0x2B);
	SPI.SendData(0x00);
	SPI.SendData(LCD->dy);
	SPI.SendData(0x00);
	SPI.SendData(LCD->dy + LCD->TFT_HEIGHT - 1);
	SPI.SendCmd(0x2C); //Memory write

	SPI.Spi8to16();

	DATA;
	/////////////////Spi8to16(LCD); //16bit mode

	DMA_TX_Complete = 0;

	uint32_t index = 0; // ������ �� 0 �� ����� �����

	for (uint16_t index_line = 0; index_line < LCD->TFT_HEIGHT; index_line++) {

		for (int32_t row = 0; row < LCD->TFT_WIDTH; row++) {

			if (index % 2) {
				line_buffer0[row] =	LCD->palete[(LCD->buffer8[index / 2]) & 0x0F]; //4 bit
			} else {
				line_buffer0[row] = LCD->palete[(LCD->buffer8[index / 2]) >> 4]; //4 bit
			}
			index++;
		}

		HAL_SPI_Transmit_DMA(LCD->hspi, (uint8_t*)line_buffer0, LCD->TFT_WIDTH);
		while(DMA_TX_Complete == 0){__NOP();};
		DMA_TX_Complete = 0;
	}

	SPI.Spi16to8(); //8bit mode
	if (LCD->GPIO_CS != NULL) {
		CS_1;
	}

}

void TFT_Driver::ST7789_UpdateDMA4bitV2(void) {

	if (blockUpdate) return;

	needUpdate = 0;

	uint16_t line_buffer0[LCD->TFT_WIDTH];
	uint16_t line_buffer1[LCD->TFT_WIDTH];

	if (LCD->GPIO_CS != NULL) {
		CS_0;
	}

	//int32_t i = 0;
	uint8_t HI;
	uint8_t LO;

	HI = (LCD->dx + LCD->TFT_WIDTH - 1) >> 8;
	LO = (LCD->dx + LCD->TFT_WIDTH - 1) & 0xFF;
	SPI.SendCmd(0x2A);
	SPI.SendData(0x00);
	SPI.SendData(LCD->dx);
	SPI.SendData(HI);
	SPI.SendData(LO);
	SPI.SendCmd(0x2B);
	SPI.SendData(0x00);
	SPI.SendData(LCD->dy);
	SPI.SendData(0x00);
	SPI.SendData(LCD->dy + LCD->TFT_HEIGHT - 1);
	SPI.SendCmd(0x2C); //Memory write

	SPI.Spi8to16();

	DATA;
	/////////////////Spi8to16(LCD); //16bit mode

	DMA_TX_Complete = 1;

	uint32_t index = 0; // ������ �� 0 �� ����� �����

	for (uint16_t index_line = 0; index_line < LCD->TFT_HEIGHT; ) {

		for (int32_t row = 0; row < LCD->TFT_WIDTH; row++) {

			if (index % 2) {
				line_buffer0[row] =	LCD->palete[(LCD->buffer8[index / 2]) & 0x0F]; //4 bit
			} else {
				line_buffer0[row] = LCD->palete[(LCD->buffer8[index / 2]) >> 4]; //4 bit
			}
			index++;
		}

		while(DMA_TX_Complete == 0){__NOP();};
				DMA_TX_Complete = 0;

		HAL_SPI_Transmit_DMA(LCD->hspi, (uint8_t*)line_buffer0, LCD->TFT_WIDTH);
	    //� ��� ����� �������� � ������ 1
		index_line++;
		for (int16_t row = 0; row < LCD->TFT_WIDTH; row++) {

			if (index % 2) {
				line_buffer1[row] =	LCD->palete[(LCD->buffer8[index / 2]) & 0x0F]; //4 bit
			} else {
				line_buffer1[row] = LCD->palete[(LCD->buffer8[index / 2]) >> 4]; //4 bit
			}
			index++;
		}


		while(DMA_TX_Complete == 0){__NOP();};
		DMA_TX_Complete = 0;


		HAL_SPI_Transmit_DMA(LCD->hspi, (uint8_t*)line_buffer1, LCD->TFT_WIDTH);
		index_line++;
	}


	while(DMA_TX_Complete == 0){__NOP();};
			DMA_TX_Complete = 0;

	SPI.Spi16to8(); //8bit mode
	if (LCD->GPIO_CS != NULL) {
		CS_1;
	}

}

void TFT_Driver::ST7789_UpdateDMA8bitV2(void) {

	if (blockUpdate) return;

	needUpdate = 0;

	uint16_t line_buffer0[LCD->TFT_WIDTH];
	uint16_t line_buffer1[LCD->TFT_WIDTH];

	if (LCD->GPIO_CS != NULL) {
		CS_0;
	}

	//int32_t i = 0;
	uint8_t HI;
	uint8_t LO;

	HI = (LCD->dx + LCD->TFT_WIDTH - 1) >> 8;
	LO = (LCD->dx + LCD->TFT_WIDTH - 1) & 0xFF;
	SPI.SendCmd(0x2A);
	SPI.SendData(0x00);
	SPI.SendData(LCD->dx);
	SPI.SendData(HI);
	SPI.SendData(LO);
	SPI.SendCmd(0x2B);
	SPI.SendData(0x00);
	SPI.SendData(LCD->dy);
	SPI.SendData(0x00);
	SPI.SendData(LCD->dy + LCD->TFT_HEIGHT - 1);
	SPI.SendCmd(0x2C); //Memory write

	SPI.Spi8to16();

	DATA;
	/////////////////Spi8to16(LCD); //16bit mode

	DMA_TX_Complete = 1;

	uint32_t index = 0; // ������ �� 0 �� ����� �����

	for (uint16_t index_line = 0; index_line < LCD->TFT_HEIGHT; ) {

		for (int16_t row = 0; row < LCD->TFT_WIDTH; row++) {
				line_buffer0[row] =	LCD->palete[LCD->buffer8[index]]; //8 bit
			index++;
		}

		while(DMA_TX_Complete == 0){__NOP();};
				DMA_TX_Complete = 0;

		HAL_SPI_Transmit_DMA(LCD->hspi, (uint8_t*)line_buffer0, LCD->TFT_WIDTH);
	    //� ��� ����� �������� � ������ 1
		index_line++;
		for (int16_t row = 0; row < LCD->TFT_WIDTH; row++) {
				line_buffer1[row] =	LCD->palete[LCD->buffer8[index]]; //8 bit
			index++;
		}

		while(DMA_TX_Complete == 0){__NOP();};
		DMA_TX_Complete = 0;


		HAL_SPI_Transmit_DMA(LCD->hspi, (uint8_t*)line_buffer1, LCD->TFT_WIDTH);
		index_line++;
	}


	while(DMA_TX_Complete == 0){__NOP();};
			DMA_TX_Complete = 0;

	SPI.Spi16to8(); //8bit mode
	if (LCD->GPIO_CS != NULL) {
		CS_1;
	}

}

extern uint16_t LCD_Buffer16[240 * 240];
void TFT_Driver::ST7789_UpdateDMA16bitV2(void) {

	if (blockUpdate) return;

	    needUpdate = 0;

		uint16_t line_buffer0[LCD->TFT_WIDTH];
		uint16_t line_buffer1[LCD->TFT_WIDTH];

		if (LCD->GPIO_CS != NULL) {
			CS_0;
		}

		//int32_t i = 0;
		uint8_t HI;
		uint8_t LO;

		HI = (LCD->dx + LCD->TFT_WIDTH - 1) >> 8;
		LO = (LCD->dx + LCD->TFT_WIDTH - 1) & 0xFF;
		SPI.SendCmd(0x2A);
		SPI.SendData(0x00);
		SPI.SendData(LCD->dx);
		SPI.SendData(HI);
		SPI.SendData(LO);
		SPI.SendCmd(0x2B);
		SPI.SendData(0x00);
		SPI.SendData(LCD->dy);
		SPI.SendData(0x00);
		SPI.SendData(LCD->dy + LCD->TFT_HEIGHT - 1);
		SPI.SendCmd(0x2C); //Memory write

		SPI.Spi8to16();

		DATA;
		/////////////////Spi8to16(LCD); //16bit mode

		DMA_TX_Complete = 1;

		uint32_t index = 0; // ������ �� 0 �� ����� �����

		for (uint16_t index_line = 0; index_line < LCD->TFT_HEIGHT; ) {

			for (int16_t row = 0; row < LCD->TFT_WIDTH; row++) {
					line_buffer0[row] =	LCD->buffer16[index]; //8 bit
				index++;
			}

			while(DMA_TX_Complete == 0){__NOP();};
					DMA_TX_Complete = 0;

			HAL_SPI_Transmit_DMA(LCD->hspi, (uint8_t*)line_buffer0, LCD->TFT_WIDTH);
		    //� ��� ����� �������� � ������ 1
			index_line++;
			for (int16_t row = 0; row < LCD->TFT_WIDTH; row++) {
					line_buffer1[row] =	LCD->buffer16[index]; //8 bit
				index++;
			}

			while(DMA_TX_Complete == 0){__NOP();};
			DMA_TX_Complete = 0;


			HAL_SPI_Transmit_DMA(LCD->hspi, (uint8_t*)line_buffer1, LCD->TFT_WIDTH);
			index_line++;
		}


		while(DMA_TX_Complete == 0){__NOP();};
				DMA_TX_Complete = 0;

		SPI.Spi16to8(); //8bit mode
		if (LCD->GPIO_CS != NULL) {
			CS_1;
		}

}

//DMA с блокировкой
void TFT_Driver::ST7789_UpdateDMA16bitV3(void) {

	if (blockUpdate) return;

	needUpdate = 0;

		if (LCD->GPIO_CS != NULL) {
			CS_0;
		}

		//int32_t i = 0;
		uint8_t HI;
		uint8_t LO;

		HI = (LCD->dx + LCD->TFT_WIDTH - 1) >> 8;
		LO = (LCD->dx + LCD->TFT_WIDTH - 1) & 0xFF;
		SPI.SendCmd(0x2A);
		SPI.SendData(0x00);
		SPI.SendData(LCD->dx);
		SPI.SendData(HI);
		SPI.SendData(LO);
		SPI.SendCmd(0x2B);
		SPI.SendData(0x00);
		SPI.SendData(LCD->dy);
		SPI.SendData(0x00);
		SPI.SendData(LCD->dy + LCD->TFT_HEIGHT - 1);
		SPI.SendCmd(0x2C); //Memory write

		SPI.Spi8to16();

		DATA;
		/////////////////Spi8to16(LCD); //16bit mode

		DMA_TX_Complete = 0;

		HAL_SPI_Transmit_DMA(LCD->hspi, (uint8_t*)LCD->buffer16, LCD->TFT_WIDTH * LCD->TFT_HEIGHT);

		while(DMA_TX_Complete == 0){__NOP();};
				DMA_TX_Complete = 0;

		SPI.Spi16to8(); //8bit mode
		if (LCD->GPIO_CS != NULL) {
			CS_1;
		}

}

void TFT_Driver::ST7789_Transmit_Array(char dc, uint8_t *data, int nbytes)
{
	if (dc)
		DATA;
	else
		CMD;

	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();


	HAL_SPI_Transmit(LCD->hspi, (uint8_t *)data, nbytes, 1000);
}


void TFT_Driver::ST7789_Update_DMA_Cicle_On(void)
{

	DMA2_Stream3->CR &= ~DMA_SxCR_EN;     //Отключаем DMA
    while( !(SPI1->SR & SPI_SR_TXE));	  //Ждем окончания передачи по SPI
	DMA2_Stream3->NDTR = 0;               //Сброс счетчика DMA
	SPI1->CR1 &= ~SPI_CR1_SPE;            //Спокойно отключаем SPI
	SPI1->CR2  &= ~SPI_CR2_TXDMAEN;       //Отвязываем от DMA
	SPI1->CR1 &= ~SPI_CR1_DFF;            //8bit mode
	SPI1->CR1 |= SPI_CR1_SPE;             //Включаем для работы в обычном режиме


	//int32_t i = 0;
	uint8_t HI;
	uint8_t LO;

	HI = (LCD->dx + LCD->TFT_WIDTH - 1) >> 8;
	LO = (LCD->dx + LCD->TFT_WIDTH - 1) & 0xFF;
	SPI.SendCmd(0x2A);
	SPI.SendData(0x00);
	SPI.SendData(LCD->dx);
	SPI.SendData(HI);
	SPI.SendData(LO);
	SPI.SendCmd(0x2B);
	SPI.SendData(0x00);
	SPI.SendData(LCD->dy);
	SPI.SendData(0x00);
	SPI.SendData(LCD->dy + LCD->TFT_HEIGHT - 1);
	SPI.SendCmd(0x2C); //Memory write

//	
//	while( !(SPI1->SR & SPI_SR_TXE));       //Ждем окончания передачи по SPI
//	SPI1->CR1 &= ~SPI_CR1_SPE;              //Спокойно отключаем SPI
//	SPI1->CR1 |= SPI_CR1_DFF;               //16bit mode
//	
	DATA;

	SPI.Spi8to16();

	DMA2_Stream3->CR   &= ~DMA_SxCR_EN; // DMA
	DMA2_Stream3->NDTR  = 240*135;
	DMA2_Stream3->PAR   = 0x4001300C;//SPI
	DMA2_Stream3->M0AR  = (uint32_t)&LCD->buffer16[0];
	DMA2_Stream3->CR   |=  DMA_SxCR_CIRC;      //Кольцевой режим
	DMA2_Stream3->CR   |=  DMA_SxCR_EN;        //Включаем DMA

	SPI1->CR2 |= SPI_CR2_TXDMAEN;              //SPI на DMA
	SPI1->CR1 |= SPI_CR1_SPE;                  //Включаем SPI для работы в DMA
}

void TFT_Driver::ST7789_Update_DMA_Cicle_Off(void)
{
	DMA2_Stream3->CR &= ~DMA_SxCR_EN;     //Отключаем DMA
    while( !(SPI1->SR & SPI_SR_TXE));	  //Ждем окончания передачи по SPI
	DMA2_Stream3->NDTR = 0;               //Сброс счетчика DMA
	DMA2_Stream3->CR &= ~DMA_SxCR_CIRC;   //Выкл Кольцевой режим

	SPI1->CR1 &= ~SPI_CR1_SPE;            //Спокойно отключаем SPI
	SPI1->CR2 &= ~SPI_CR2_TXDMAEN;        //Отвязываем от DMA
	SPI1->CR1 &= ~SPI_CR1_DFF;            //8bit mode
	SPI1->CR1 |= SPI_CR1_SPE;             //Включаем для работы в обычном режиме
}

#endif /* TFT_Driver_ST7789 */

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

	//Клиппинг окна
	if (x0 < 0) x0 = 0;
	if (y0 < 0) y0 = 0;
	if (x1 >= LCD->TFT_WIDTH)  x1 = LCD->TFT_WIDTH - 1;
	if (y1 >= LCD->TFT_HEIGHT) y1 = LCD->TFT_HEIGHT - 1;
	if (x0 > x1 || y0 > y1) return;

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
		//Отправляем только пиксели заданного окна.
		//Старший ниббель - четный x, младший - нечетный (см. SetPixel4)
		uint32_t stride = ((uint32_t)LCD->TFT_WIDTH + 1) / 2;
		for (int32_t y = y0; y <= y1; y++) {
			for (int32_t x = x0; x <= x1; x++) {
				uint8_t b = LCD->buffer8[y * stride + x / 2];
				uint16_t c = (x & 1) ? LCD->palete[b & 0x0F] : LCD->palete[b >> 4];
				LCD->hspi->Instance->DR = c;
				while (!(LCD->hspi->Instance->SR & SPI_FLAG_TXE));
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

	//Ждем фактического завершения передачи (TXE + BSY),
	//иначе переключение DFF/выключение SPI оборвет последнее слово
	while (!(LCD->hspi->Instance->SR & SPI_SR_TXE));
	while (LCD->hspi->Instance->SR & SPI_SR_BSY);

	SPI.Spi16to8(); //8bit mode

	if (LCD->GPIO_CS != NULL) {
		CS_1;
	}

}

void TFT_Driver::ST7789_AddrSet(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	//SendData(uint8_t) обрезает значения >255 - шлем старший и младший байты,
	//как в ST7789_Update (иначе dx/dy или координаты > 255 портят окно)
	SPI.SendCmd(0x2A);
	SPI.SendData((x0 + LCD->dx) >> 8);
	SPI.SendData((x0 + LCD->dx) & 0xFF);
	SPI.SendData((x1 + LCD->dx) >> 8);
	SPI.SendData((x1 + LCD->dx) & 0xFF);
	SPI.SendCmd(0x2B);
	SPI.SendData((y0 + LCD->dy) >> 8);
	SPI.SendData((y0 + LCD->dy) & 0xFF);
	SPI.SendData((y1 + LCD->dy) >> 8);
	SPI.SendData((y1 + LCD->dy) & 0xFF);
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
	//Делегируем основному обновлению: внутри клиппинг, поддержка 4/16 бит,
	//ожидание BSY перед сменой DFF
	ST7789_Update((int)x1, (int)y1, (int)x2, (int)y2);
}

// Общие буферы линии для DMA-обновлений (вместо VLA на стеке).
// Функции DMA-обновления блокирующие и не реентерабельные, поэтому совместное использование безопасно.
#ifndef TFT_DMA_LINE_BUF_MAX
#define TFT_DMA_LINE_BUF_MAX 512
#endif
static uint16_t DMA_line_buffer0[TFT_DMA_LINE_BUF_MAX];
static uint16_t DMA_line_buffer1[TFT_DMA_LINE_BUF_MAX];

//Ожидание завершения предыдущей DMA-строки и запуск новой.
//Возвращает статус HAL: при ошибке (HAL_BUSY и т.п.) передача не начата,
//вызывающий обязан завершить обновление, не зависая в ожидании флага.
HAL_StatusTypeDef TFT_Driver::ST7789_DMA_WaitAndSend(uint16_t *line) {
	while (DMA_TX_Complete == 0) { __NOP(); }
	DMA_TX_Complete = 0;
	return HAL_SPI_Transmit_DMA(LCD->hspi, (uint8_t*)line, LCD->TFT_WIDTH);
}

void TFT_Driver::ST7789_UpdateDMA4bitV2(void) {

	if (blockUpdate) return;

	needUpdate = 0;

	if (LCD->TFT_WIDTH > TFT_DMA_LINE_BUF_MAX) return; //Экран шире буфера линии
	uint16_t *line_buffer0 = DMA_line_buffer0;
	uint16_t *line_buffer1 = DMA_line_buffer1;

	if (LCD->GPIO_CS != NULL) {
		CS_0;
	}

	//int32_t i = 0;
	SPI.SendCmd(0x2A);
	SPI.SendData(LCD->dx >> 8);
	SPI.SendData(LCD->dx & 0xFF);
	SPI.SendData((LCD->dx + LCD->TFT_WIDTH - 1) >> 8);
	SPI.SendData((LCD->dx + LCD->TFT_WIDTH - 1) & 0xFF);
	SPI.SendCmd(0x2B);
	SPI.SendData(LCD->dy >> 8);
	SPI.SendData(LCD->dy & 0xFF);
	SPI.SendData((LCD->dy + LCD->TFT_HEIGHT - 1) >> 8);
	SPI.SendData((LCD->dy + LCD->TFT_HEIGHT - 1) & 0xFF);
	SPI.SendCmd(0x2C); //Memory write

	SPI.Spi8to16();

	DATA;
	/////////////////Spi8to16(LCD); //16bit mode

	DMA_TX_Complete = 1; //Активных передач нет - первый wait пройдет сразу

	uint32_t index = 0; //Индекс пикселя от 0 до W*H-1
	HAL_StatusTypeDef st = HAL_OK;

	for (uint16_t index_line = 0; index_line < LCD->TFT_HEIGHT; ) {

		for (int32_t row = 0; row < LCD->TFT_WIDTH; row++) {

			if (index % 2) {
				line_buffer0[row] =	LCD->palete[(LCD->buffer8[index / 2]) & 0x0F]; //4 bit
			} else {
				line_buffer0[row] = LCD->palete[(LCD->buffer8[index / 2]) >> 4]; //4 bit
			}
			index++;
		}

		st = ST7789_DMA_WaitAndSend(line_buffer0);
		if (st != HAL_OK) break;
	    //Пока DMA шлет буфер 0 - готовим буфер 1
		index_line++;
		//Нечетная высота: последняя строка уже отправлена, выходим без
		//чтения лишней строки за концом фреймбуфера
		if (index_line >= LCD->TFT_HEIGHT) break;

		for (int16_t row = 0; row < LCD->TFT_WIDTH; row++) {

			if (index % 2) {
				line_buffer1[row] =	LCD->palete[(LCD->buffer8[index / 2]) & 0x0F]; //4 bit
			} else {
				line_buffer1[row] = LCD->palete[(LCD->buffer8[index / 2]) >> 4]; //4 bit
			}
			index++;
		}

		st = ST7789_DMA_WaitAndSend(line_buffer1);
		if (st != HAL_OK) break;
		index_line++;
	}

	//Финальное ожидание: DMA завершено != SPI завершен. Ждем TXE+BSY,
	//иначе переключение DFF оборвет последнее слово
	while (DMA_TX_Complete == 0) { __NOP(); }
	if (st == HAL_OK) {
		while (!(LCD->hspi->Instance->SR & SPI_SR_TXE));
		while (LCD->hspi->Instance->SR & SPI_SR_BSY);
	}

	SPI.Spi16to8(); //8bit mode
	if (LCD->GPIO_CS != NULL) {
		CS_1;
	}

}

void TFT_Driver::ST7789_UpdateDMA8bitV2(void) {

	if (blockUpdate) return;

	needUpdate = 0;

	if (LCD->TFT_WIDTH > TFT_DMA_LINE_BUF_MAX) return; //Экран шире буфера линии
	uint16_t *line_buffer0 = DMA_line_buffer0;
	uint16_t *line_buffer1 = DMA_line_buffer1;

	if (LCD->GPIO_CS != NULL) {
		CS_0;
	}

	//int32_t i = 0;
	SPI.SendCmd(0x2A);
	SPI.SendData(LCD->dx >> 8);
	SPI.SendData(LCD->dx & 0xFF);
	SPI.SendData((LCD->dx + LCD->TFT_WIDTH - 1) >> 8);
	SPI.SendData((LCD->dx + LCD->TFT_WIDTH - 1) & 0xFF);
	SPI.SendCmd(0x2B);
	SPI.SendData(LCD->dy >> 8);
	SPI.SendData(LCD->dy & 0xFF);
	SPI.SendData((LCD->dy + LCD->TFT_HEIGHT - 1) >> 8);
	SPI.SendData((LCD->dy + LCD->TFT_HEIGHT - 1) & 0xFF);
	SPI.SendCmd(0x2C); //Memory write

	SPI.Spi8to16();

	DATA;
	/////////////////Spi8to16(LCD); //16bit mode

	DMA_TX_Complete = 1; //Активных передач нет - первый wait пройдет сразу

	uint32_t index = 0; //Индекс пикселя от 0 до W*H-1
	HAL_StatusTypeDef st = HAL_OK;

	for (uint16_t index_line = 0; index_line < LCD->TFT_HEIGHT; ) {

		for (int16_t row = 0; row < LCD->TFT_WIDTH; row++) {
				line_buffer0[row] =	LCD->palete[LCD->buffer8[index]]; //8 bit
			index++;
		}

		st = ST7789_DMA_WaitAndSend(line_buffer0);
		if (st != HAL_OK) break;
	    //Пока DMA шлет буфер 0 - готовим буфер 1
		index_line++;
		//Нечетная высота: последняя строка уже отправлена
		if (index_line >= LCD->TFT_HEIGHT) break;

		for (int16_t row = 0; row < LCD->TFT_WIDTH; row++) {
				line_buffer1[row] =	LCD->palete[LCD->buffer8[index]]; //8 bit
			index++;
		}

		st = ST7789_DMA_WaitAndSend(line_buffer1);
		if (st != HAL_OK) break;
		index_line++;
	}

	//Финальное ожидание: DMA завершено != SPI завершен
	while (DMA_TX_Complete == 0) { __NOP(); }
	if (st == HAL_OK) {
		while (!(LCD->hspi->Instance->SR & SPI_SR_TXE));
		while (LCD->hspi->Instance->SR & SPI_SR_BSY);
	}

	SPI.Spi16to8(); //8bit mode
	if (LCD->GPIO_CS != NULL) {
		CS_1;
	}

}

void TFT_Driver::ST7789_UpdateDMA16bitV2(void) {

	if (blockUpdate) return;

	    needUpdate = 0;

		if (LCD->TFT_WIDTH > TFT_DMA_LINE_BUF_MAX) return; //Экран шире буфера линии
		uint16_t *line_buffer0 = DMA_line_buffer0;
		uint16_t *line_buffer1 = DMA_line_buffer1;

		if (LCD->GPIO_CS != NULL) {
			CS_0;
		}

		//int32_t i = 0;
		SPI.SendCmd(0x2A);
		SPI.SendData(LCD->dx >> 8);
		SPI.SendData(LCD->dx & 0xFF);
		SPI.SendData((LCD->dx + LCD->TFT_WIDTH - 1) >> 8);
		SPI.SendData((LCD->dx + LCD->TFT_WIDTH - 1) & 0xFF);
		SPI.SendCmd(0x2B);
		SPI.SendData(LCD->dy >> 8);
		SPI.SendData(LCD->dy & 0xFF);
		SPI.SendData((LCD->dy + LCD->TFT_HEIGHT - 1) >> 8);
		SPI.SendData((LCD->dy + LCD->TFT_HEIGHT - 1) & 0xFF);
		SPI.SendCmd(0x2C); //Memory write

		SPI.Spi8to16();

		DATA;
		/////////////////Spi8to16(LCD); //16bit mode

		DMA_TX_Complete = 1; //Активных передач нет - первый wait пройдет сразу

		uint32_t index = 0; //Индекс пикселя от 0 до W*H-1
		HAL_StatusTypeDef st = HAL_OK;

		for (uint16_t index_line = 0; index_line < LCD->TFT_HEIGHT; ) {

			for (int16_t row = 0; row < LCD->TFT_WIDTH; row++) {
					line_buffer0[row] =	LCD->buffer16[index]; //16 bit
				index++;
			}

			st = ST7789_DMA_WaitAndSend(line_buffer0);
			if (st != HAL_OK) break;
		    //Пока DMA шлет буфер 0 - готовим буфер 1
			index_line++;
			//Нечетная высота: последняя строка уже отправлена
			if (index_line >= LCD->TFT_HEIGHT) break;

			for (int16_t row = 0; row < LCD->TFT_WIDTH; row++) {
					line_buffer1[row] =	LCD->buffer16[index]; //16 bit
				index++;
			}

			st = ST7789_DMA_WaitAndSend(line_buffer1);
			if (st != HAL_OK) break;
			index_line++;
		}

		//Финальное ожидание: DMA завершено != SPI завершен
		while (DMA_TX_Complete == 0) { __NOP(); }
		if (st == HAL_OK) {
			while (!(LCD->hspi->Instance->SR & SPI_SR_TXE));
			while (LCD->hspi->Instance->SR & SPI_SR_BSY);
		}

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
		SPI.SendCmd(0x2A);
		SPI.SendData(LCD->dx >> 8);
		SPI.SendData(LCD->dx & 0xFF);
		SPI.SendData((LCD->dx + LCD->TFT_WIDTH - 1) >> 8);
		SPI.SendData((LCD->dx + LCD->TFT_WIDTH - 1) & 0xFF);
		SPI.SendCmd(0x2B);
		SPI.SendData(LCD->dy >> 8);
		SPI.SendData(LCD->dy & 0xFF);
		SPI.SendData((LCD->dy + LCD->TFT_HEIGHT - 1) >> 8);
		SPI.SendData((LCD->dy + LCD->TFT_HEIGHT - 1) & 0xFF);
		SPI.SendCmd(0x2C); //Memory write

		SPI.Spi8to16();

		DATA;
		/////////////////Spi8to16(LCD); //16bit mode

		//HAL_SPI_Transmit_DMA принимает Size как uint16_t, а NDTR DMA тоже
		//16-битный. Для 240x320 (76800 слов) старый код молча усекал размер.
		//Передаем кадр кусками по 65535 слов.
		uint32_t total = (uint32_t)LCD->TFT_WIDTH * LCD->TFT_HEIGHT;
		uint16_t *p = &LCD->buffer16[0];

		while (total > 0) {
			uint32_t chunk = (total > 65535u) ? 65535u : total;

			DMA_TX_Complete = 0;
			HAL_StatusTypeDef st = HAL_SPI_Transmit_DMA(LCD->hspi, (uint8_t*)p, (uint16_t)chunk);
			if (st != HAL_OK) break; //Передача не начата (HAL_BUSY) - не зависаем

			while (DMA_TX_Complete == 0) { __NOP(); }

			p += chunk;
			total -= chunk;
		}

		//Ждем фактического завершения SPI перед сменой DFF
		while (!(LCD->hspi->Instance->SR & SPI_SR_TXE));
		while (LCD->hspi->Instance->SR & SPI_SR_BSY);

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


//Кольцевое DMA-обновление: непрерывный поток фреймбуфера на экран.
//Требует, чтобы к hspi был привязан TX DMA (CubeMX: SPI TX DMA Enable).
void TFT_Driver::ST7789_Update_DMA_Cicle_On(void)
{
	SPI_TypeDef *spi = LCD->hspi->Instance;
	DMA_Stream_TypeDef *dma;

	if (LCD->hspi->hdmatx == NULL) return; //TX DMA не привязан к SPI
	dma = LCD->hspi->hdmatx->Instance;

	//NDTR DMA 16-битный: кадр больше 65535 слов одним кольцом не передать
	//(раньше размер молча усекался и поток рассыпался)
	if ((uint32_t)LCD->TFT_WIDTH * LCD->TFT_HEIGHT > 0xFFFFu) return;

	dma->CR &= ~DMA_SxCR_EN;            //Отключаем DMA
	while (!(spi->SR & SPI_SR_TXE));    //Ждем окончания передачи по SPI
	while (spi->SR & SPI_SR_BSY);
	dma->NDTR = 0;                      //Сброс счетчика DMA
	spi->CR1 &= ~SPI_CR1_SPE;           //Спокойно отключаем SPI
	spi->CR2 &= ~SPI_CR2_TXDMAEN;       //Отвязываем от DMA
	spi->CR1 &= ~SPI_CR1_DFF;           //8bit mode
	spi->CR1 |= SPI_CR1_SPE;            //Включаем для работы в обычном режиме

	if (LCD->GPIO_CS != NULL) {
		CS_0;
	}

	//int32_t i = 0;
	SPI.SendCmd(0x2A);
	SPI.SendData(LCD->dx >> 8);
	SPI.SendData(LCD->dx & 0xFF);
	SPI.SendData((LCD->dx + LCD->TFT_WIDTH - 1) >> 8);
	SPI.SendData((LCD->dx + LCD->TFT_WIDTH - 1) & 0xFF);
	SPI.SendCmd(0x2B);
	SPI.SendData(LCD->dy >> 8);
	SPI.SendData(LCD->dy & 0xFF);
	SPI.SendData((LCD->dy + LCD->TFT_HEIGHT - 1) >> 8);
	SPI.SendData((LCD->dy + LCD->TFT_HEIGHT - 1) & 0xFF);
	SPI.SendCmd(0x2C); //Memory write

	DATA;

	SPI.Spi8to16();

	dma->CR  &= ~DMA_SxCR_EN;                          // DMA
	dma->NDTR = LCD->TFT_WIDTH * LCD->TFT_HEIGHT;      // Весь фреймбуфер
	dma->PAR  = (uint32_t)&spi->DR;                    // SPI->DR
	dma->M0AR = (uint32_t)&LCD->buffer16[0];
	dma->CR  |=  DMA_SxCR_CIRC;      //Кольцевой режим
	dma->CR  |=  DMA_SxCR_EN;        //Включаем DMA

	spi->CR2 |= SPI_CR2_TXDMAEN;     //SPI на DMA
	spi->CR1 |= SPI_CR1_SPE;         //Включаем SPI для работы в DMA
}

void TFT_Driver::ST7789_Update_DMA_Cicle_Off(void)
{
	SPI_TypeDef *spi = LCD->hspi->Instance;
	DMA_Stream_TypeDef *dma;

	if (LCD->hspi->hdmatx == NULL) return; //TX DMA не привязан к SPI
	dma = LCD->hspi->hdmatx->Instance;

	dma->CR &= ~DMA_SxCR_EN;            //Отключаем DMA
	while (!(spi->SR & SPI_SR_TXE));    //Ждем окончания передачи по SPI
	while (spi->SR & SPI_SR_BSY);
	dma->NDTR = 0;                      //Сброс счетчика DMA
	dma->CR &= ~DMA_SxCR_CIRC;          //Выкл Кольцевой режим

	spi->CR1 &= ~SPI_CR1_SPE;           //Спокойно отключаем SPI
	spi->CR2 &= ~SPI_CR2_TXDMAEN;       //Отвязываем от DMA
	spi->CR1 &= ~SPI_CR1_DFF;           //8bit mode
	spi->CR1 |= SPI_CR1_SPE;            //Включаем для работы в обычном режиме

	if (LCD->GPIO_CS != NULL) {
		CS_1;
	}
}

#endif /* TFT_Driver_ST7789 */

#include "TFT_config.h"

#if defined(TFT_DRIVER_ILI9225)

#include "TFT_Driver.h"
#include "ILI9225.h"

void TFT_Driver::ILI9225_setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	if (LCD == NULL) return;

	SPI.SendCmd16 (ILI9225_HORIZONTAL_WINDOW_ADDR1);
    SPI.SendData16( x1);

    SPI.SendCmd16 ( ILI9225_HORIZONTAL_WINDOW_ADDR2);
	SPI.SendData16( x0);

    SPI.SendCmd16 ( ILI9225_VERTICAL_WINDOW_ADDR1);
	SPI.SendData16( y1);

    SPI.SendCmd16 ( ILI9225_VERTICAL_WINDOW_ADDR2);
	SPI.SendData16( y0);

    SPI.SendCmd16 ( ILI9225_RAM_ADDR_SET1);
	SPI.SendData16( x0);

    SPI.SendCmd16 ( ILI9225_RAM_ADDR_SET2);
	SPI.SendData16( y0);

    SPI.SendCmd16 ( 0x0022);

}

void TFT_Driver::ILI9225_UpdateScreen(void)
{
	if (LCD == NULL) return;
#if defined(TFT_PLATFORM_STM32)
	if (LCD->hspi == NULL || LCD->hspi->Instance == NULL) return;
	if (LCD->Bit == 4 && LCD->buffer8 == NULL) return;
	if (LCD->Bit == 16 && LCD->buffer16 == NULL) return;
#else
	if (LCD->spi == NULL) return;
	if (LCD->Bit == 4 && LCD->buffer8 == NULL) return;
	if (LCD->Bit == 16 && LCD->buffer16 == NULL) return;
#endif

#if defined(TFT_PLATFORM_STM32)
	if (LCD->GPIO_CS != NULL) {
		CS_0;
	}
#else
	CS_0;
#endif

	SPI.Spi8to16();

	ILI9225_setWindow(0, 0, LCD->TFT_WIDTH - 1, LCD->TFT_HEIGHT - 1);

	DATA;

#if defined(TFT_PLATFORM_STM32)
	if (LCD->Bit == 4)
	{
		uint32_t stride = ((uint32_t)LCD->TFT_WIDTH + 1) / 2;
		for (int32_t y = 0; y < LCD->TFT_HEIGHT; y++)
		{
			const uint8_t *src_line = &LCD->buffer8[y * stride];
			for (int32_t x = 0; x < LCD->TFT_WIDTH; x++)
			{
				uint8_t b = src_line[x / 2];
				uint16_t c = (x & 1) ? LCD->palete[b & 0x0F] : LCD->palete[b >> 4];
				LCD->hspi->Instance->DR = c;
				uint32_t wait_start = HAL_GetTick();
				while ((LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0) {
					if (HAL_GetTick() - wait_start > 50) break;
				}
			}
		}
	}

	if (LCD->Bit == 16)
	{
		for(uint32_t i = 0; i < (uint32_t)LCD->TFT_WIDTH * LCD->TFT_HEIGHT; i++)
		{
				LCD->hspi->Instance->DR = LCD->buffer16[i];
				uint32_t wait_start = HAL_GetTick();
				while( (LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0 ) {
					if (HAL_GetTick() - wait_start > 50) break;
				}
		}
	}

	//Ждем фактического завершения передачи перед сменой DFF
	uint32_t wait_start = HAL_GetTick();
	while( (LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0 ) {
		if (HAL_GetTick() - wait_start > 50) break;
	}
	while( LCD->hspi->Instance->SR & SPI_FLAG_BSY ) {
		if (HAL_GetTick() - wait_start > 50) break;
	}
#else // TFT_PLATFORM_PICO
	if (LCD->Bit == 4)
	{
		uint32_t stride = ((uint32_t)LCD->TFT_WIDTH + 1) / 2;
		for (int32_t y = 0; y < LCD->TFT_HEIGHT; y++)
		{
			const uint8_t *src_line = &LCD->buffer8[y * stride];
			for (int32_t x = 0; x < LCD->TFT_WIDTH; x++)
			{
				uint8_t b = src_line[x / 2];
				uint16_t c = (x & 1) ? LCD->palete[b & 0x0F] : LCD->palete[b >> 4];
				spi_write16_blocking(LCD->spi, &c, 1);
			}
		}
	}

	if (LCD->Bit == 16)
	{
		spi_write16_blocking(LCD->spi, LCD->buffer16, (size_t)LCD->TFT_WIDTH * LCD->TFT_HEIGHT);
	}

	while (spi_get_hw(LCD->spi)->sr & SPI_SSPSR_BSY_BITS);
#endif

	SPI.Spi16to8();

#if defined(TFT_PLATFORM_STM32)
	if (LCD->GPIO_CS != NULL) {
		CS_1;
	}
#else
	CS_1;
#endif


}

#endif

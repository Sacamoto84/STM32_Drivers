#include "TFT_config.h"

#if defined(TFT_DRIVER_ILI9225)

#include "TFT_Driver.h"
#include "ILI9225.h"

void TFT_Driver::ILI9225_setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

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

	if (LCD->GPIO_CS != NULL) {
		CS_0;
	}

	ILI9225_setWindow(0, 0, LCD->TFT_WIDTH - 1, LCD->TFT_HEIGHT - 1);

	DATA;

	if (LCD->Bit == 4)
	{
		for(uint32_t i = 0; i < (uint32_t)LCD->TFT_WIDTH * LCD->TFT_HEIGHT; i++)
		{
			if (i % 2)
			{
				LCD->hspi->Instance->DR = LCD->palete[(LCD->buffer8[i/2]) & 0x0F]; //4 bit
				while( (LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0 );
			}
			else
			{
				LCD->hspi->Instance->DR = LCD->palete[(LCD->buffer8[i/2]) >> 4]; //4 bit
				while( (LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0 );
			}
		}
	}

	if (LCD->Bit == 16)
	{
		for(uint32_t i = 0; i < (uint32_t)LCD->TFT_WIDTH * LCD->TFT_HEIGHT; i++)
		{
				LCD->hspi->Instance->DR = LCD->buffer16[i];
				while( (LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0 );
		}
	}

	//Ждем фактического завершения передачи перед сменой DFF
	while( (LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0 );
	while( LCD->hspi->Instance->SR & SPI_FLAG_BSY );

	if (LCD->GPIO_CS != NULL) {
		CS_1;
	}


}

#endif

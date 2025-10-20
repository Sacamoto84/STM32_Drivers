#include "TFT_config.h"


#if defined(TFT_DRIVER_ILI9225)

#include "ILI9225.h"

#define TFTLCD_DELAY 0xFFFF
const uint16_t ILI9225_regValues[] = {
	/* Start Initial Sequence */
	/* Set SS bit and direction output from S528 to S1 */
	ILI9225_POWER_CTRL1, 0x0000,//0x0000, // Set SAP,DSTB,STB
	ILI9225_POWER_CTRL2, 0x0000, // Set APON,PON,AON,VCI1EN,VC
	ILI9225_POWER_CTRL3, 0x0000, // Set BT,DC1,DC2,DC3
	ILI9225_POWER_CTRL4, 0x0000,//0x0000, // Set GVDD
	ILI9225_POWER_CTRL5, 0x0000,//0x0000, // Set VCOMH/VCOML voltage
	TFTLCD_DELAY, 40, 

	// Power-on sequence
	ILI9225_POWER_CTRL2, 0x0018, // Set APON,PON,AON,VCI1EN,VC
	ILI9225_POWER_CTRL3, 0x6121, // Set BT,DC1,DC2,DC3
	ILI9225_POWER_CTRL4, 0x006F, // Set GVDD   /*007F 0088 */
	ILI9225_POWER_CTRL5, 0x495F, // Set VCOMH/VCOML voltage
	ILI9225_POWER_CTRL1, 0x0800, // Set SAP,DSTB,STB
	
	TFTLCD_DELAY, 10,
	ILI9225_POWER_CTRL2, 0x103B, // Set APON,PON,AON,VCI1EN,VC
	TFTLCD_DELAY, 50,

	ILI9225_DRIVER_OUTPUT_CTRL, 0x011C, // set the display line number and display direction
	ILI9225_LCD_AC_DRIVING_CTRL, 0x0100, // set 1 line inversion
	ILI9225_ENTRY_MODE, 0x1030, // set GRAM write direction and BGR=1.
	ILI9225_DISP_CTRL1, 0x0000, // Display off
	ILI9225_BLANK_PERIOD_CTRL1, 0x0808, // set the back porch and front porch
	ILI9225_FRAME_CYCLE_CTRL, 0x1100, // set the clocks number per line
	ILI9225_INTERFACE_CTRL, 0x0000, // CPU interface
	ILI9225_OSC_CTRL, 0x0D01, // Set Osc  /*0e01*/ 110hz
	ILI9225_VCI_RECYCLING, 0x0020, // Set VCI recycling
	ILI9225_RAM_ADDR_SET1, 0x0000, // RAM Address
	ILI9225_RAM_ADDR_SET2, 0x0000, // RAM Address

	/* Set GRAM area */
	ILI9225_GATE_SCAN_CTRL, 0x0000, 
	ILI9225_VERTICAL_SCROLL_CTRL1, 0x00DB, 
	ILI9225_VERTICAL_SCROLL_CTRL2, 0x0000, 
	ILI9225_VERTICAL_SCROLL_CTRL3, 0x0000, 
	ILI9225_PARTIAL_DRIVING_POS1, 0x00DB, 
	ILI9225_PARTIAL_DRIVING_POS2, 0x0000, 
	ILI9225_HORIZONTAL_WINDOW_ADDR1, 0x00AF, 
	ILI9225_HORIZONTAL_WINDOW_ADDR2, 0x0000, 
	ILI9225_VERTICAL_WINDOW_ADDR1, 0x00DB, 
	ILI9225_VERTICAL_WINDOW_ADDR2, 0x0000, 

////	/* Set GAMMA curve */
  ILI9225_GAMMA_CTRL1, 0x0000,	
  ILI9225_GAMMA_CTRL2, 0x0808, 
	ILI9225_GAMMA_CTRL3, 0x080A, 
	ILI9225_GAMMA_CTRL4, 0x000A, 
	ILI9225_GAMMA_CTRL5, 0x0A08, 
  ILI9225_GAMMA_CTRL6, 0x0808, 
	ILI9225_GAMMA_CTRL7, 0x0000, 
	ILI9225_GAMMA_CTRL8, 0x0A00,  //Высветляет
	//ILI9225_GAMMA_CTRL8, 0x0100,  //Высветляет XX--
  ILI9225_GAMMA_CTRL9, 0x0710, 
	ILI9225_GAMMA_CTRL10, 0x0710, 
	
	ILI9225_DISP_CTRL1, 0x0012, 
	TFTLCD_DELAY, 50, 
	ILI9225_DISP_CTRL1, 0x1017,
};

void TFT::ILI9225_init_table16(const uint16_t *table, int16_t size)
{
      const uint16_t  *p = table;
	  static uint16_t  cmd;
	  static uint16_t  d;
	
    while (size > 0) 
		{
			  cmd  = *p++;
			  d    = *p++;
        
			  if (cmd == TFTLCD_DELAY) HAL_Delay(d);
        else 
				  { 
            SPI.SendCmd16(cmd);
            SPI.SendData16( d);
          }
					
      size -= 2 * sizeof(int16_t);
    }
}

void TFT::ILI9225_init(void)
{
  //Перевод на 16 bit формат
  //LCD->hspi->Instance->CR1 |=  SPI_CR1_SPE; //Включаем SPI

  //Сброс экрана
  if (LCD->GPIO_RESET != NULL) {
  		SPI.LCD_Reset();
  	}

  if (LCD->GPIO_CS != NULL) {
  		CS_0;
  	}

  SPI.Spi8to16(); //Перевод на 16 бит

  ILI9225_init_table16(ILI9225_regValues, sizeof(ILI9225_regValues));

  if (LCD->GPIO_CS != NULL) {
  		CS_1;
  	}
}

#endif /* TFT_Driver_ILI9225 */

#include "TFT_config.h"

#if defined (TFT_DRIVER_SSD1306)

#include "../TFT.h"

#define SSD1306_I2C_ADDR         0x78 //0x78
#define SSD1306_I2C_ADDR2        0x7A //0x7A


//* Write command */
//#define SSD1306_WRITECOMMAND(command)      ssd1306_I2C_Write(SSD1306_I2C, SSD1306_I2C_ADDR, 0x00, (command))
///* Write data */
//#define SSD1306_WRITEDATA(data)            ssd1306_I2C_Write(SSD1306_I2C, SSD1306_I2C_ADDR, 0x40, (data))

void TFT_Driver::SSD1306_WRITECOMMAND(uint8_t data) {
	if (LCD->hi2c)  //I2C
	{
		uint8_t dt[2];
		dt[0] = 0;
		dt[1] = data;
		HAL_I2C_Master_Transmit(LCD->hi2c, LCD->I2C_Adress, dt, 2, 1000);
	} else                          //SPI
	{
#if defined(TFT_USE_SPI)
		SPI.SendCmd(data);
#endif
	}

}

void TFT_Driver::SSD1306_WRITEDATA(uint8_t data) {
	if (LCD->hi2c)  //I2C
	{
#ifdef TFT_USE_I2C
		uint8_t dt[2];
		dt[0] = 0x40;
		dt[1] = data;
		HAL_I2C_Master_Transmit(LCD->hi2c, LCD->I2C_Adress, dt, 2, 1000);
#endif
	} else //SPI
	{
#if defined( TFT_USE_SPI)
		SPI.SendData(data);
#endif
	}

}

//Инициализация первого экрана
void TFT_Driver::SSD1306_Init(void) {

#if defined(TFT_USE_SPI)
	//Только для SPI
	if (LCD->GPIO_RESET != NULL)	
		SPI.LCD_Reset();
	
	if (LCD->GPIO_CS != NULL)	
		CS_0;
#endif

	/* Init LCD */
	SSD1306_WRITECOMMAND(0xAE); //display off
	SSD1306_WRITECOMMAND(0x20); //Set Memory Addressing Mode
	SSD1306_WRITECOMMAND(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	SSD1306_WRITECOMMAND(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	SSD1306_WRITECOMMAND(0xC8); //Set COM Output Scan Direction
	SSD1306_WRITECOMMAND(0x00); //---set low column address
	SSD1306_WRITECOMMAND(0x10); //---set high column address
	SSD1306_WRITECOMMAND(0x40); //--set start line address
	SSD1306_WRITECOMMAND(0x81); //--set contrast control register
	SSD1306_WRITECOMMAND(0xFF);
	SSD1306_WRITECOMMAND(0xA1); //--set segment re-map 0 to 127
	SSD1306_WRITECOMMAND(0xA6); //--set normal display
	SSD1306_WRITECOMMAND(0xA8); //--set multiplex ratio(1 to 64)

	if (LCD->TFT_HEIGHT == 64)
		SSD1306_WRITECOMMAND(0x3F); //64
	else
		SSD1306_WRITECOMMAND(0x1F); //32

	SSD1306_WRITECOMMAND(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	SSD1306_WRITECOMMAND(0xD3); //-set display offset
	SSD1306_WRITECOMMAND(0x00); //-not offset
	SSD1306_WRITECOMMAND(0xD5); //--set display clock divide ratio/oscillator frequency
	SSD1306_WRITECOMMAND(0xF0); //--set divide ratio
	SSD1306_WRITECOMMAND(0xD9); //--set pre-charge period
	SSD1306_WRITECOMMAND(0x22); //

	SSD1306_WRITECOMMAND(0xDA); //--set com pins hardware configuration
	if (LCD->TFT_HEIGHT == 64)
		SSD1306_WRITECOMMAND(0x12); //64
	else
		SSD1306_WRITECOMMAND(0x02); //32

	SSD1306_WRITECOMMAND(0xDB); //--set vcomh
	SSD1306_WRITECOMMAND(0x20); //0x20,0.77xVcc
	SSD1306_WRITECOMMAND(0x8D); //--set DC-DC enable
	SSD1306_WRITECOMMAND(0x14); //
	SSD1306_WRITECOMMAND(0xAF); //--turn on SSD1306 panel

#if defined(TFT_USE_SPI)
	if (LCD->GPIO_CS != NULL)	
		CS_1;
    #endif
	/* Set default values */
	//uTFT.CurrentX = 0;
	//uTFT.CurrentY = 0;
}

volatile HAL_StatusTypeDef res;

//490uS-72MHz
void TFT_Driver::SSD1306_UpdateScreen(void) {
	uint8_t *p;

	if (LCD->hi2c)  //I2C
	{

#ifdef	TFT_USE_I2C


//		for(uint8_t i = 0; i < 7; i++) {
//		        //SSD1306_WRITECOMMAND(0xB0+i);
//		        //SSD1306_WRITECOMMAND(0x00);
//		       // SSD1306_WRITECOMMAND(0x10);
//                HAL_Delay(5);
//		        HAL_I2C_Master_Transmit(LCD->hi2c, LCD->I2C_Adress, &LCD->buffer8[128*i],
//		       				128, 1000);
//
//		    }







//		SSD1306_WRITECOMMAND(0xB0);
//		SSD1306_WRITECOMMAND(0x00);
//		SSD1306_WRITECOMMAND(0x10);

#if defined(LCD_osDelay)
			osDelay(2);
		#else
		//HAL_Delay(10);
#endif

		p = &LCD->buffer8[0];
		p--;
		*p = 0x40;

		res = HAL_I2C_Master_Transmit(LCD->hi2c, LCD->I2C_Adress, p,
				(LCD->TFT_HEIGHT * LCD->TFT_WIDTH) / 8 + 1, 1000);


		return;
#endif

	} else { //SPI

#if defined(TFT_USE_SPI)
    if (LCD->GPIO_CS != NULL)	
    	CS_0;
		
		SSD1306_WRITECOMMAND( 0xB0);
		SSD1306_WRITECOMMAND( 0x00);
		SSD1306_WRITECOMMAND( 0x10);
		
		HAL_GPIO_WritePin(LCD->GPIO_DC, LCD->GPIO_Pin_DC, GPIO_PIN_SET);
		uint8_t * p;
		p = &LCD->buffer8[0];
		p--;			
		for(uint16_t i = 0; i < 1025; i++)
		{			
		  LCD->hspi->Instance->DR = *p++;		
          while( (LCD->hspi->Instance->SR & SPI_FLAG_TXE) == 0 );
		}	
		
		if (LCD->GPIO_CS != NULL)	
			CS_1;

#endif

	}

}

void TFT_Driver::SSD1306_Contrast(uint8_t c) {
	SSD1306_WRITECOMMAND(0x81);
	SSD1306_WRITECOMMAND(c);
}

#endif /* TFT_DRIVER_SSD1306 */

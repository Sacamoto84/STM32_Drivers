### ✨ [top](../README.md)

# Инициализация 

###Создаем описание экрана, и описываем его характеристики, мы можем использовать в качестве Driver

```
ST7735,	ST7789,	ILI9225, SSD1306, PCD8544, ST7735S, LCD_USB, SPRITE_RAM
```

```
typedef struct {
	int32_t TFT_WIDTH;       // Ширина
	int32_t TFT_HEIGHT;      // Высота
	LCD_DRIVER LCD_Driver;   // Драйвер ST7735,	ST7789,	ILI9225, SSD1306, PCD8544, ST7735S, LCD_USB, SPRITE_RAM
	uint8_t Bit;             // Битность 16 8 4 2 1

#ifdef TFT_USE_SPI
  SPI_HandleTypeDef *hspi;
#else
  uint32_t *notUse;
#endif

#ifdef TFT_USE_I2C
  I2C_HandleTypeDef *hi2c;
#else
  uint32_t * notUse2;
#endif

	uint8_t I2C_Adress;

	uint16_t *buffer16;        // буффер для 16  битных цветов
	uint8_t  *buffer8;         // буффер для !16 битных цветов
	uint16_t *palete;          // Палитра

	uint16_t dx;               // Смещение начала отображения, для некоторых экранов нужно
	uint16_t dy;               // 
	GPIO_TypeDef *GPIO_CS;
	uint16_t GPIO_Pin_CS;
	GPIO_TypeDef *GPIO_DC;
	uint16_t GPIO_Pin_DC;
	GPIO_TypeDef *GPIO_RESET;
	uint16_t GPIO_Pin_RESET;
	_MADCTL MADCTL;
	_ROTATE_DELTA ROTATE_DELTA; //

} TFT_LCD_t;
```












# Использование

```cpp
TFT tft; //Экрана
...
tft.init(&LCD_0); //Привязка описания экрана и его инициализация

```

# Примеры готовых описаний экранов

SSD1306 (Новая версия)
========================
	
```cpp
//SSD1306 128 64 1 bit
uint8_t LCD_Buffer[128 * 8];
TFT_LCD_t LCD_0 = {
      128,                  // Ширина экрана
      64,                   // Высота экрана
      SSD1306,              // Драйвер
      1,                    // bit
      NULL,                 // notUse (SPI)
      &hi2c1,	               // I2C
      0x78,                 // адресс
      NULL,                 // 16  бит буффер
      &LCD_Buffer[0],       // !16 бит буффер
};
```

ST7789(Страная версия нужно переделать)
========================

```cpp
//ST7789 135 240 16 bit
uint16_t LCD_Buffer16_0[240 * 136 + 16];

TFT_LCD_t LCD_0 = { 135,                  // Ширина экрана
    240,                  // Высота экрана
    ST7789,               // Драйвер
    16,                   // bit
    &hspi5,               // Spi
    NULL,	              // I2C
    0,                    // I2C адрес (не используется)
    &LCD_Buffer16_0[6],   // 16 бит буффер
    NULL,                 // !16 бит буффер
    NULL,                 // Палитра
    52,                   // Смещение по X
    40,                   // Смещение по Y
    CS_GPIO_Port,         // CS PORT
    CS_Pin,               // CS PIN
    DC_GPIO_Port,         // DC PORT
    DC_Pin,               // DC PIN
    RESET_GPIO_Port,      // RESET PORT
    RESET_Pin,	          // RESET PIN
    0, 
    {                     //MADCTL
		0,   //MY
		0,   //MX
		0,   //MV
		0,   //ML
		0,   //RGB
		0,   //MH
    }, 
    {               //Смещение но X Y
    135, 240, 52,   //DX_0
    40,             //DY_0
    240, 135, 40,   //DX_90
    53,             //DY_90
    135, 240, 53,   //DX_180
    40,             //DY_180
    240, 135, 40,   //DX_270
    52,             //DY_270
    } 
};

```
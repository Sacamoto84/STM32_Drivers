#ifndef TFT_DRIVER_H_
#define TFT_DRIVER_H_

#include "TFT_config.h"

#include "TFT_define.h"

#ifdef TFT_USE_SPI
  #ifdef __cplusplus
  #include "TFT_SPI.h"
  #endif
#endif

#ifdef __cplusplus
class TFT_Driver {
public:

	TFT_LCD_t *LCD;

#ifdef TFT_USE_SPI
	  TFT_SPI SPI;
    #endif

	//Флаг завершения DMA-передачи. Пользователь ОБЯЗАН определить HAL_SPI_TxCpltCallback
	//и выставлять в нем DMA_TX_Complete = 1 (для активного hspi),
	//иначе функции ST7789_UpdateDMA*V2/V3 зависнут в ожидании.
	volatile uint32_t DMA_TX_Complete;
	uint32_t blockUpdate;
	uint32_t needUpdate;

	void init(TFT_LCD_t *_LCD) {
		LCD = _LCD;

		//Детерминированное начальное состояние флагов.
		//Для объекта на стеку/куче без этого мусор в blockUpdate
		//навсегда блокирует Update, а DMA-циклы зависают в ожидании.
		DMA_TX_Complete = 1; //1 = передачи нет, можно стартовать
		blockUpdate = 0;
		needUpdate = 0;

		switch (LCD->LCD_Driver) {

#if defined(TFT_DRIVER_SSD1306)
		case SSD1306:
#ifdef TFT_USE_SPI
			SPI.init(_LCD);
#endif
			SSD1306_Init();
			break;
#endif

#if defined(TFT_DRIVER_ST7789)
		    case ST7789:
			  SPI.init(_LCD);
			  ST7789_Init();
			  break;   // 1 - ST7789
#endif

#if defined(TFT_DRIVER_ILI9225)
		    case ILI9225:
			  SPI.init(_LCD);
			  ILI9225_init();
			  break;	 // 2 - ILI9225
#endif

	default:
		break;
	}
}

	void Update(void) {
		switch (LCD->LCD_Driver) {

#if defined(TFT_DRIVER_SSD1306)
		case SSD1306:
			SSD1306_UpdateScreen();
			break;
#endif

#if defined(TFT_DRIVER_ILI9225)
		case ILI9225:
			ILI9225_UpdateScreen();
			break; // 2 - ILI9225
#endif

#if defined(TFT_DRIVER_ST7789)
		case ST7789:
			ST7789_Update();
			break; //1-ST7789
#endif

#if defined(TFT_DRIVER_LCD_USB)
		case LCD_USB:
			uTFT_USB_Update();
			break;
#endif

		default:
			break;
		}
	}
	;

	void UpdateWindow(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {

		switch (LCD->LCD_Driver) {
#if defined(TFT_DRIVER_ST7789)
		case ST7789:
			ST7789_Update_Window(x1, y1, x2, y2);
			break; //1-ST7789
#endif

		default:
			break;
		}
	}
	;







#if defined(TFT_DRIVER_SSD1306)
	void SSD1306_Init();
	void SSD1306_WRITECOMMAND(uint8_t data);
	void SSD1306_WRITEDATA(uint8_t data);
	void SSD1306_UpdateScreen(void);
	void SSD1306_Contrast(uint8_t c);
#endif

#if defined(TFT_DRIVER_ILI9225)
	void ILI9225_init();
	void ILI9225_init_table16(const uint16_t *table, int16_t size);
	void ILI9225_setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	void ILI9225_UpdateScreen();
#endif

#if defined(TFT_DRIVER_ST7789)
	void ST7789_Init(void);
	void ST7789_Update(void);
	void ST7789_Update(List_Update_Particle U);
	void ST7789_Update(int x0, int y0, int x1, int y1);
	void ST7789_AddrSet(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	// Поворот экрана
	void ST77XX_Rotate(uint16_t depth);
	//Обновление регистра MADCTL
	void ST77XX_Update_MADCTL(void);
	void ST7789_Update_Window(int16_t x1, int16_t y1, int16_t x2, int16_t y2);

	//Ожидание завершения предыдущей DMA-строки и запуск новой.
	//Возвращает статус HAL: при ошибке передача не начата.
	HAL_StatusTypeDef ST7789_DMA_WaitAndSend(uint16_t *line);

	void ST7789_UpdateDMA4bitV2(void);
	void ST7789_UpdateDMA8bitV2(void);
	void ST7789_UpdateDMA16bitV2(void);
	//DMA с блокировкой
	void ST7789_UpdateDMA16bitV3(void);
	void ST7789_Transmit_Array(char dc, uint8_t *data, int nbytes);
	void ST7789_Update_DMA_Cicle_On(void);
	void ST7789_Update_DMA_Cicle_Off(void);
#endif

#if defined(TFT_DRIVER_LCD_USB)
	void uTFT_USB_Update();
#endif

}

;

#endif /* __cplusplus */

#endif /* TFT_DRIVER_H_ */

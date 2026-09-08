### ✨ [top](../README.md)

# Конфигурация TFT_config.h

Скопируйте `TFT_config_Template.h` в папку include вашего проекта под именем `TFT_config.h`.

## Интерфейсы

```cpp
#define TFT_USE_SPI   // SPI (ST7789, ILI9225, SSD1306)
#define TFT_USE_I2C   // I2C (SSD1306)
```

Внимание: имя макроса - латинская `C`. Старое имя с кириллической `С`
поддерживается для совместимости, но использовать его не следует.

## Драйвер экрана (обязательно один)

```cpp
//#define TFT_DRIVER_SSD1306
//#define TFT_DRIVER_ILI9225
#define TFT_DRIVER_ST7789
```

ST7735 / ST7735S / PCD8544 объявлены в enum `LCD_DRIVER`, но реализаций
в библиотеке нет - выбирайте из трех перечисленных выше.

## Глубина цвета фреймбуфера

```cpp
//#define TFT_USE_1BIT
//#define TFT_USE_4BIT
//#define TFT_USE_8BIT
//#define TFT_USE_16BIT
```

Если не задана ни одна - доступны все варианты, выбор по `LCD->Bit`
во время выполнения. Можно задать несколько.

```cpp
//#define TFT_USE_ONLY_ONE_BIT_COLOR
```

Оптимизация: единственная глубина цвета, проверки `LCD->Bit`
не генерируются (меньше код, быстрее SetPixel/GetPixel).

## Модули

```cpp
//#define TFT_UNIT_VIDEO   // плеер видео с SD (Video/video.cpp), требует FAT_FS
//#define FAT_FS 1         // чтение BMP/Gif с SD (Bitmap/BMPFromFile*.cpp, Animated/Gif.h)
//#define TFT_USE_TIMBER   // логирование через timber (Gif, Video)
//#define TFT_USE_DWT      // замер времени кадра через HiSpeedDWT (Gif)
```

## DMA-обновления ST7789

Функции `ST7789_UpdateDMA*V2/V3` ждут флаг `DMA_TX_Complete`.
Пользователь ОБЯЗАН определить колбэк завершения DMA:

```cpp
extern "C" void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    tft.driver.DMA_TX_Complete = 1;
}
```

Без колбэка функции зависнут. Требуется TX DMA, привязанный к SPI в CubeMX.

`ST7789_Update_DMA_Cicle_On/Off` (кольцевое DMA) берут регистры SPI и DMA
из `hspi->hdmatx` - жесткая привязка к конкретным адресам SPI1/DMA2 удалена.

### 🏠 [← На главную](../README.md)

# Инициализация и структура описания экрана (`TFT_LCD_t`)

В библиотеке **uTFT2** каждый экран или виртуальный спрайт описывается структурой `TFT_LCD_t`. Это позволяет управлять несколькими дисплеями одновременно (например, основным цветным TFT и дополнительным сервисным OLED), а также создавать независимые виртуальные экраны в оперативной памяти (`SPRITE_RAM`).

---

## 1. Поддерживаемые драйверы (`LCD_DRIVER`)

В перечислении `LCD_DRIVER` объявлены контроллеры:
* **`ST7789`** — полноцветные IPS/TFT матрицы (SPI, поддержка 16 и 4 бит, DMA).
* **`SSD1306`** — монохромные OLED дисплеи (I2C или SPI, 1 бит).
* **`ILI9225`** — цветные TFT матрицы (SPI, 16 и 4 бит).
* **`SPRITE_RAM`** — виртуальный дисплей в оперативной памяти (off-screen framebuffer для композитинга и трансформаций).

> [!NOTE]
> Значения `ST7735`, `ST7735S`, `PCD8544`, `LCD_USB` присутствуют в enum для обратной совместимости, но аппаратных реализаций в кодовой базе не имеют.

---

## 2. Описание полей структуры `TFT_LCD_t`

```cpp
typedef struct {
    int32_t TFT_WIDTH;          // Физическая ширина экрана в пикселях
    int32_t TFT_HEIGHT;         // Физическая высота экрана в пикселях
    LCD_DRIVER LCD_Driver;      // Тип контроллера (ST7789, SSD1306, ILI9225, SPRITE_RAM)
    uint8_t Bit;                // Глубина цвета фреймбуфера: 16, 8, 4 или 1

#ifdef TFT_USE_SPI
    SPI_HandleTypeDef *hspi;    // Указатель на структуру SPI HAL (например, &hspi1)
#else
    uint32_t *notUse;
#endif

#ifdef TFT_USE_I2C
    I2C_HandleTypeDef *hi2c;    // Указатель на структуру I2C HAL (например, &hi2c1)
#else
    uint32_t *notUse2;
#endif

    uint8_t I2C_Adress;         // 8-битный адрес I2C (для SSD1306 обычно 0x78)

    uint16_t *buffer16;         // Указатель на буфер кадра для 16 бит цвета
    uint8_t  *buffer8;          // Указатель на буфер кадра для 1, 4 и 8 бит цвета
    uint16_t *palete;           // Указатель на палитру RGB565 (для 4-битного и 8-битного режимов)

    uint16_t dx;                // Аппаратное смещение по оси X (для матриц с нестандартным окном)
    uint16_t dy;                // Аппаратное смещение по оси Y
    GPIO_TypeDef *GPIO_CS;      // Порт пина Chip Select (CS) (или NULL, если пин не задействован)
    uint16_t GPIO_Pin_CS;       // Номер пина CS
    GPIO_TypeDef *GPIO_DC;      // Порт пина Data/Command (DC)
    uint16_t GPIO_Pin_DC;       // Номер пина DC
    GPIO_TypeDef *GPIO_RESET;   // Порт пина сброса RESET (или NULL)
    uint16_t GPIO_Pin_RESET;    // Номер пина RESET
    _MADCTL MADCTL;             // Настройки развертки контроллера (MY, MX, MV, ML, RGB, MH)
    _ROTATE_DELTA ROTATE_DELTA; // Смещения и размеры для углов поворота 0°, 90°, 180°, 270°
} TFT_LCD_t;
```

---

## 3. Примеры конфигураций экранов

### Пример 1: ST7789 (135x240, 16 бит, SPI)

Для 16-битного цвета создается буфер `uint16_t buffer[W * H]` (64 800 байт):

```cpp
#include "main.h"
#include "TFT.h"

// 135 * 240 пикселей * 2 байта = 64 800 байт
uint16_t st7789_buffer16[135 * 240];

TFT_LCD_t LCD_ST7789 = {
    .TFT_WIDTH       = 135,
    .TFT_HEIGHT      = 240,
    .LCD_Driver      = ST7789,
    .Bit             = 16,
    .hspi            = &hspi1,
    .hi2c            = NULL,
    .I2C_Adress      = 0,
    .buffer16        = st7789_buffer16,
    .buffer8         = NULL,
    .palete          = NULL,
    .dx              = 52,     // Смещение окна контроллера для дисплея 135x240
    .dy              = 40,
    .GPIO_CS         = LCD_CS_GPIO_Port,
    .GPIO_Pin_CS     = LCD_CS_Pin,
    .GPIO_DC         = LCD_DC_GPIO_Port,
    .GPIO_Pin_DC     = LCD_DC_Pin,
    .GPIO_RESET      = LCD_RST_GPIO_Port,
    .GPIO_Pin_RESET  = LCD_RST_Pin,
    .MADCTL = {
        .MY  = 0,
        .MX  = 0,
        .MV  = 0,
        .ML  = 0,
        .RGB = 0,
        .MH  = 0,
    },
    .ROTATE_DELTA = {
        .W_0    = 135, .H_0    = 240, .DX_0   = 52, .DY_0   = 40,
        .W_90   = 240, .H_90   = 135, .DX_90  = 40, .DY_90  = 53,
        .W_180  = 135, .H_180  = 240, .DX_180 = 53, .DY_180 = 40,
        .W_270  = 240, .H_270  = 135, .DX_270 = 40, .DY_270 = 52,
    }
};

TFT tft;

void InitDisplay(void) {
    tft.init(&LCD_ST7789);
    tft.Fill(BLACK);
    tft.driver.ST7789_Update();
}
```

---

### Пример 2: ST7789 (135x240, 4 бита с палитрой, экономия RAM)

В 4-битном режиме 1 байт хранит 2 пикселя. Нечётная ширина 135 пикселей требует шаг строки `(135 + 1) / 2 = 68` байт. 
Общий размер буфера составляет всего `68 * 240 = 16 320 байт` (экономия памяти в 4 раза!):

```cpp
// 68 байт на строку * 240 строк = 16 320 байт
uint8_t st7789_buffer4bit[((135 + 1) / 2) * 240];

// Палитра из 16 цветов в формате RGB565
uint16_t palette16[16] = {
    BLACK, NAVY, DARKGREEN, DARKCYAN, MAROON, PURPLE, OLIVE, LIGHTGREY,
    DARKGREY, BLUE, GREEN, CYAN, RED, MAGENTA, YELLOW, WHITE
};

TFT_LCD_t LCD_ST7789_4Bit = {
    .TFT_WIDTH       = 135,
    .TFT_HEIGHT      = 240,
    .LCD_Driver      = ST7789,
    .Bit             = 4,
    .hspi            = &hspi1,
    .buffer16        = NULL,
    .buffer8         = st7789_buffer4bit,
    .palete          = palette16,
    .dx              = 52,
    .dy              = 40,
    .GPIO_CS         = LCD_CS_GPIO_Port,
    .GPIO_Pin_CS     = LCD_CS_Pin,
    .GPIO_DC         = LCD_DC_GPIO_Port,
    .GPIO_Pin_DC     = LCD_DC_Pin,
    .GPIO_RESET      = LCD_RST_GPIO_Port,
    .GPIO_Pin_RESET  = LCD_RST_Pin,
};
```

---

### Пример 3: SSD1306 (128x64, 1 бит, I2C)

Для монохромного OLED дисплея 128x64 буфер занимает `128 * 64 / 8 = 1024 байта`:

```cpp
uint8_t oled_buffer[128 * 64 / 8];

TFT_LCD_t LCD_SSD1306_I2C = {
    .TFT_WIDTH       = 128,
    .TFT_HEIGHT      = 64,
    .LCD_Driver      = SSD1306,
    .Bit             = 1,
    .hi2c            = &hi2c1,
    .I2C_Adress      = 0x78,
    .buffer16        = NULL,
    .buffer8         = oled_buffer,
    .palete          = NULL,
};

TFT oled;

void InitOLED(void) {
    oled.init(&LCD_SSD1306_I2C);
    oled.Fill(0); // Очистка (черный)
    oled.driver.SSD1306_UpdateScreen();
}
```

---

### Пример 4: Виртуальный экран в памяти (SPRITE_RAM)

Для создания канвы в ОЗУ без привязки к физическим контроллерам:

```cpp
#include "Transformation/transformation.h"

// Макрос выделения буфера и дескриптора для 16-битного спрайта 32x32:
CREATE_SPRITE16(mySprite, 32, 32);

TFT spr;

void SetupSprite(void) {
    spr.init(&mySprite);
    spr.Fill(BLACK);
    spr.CircleFilled(16, 16, 12, RED);

    // Копирование спрайта на физический экран по координатам (X=50, Y=80)
    tft.copy(&spr, 50, 80);
}
```
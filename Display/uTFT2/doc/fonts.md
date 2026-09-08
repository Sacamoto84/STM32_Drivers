
# GFX-шрифты (FontGFX)

```cpp
#include "gfxfont.h"
#include "gfx_fonts_includes.h"

GFXFONT gfxfont;

gfxfont.init(&tft);
gfxfont.setFont(&FreeMono9pt7b);

gfxfont.set_delta_x(-3);   //Добавочный интервал между символами

tft.GotoXY(2, 60);
gfxfont.Puts(str);
```

Символы вне диапазона выбранного шрифта (например кириллица в латинском
FreeMono) пропускаются с продвижением курсора.

# Smooth-шрифты (FontSmooth)

Формат vlw, сглаживание по альфе:

```cpp
#include "FontSmooth/FontSmooth.h"

Font_Smooth_Load(_acRoboto_Medium_en_ru_24);  //Загрузка метрик

tft.SetFontColor(WHITE, BLACK);
Font_Smooth_drawStr(&tft, 10, 10, "Привет");  //UTF-8 строка
```

# Classic-шрифты (FontClassic)

Фиксированные растровые:

```cpp
#include "FontClassic/FontClassic.h"

tft.SetFontColor(WHITE, BLACK);
tft.GotoXY(10, 10);
FontClassicPuts(&tft, (char*)"Hello", &Font_7x10, 0);
```

# MicroElectronika-шрифты (FontMicroElectronika)

```cpp
#include "FontMicroElectronika/FontMicroElectronika.h"

FontMicroSetFont(&Courier_12x18);
tft.GotoXY(10, 10);
FontMicroPuts(&tft, (char*)"Текст", 1, 0);  //dx=1, без прозрачности
```

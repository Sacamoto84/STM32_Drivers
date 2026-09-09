
### 🏠 [← На главную](../README.md)

# Шрифтовые подсистемы (`Fonts`)

Библиотека **uTFT2** включает четыре различных движка для отрисовки шрифтов, каждый из которых оптимизирован под свои сценарии: от сверхлегких растровых до пропорциональных векторных и сглаженных антиалиасинг-шрифтов.

---

## 1. GFX-шрифты (`FontGFX`)

Пропорциональные шрифты в стиле Adafruit GFX. Позволяют выводить текст с произвольной шириной символов и базовой линией.

```cpp
#include "FontGFX/gfxfont.h"
#include "FontGFX/gfx_fonts_includes.h"

GFXFONT gfxfont;

void DrawTextGFX(void) {
    gfxfont.init(&tft);
    gfxfont.setFont(&FreeMono9pt7b);

    // Добавочный интервал между символами в пикселях (может быть отрицательным)
    gfxfont.set_delta_x(-1);

    tft.SetFontColor(WHITE, BLACK);
    tft.GotoXY(10, 30);
    
    // Поддерживаются строки UTF-8 с кириллицей и переносы строк \n
    gfxfont.Puts("Привет, STM32!\nВторая строка");
}
```

### Особенности FontGFX:
* **Кириллица:** функция `utf8rus2` автоматически конвертирует UTF-8 русский текст в коды глифов расширенной таблицы CP1251 (`0x80..0xC1`).
* **Перенос строк:** символ `\n` автоматически сбрасывает координату X в `0` и смещает Y вниз на высоту `yAdvance`.
* **Защита от сбоев:** символы, отсутствующие в шрифте, пропускаются с продвижением курсора на ширину пробела без повреждения памяти.

---

## 2. Сглаженные шрифты (`FontSmooth`)

Шрифты с субпиксельным сглаживанием (anti-aliasing) формата Processing `.vlw` с 4-битной маской прозрачности. Обеспечивают наивысшее типографическое качество на цветных экранах.

```cpp
#include "FontSmooth/FontSmooth.h"

void DrawSmoothText(void) {
    // Загрузка дескриптора шрифта из flash
    Font_Smooth_Load(_acRoboto_Medium_en_ru_24);

    tft.SetFontColor(YELLOW, BLACK);
    
    // Вывод текста со сглаживанием по альфа-каналу
    Font_Smooth_drawStr(&tft, 10, 40, "Roboto Сглаженный");
}
```

### Особенности FontSmooth:
* **Универсальность цветности:** благодаря вызовам `LineHW` и `alphaBlend` сглаженные шрифты корректно и безопасно работают на экранах любой битности (16, 8, 4 и 1 бит).
* **Поддержка UTF-8:** встроенный декодер преобразует русские символы в Unicode-индексы глифов.

---

## 3. Классические растровые шрифты (`FontClassic`)

Легковесные моноширинные растровые шрифты с фиксированной сеткой пикселей. Занимают минимум места во Flash и выводятся с максимальной скоростью.

```cpp
#include "FontClassic/FontClassic.h"

void DrawClassicText(void) {
    tft.SetFontColor(GREEN, BLACK);
    tft.GotoXY(10, 10);

    // Параметры: дисплей, строка, указатель на шрифт, NoBack (0 = рисовать фон, 1 = прозрачный фон)
    FontClassicPuts(&tft, (char*)"Status: OK\nTemp: 24C", &Font_11x18, 0);
}
```

### Доступные размеры:
* `Font_7x10` — 7x10 пикселей (компактный)
* `Font_11x18` — 11x18 пикселей (средний)
* `Font_16x26` — 16x26 пикселей (крупный)

Корректно обрабатываются символы `\n` (переход на новую строку с шагом `FontHeight`) и `\r`.

---

## 4. Шрифты MikroElektronika (`FontMicroElectronika`)

Шрифты формата MikroElektronika с раздельной поддержкой латиницы и кириллицы (CP1251).

```cpp
#include "FontMicroElectronika/FontMicroElectronika.h"

void DrawMicroText(void) {
    FontMicroSetFont(&Courier_12x18);
    
    tft.SetFontColor(CYAN, BLACK);
    tft.GotoXY(10, 10);

    // Параметры: дисплей, строка, dx (интервал), transparent (0 = с фоном, 1 = прозрачный)
    FontMicroPuts(&tft, (char*)"Привет Мир", 1, 0);

    // Точный расчет длины строки в пикселях с учетом интервала dx
    uint16_t textWidth = FontMicroFindLenStr((char*)"Привет Мир", &Courier_12x18, 1);
}
```

### Популярные шрифты:
* `Courier_12x18`
* `Unispace_10x19`
* `Arial_Narrow9x15`
* `Verdana39x32`
* `MicrosoftYaHeiUI12_20x22`, `Microsoft_YaHeiUI16_27x30`, `MicrosoftYaHeiUI24_41x45`


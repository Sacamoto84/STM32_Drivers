
# Макрос создания структуры описания экрана типа спрайт
Спрайт создается в памяти, и не выводит никуда изображение, но к нему применимы все методы работы с экраном и его можно обьеденить с текущим буффером другого экрана
 
Пока реализован только 16 бит спрайт
 
```cpp
#define CREATE_SPRITE16(name, x , y) u16 name##_buffer16[x * y + 4];\
TFT_LCD_t name={x , y, SPRITE_RAM, 16, NULL , NULL, 0, &name##_buffer16[0],};
```

```cpp
TFT Sprite; <- Это Экран спрайта

//Выделение памяти
CREATE_SPRITE16(S1, 32 , 32);
...
Sprite.init(&S1);
```

Со спрайтами можно производить некоторые трансформации

## Поместить спрайт на LCD
```
tft.copy(&Sprite, -32, 20);
tft.copyTr(&Cursor, TS[1], TS[2], BLACK);
```

## Вращение
Класс transformation (Transformation/transformation.h).
Точки вращения - публичные поля `_xPivot` / `_yPivot` класса TFT.

```
transformation trans(&tft);

Sprite._xPivot = -20;   //Точка вращения спрайта
Sprite._yPivot = 15;
tft._xPivot = 135 / 2;  //Куда на экране попадает точка вращения
tft._yPivot = 240 / 2;

trans.pushRotated(&tft, &Sprite, angle, BLACK);

tft.copy(&Sprite, 20, 20);
```

## Скроллинг
```
transformation transSpr(&Sprite);

Sprite.SetPixel(25, graphVal, BLUE);
transSpr.setScrollRect(0, 0, 32, 32, BLACK); //Зона скролла и цвет заполнения
transSpr.scroll(-1, 0);
```

//Данный файл переименовываем в TFT_config.h и помещаем в /inc

#ifndef INC_TFT_CONFIG_H_
#define INC_TFT_CONFIG_H_

//Выбор используемых интерфейсов
#define TFT_USE_SPI
//#define TFT_USE_I2C

//#define LCD_USE_BL


//Выбор используемого драйвера (макросы именно TFT_DRIVER_*)
//#define TFT_DRIVER_SSD1306
//#define TFT_DRIVER_ILI9225
#define TFT_DRIVER_ST7789
//#define TFT_DRIVER_ST7735      //реализации в uTFT2 нет
//#define TFT_DRIVER_ST7735S     //реализации в uTFT2 нет

//Глубина цвета фреймбуфера (1, 4, 8, 16 бит).
//Если не задана ни одна - доступны все варианты, выбор по LCD->Bit во время выполнения.
//#define TFT_USE_1BIT
//#define TFT_USE_4BIT
//#define TFT_USE_8BIT
//#define TFT_USE_16BIT
//Оптимизация: единственная глубина цвета, проверки LCD->Bit не генерируются
//#define TFT_USE_ONLY_ONE_BIT_COLOR

//Модули
//#define TFT_UNIT_VIDEO   //плеер видео с SD (Video/video.cpp), требует FAT_FS
//#define FAT_FS 1         //работа с SD (BMPFromFile*.cpp, Gif.h)
//#define TFT_USE_TIMBER   //логирование через timber (Gif.h, Video)
//#define TFT_USE_DWT      //замер времени через HiSpeedDWT (Gif.h)

#endif /* INC_TFT_CONFIG_H_ */

#ifndef RESOURCE_RESOURCE_H_
#define RESOURCE_RESOURCE_H_

#include "../Bitmap/bitmap.h"

extern void setResStartAdress(uint32_t i);

//Получить начало ресурсов
extern uint8_t * getResAdressFontID(uint32_t id);

//Получить дескриптор картинки из ресурсов 16 и 32 бит
extern Bitmap getResBitmapID(uint32_t id);

#endif /* RESOURCE_RESOURCE_H_ */

#include "resource.h"

//Для ресурсов
static uint32_t resurce_count = 0;
static uint32_t resurce_start_adress = 0; //Начало ресурсов

void setResStartAdress(uint32_t i) {
  uint32_t *p = (uint32_t *)i;
  resurce_start_adress = i;
  resurce_count = *p;               //Получить количество записей
}

//Получить начало ресурсов
uint8_t * getResAdressFontID(uint32_t id) {
	if (resurce_start_adress == 0) return NULL;
	if (id >= resurce_count) return NULL; //Выход за границы таблицы ресурсов

	uint32_t * offset;
	offset = (uint32_t *)(resurce_start_adress + 4 + (16*id) + 12);
	uint8_t * adress;
	adress = (uint8_t *)(resurce_start_adress + *offset);
	    return adress;
}

//Получить дескриптов картинки из ресурсов 16 и 32 бит
Bitmap getResBitmapID(int8_t id) {
    Bitmap bmp = {0, 0, 0, NULL, NULL, NULL, NULL};
    if (id < 0)
    {
      return bmp;
    }
    if (resurce_start_adress == 0) return bmp;
    if ((uint32_t)id >= resurce_count) return bmp; //Выход за границы таблицы ресурсов

    uint32_t *p;
    p = (uint32_t *)(resurce_start_adress + 4 + (16*id));
    bmp.W = (uint16_t)*p++;
    bmp.H = (uint16_t)*p++;
    bmp.bit =*p++;
    uint32_t * offset;
    offset = (uint32_t *)(resurce_start_adress + 4 + (16*id) + 12);
    bmp.data = (uint32_t *)(resurce_start_adress + *offset);
  	return bmp;
}

#include "resource.h"

//Для ресурсов
static uint32_t resurce_count = 0;
static uint32_t resurce_start_adress = 0; //Начало ресурсов

void setResStartAdress(uint32_t i) {
  if (i == 0 || (i & 3u) != 0) {
    resurce_start_adress = 0;
    resurce_count = 0;
    return;
  }
  uint32_t *p = (uint32_t *)i;
  // Защита от неинициализированной/стертой flash (0xFFFFFFFF)
  if (*p > 65535) {
    resurce_start_adress = 0;
    resurce_count = 0;
    return;
  }
  resurce_start_adress = i;
  resurce_count = *p;               //Получить количество записей
}

//Получить начало ресурсов
uint8_t * getResAdressFontID(uint32_t id) {
	if (resurce_start_adress == 0) return NULL;
	if (id >= resurce_count) return NULL; //Выход за границы таблицы ресурсов

	uint32_t * offset;
	offset = (uint32_t *)(resurce_start_adress + 4 + (16*id) + 12);
	if (*offset == 0) return NULL; //Пустая или поврежденная запись
	uint8_t * adress;
	adress = (uint8_t *)(resurce_start_adress + *offset);
	return adress;
}

//Получить дескриптор картинки из ресурсов 16 и 32 бит
Bitmap getResBitmapID(uint32_t id) {
    Bitmap bmp = {0, 0, 0, NULL, NULL, NULL, NULL};
    if (resurce_start_adress == 0) return bmp;
    if (id >= resurce_count) return bmp; //Выход за границы таблицы ресурсов

    uint32_t * offset;
    offset = (uint32_t *)(resurce_start_adress + 4 + (16*id) + 12);
    if (*offset == 0) return bmp; //Пустая или поврежденная запись

    uint32_t *p;
    p = (uint32_t *)(resurce_start_adress + 4 + (16*id));
    bmp.W = (uint16_t)*p++;
    bmp.H = (uint16_t)*p++;
    bmp.bit =*p++;
    bmp.data = (uint32_t *)(resurce_start_adress + *offset);
    //Рендеры (Alpha, Background16bit, Transparent) читают steam16/steam32 -
    //заполняем их тем же адресом данных (раньше оставались NULL)
    bmp.steam16 = (unsigned short *)bmp.data;
    bmp.steam32 = bmp.data;
  	return bmp;
}

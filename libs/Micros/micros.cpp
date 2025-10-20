#include "micros.h"

static TIM_HandleTypeDef * _htim;
static volatile uint32_t * temp;

uint32_t us_count_tic;

void DWT_Init(void)
{
    SCB_DEMCR   |= CoreDebug_DEMCR_TRCENA_Msk; // разрешаем использовать счётчик
    DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk;     // запускаем счётчик
}

void micros_init(TIM_HandleTypeDef * htim){
	_htim = htim;
	temp = &_htim->Instance->CNT;
	*temp = 0;}

uint32_t micros(void){	return *temp;}


//DWT_CYCCNT = 0;// обнуляем счётчик
// здесь кусок измеряемого участка программы
//count_tic = DWT_CYCCNT; // кол-во тактов

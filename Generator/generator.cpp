#include "Generator.h"
#include "tim.h"

#include "HiSpeedDWT.h"
extern HiSpeedDWT TimerT5;

#include "timber.h"

uint32_t OUTPUT[OUTPUTSAMPLES] RAM_16;

FIL SDFile1 CCMRAM;

//Функция рендерит carrier из файла "/Carrier/_CH->Carrier_mod"
void GENERATOR::Create_Carrier(_structure_ch *_CH) {
	char str[40] = { 0 };

	//По индексу определяем имя файла, его открываем и записываем во временный буффер
	sprintf(str, "/Carrier/%s", _CH->Carrier_mod);

	if (f_open(&SDFile, str, FA_OPEN_EXISTING | FA_READ) == FR_OK) {
		timber.print(">Open OK %s\n", str);
		UINT testByte;
		f_read(&SDFile, &buffer_temp, 2048, &testByte);
		f_close(&SDFile);
	} else
	    timber.print(">Open Error %s\n", str);

	uint16_t *p;
    p = (uint16_t*)&buffer_temp[0];

	for (uint16_t i = 0; i < 1024; i++)
		_CH->buffer_carrier[i] = *p++;

}

//Прочечть файл и заполнить массив AM модуляции "/Mod/_CH->AM_mod"
void GENERATOR::Create_AM_Modulation(_structure_ch *_CH) {

	//SEGGER_RTT_printf(0, "┌─ Create_AM_Modulation\n");

	char str[40] = { 0 };

	//По индексу определяем имя файла, его открываем и записываем во временный буффер
	sprintf(str, "/Mod/%s", _CH->AM_mod);

	uint16_t i;

	if (f_open(&SDFile, str, FA_OPEN_EXISTING | FA_READ) == FR_OK) {
		//SEGGER_RTT_printf(0, "└> OK %s\n", str);
		UINT testByte;
		f_read(&SDFile, &buffer_temp, 2048, &testByte);
		f_close(&SDFile);
	} else
	    timber.print("└> Ошибка %s\n", str);

	uint16_t *p;
    p = (uint16_t*)&buffer_temp[0];

    //14us
	for (i = 0; i < 1024; i++){
		_CH->buffer_am[i] = *p++;}
}

//Прочесть файл FM модуляции и записать в буффер "/Mod/_CH->FM_mod"
void GENERATOR::Create_FM_Modulation(_structure_ch *_CH) {

    timber.print("\033[01;38;05;232;48;05;201mGEN\x1B[0m>Create_FM_Modulation CH%d\n", _CH->CH + 1);

	float x, y;
	//UINT *p;
	char str[40] = { 0 };

	x = _CH->FM_Base - _CH->FM_Dev;
	y = _CH->FM_Dev * 2;

	sprintf(str, "/Mod/%s", _CH->FM_mod);

	uint16_t i;

	if (f_open(&SDFile, str, FA_OPEN_EXISTING | FA_READ) == FR_OK) {
		timber.print(">Open OK %s\n", str);
		UINT testByte;

		if (_CH->CH == 0)
		  f_read(&SDFile, &cacheFM1, 2048, &testByte);
		else
		  f_read(&SDFile, &cacheFM2, 2048, &testByte);

		f_close(&SDFile);
	} else
        timber.print(">Open Error %s\n", str);
	///////////////////
	uint16_t *p;
	if (_CH->CH == 0)
	  p = (uint16_t*)&cacheFM1[0];
	else
	  p = (uint16_t*)&cacheFM2[0];

	float buffer[FM_MODSAMPLES];

	 //40us
	for (i = 0; i < 512; i++) {
		//_CH->buffer_fm[i] = x + (y * (float)*p / 4095.0F);
		buffer[i] = x + (y * (float)*p / 4095.0F);
		p += 2;
	}

	//7 us
	uint32_t *f1 = (uint32_t*)&_CH->buffer_fm[0];
	uint32_t *f2 = (uint32_t*)&buffer[0];
	for (i = 0; i < 512/4/2/2; i++) {
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
	}

	//TimerT5.Loger((char *)"FM1rr");

}

//Прочесть файл FM модуляции и записать в буффер "/Mod/_CH->FM_mod"
void GENERATOR::Refresh_FM_Modulation(_structure_ch *_CH) {

    timber.print("\033[01;38;05;232;48;05;68mGEN\x1B[0m>Refresh_FM_Modulation CH%d\n", _CH->CH + 1);

	float x, y;

	x = _CH->FM_Base - _CH->FM_Dev;
	y = _CH->FM_Dev * 2;

	uint16_t i;

	uint16_t *p;
	if (_CH->CH == 0)
	  p = (uint16_t*)&cacheFM1[0];

	else
	  p = (uint16_t*)&cacheFM2[0];

	float buffer[FM_MODSAMPLES];

	 //40us
	for (i = 0; i < 512; i++) {
		//_CH->buffer_fm[i] = x + (y * (float)*p / 4095.0F);
		buffer[i] = x + (y * (float)*p / 4095.0F);
		p += 2;
	}

	//7 us
	uint32_t *f1 = (uint32_t*)&_CH->buffer_fm[0];
	uint32_t *f2 = (uint32_t*)&buffer[0];
	for (i = 0; i < 512/4/2/2; i++) {
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
		*f1++ = *f2++;
	}
}



void GENERATOR::Init(void) {

	//CH1.CH = 0;
	//CH2.CH = 1;

	//Читаем настройки из файла save.ini и заполняем структуру CH1 и CH2
	//read_save_ini_to_structure_ch();

	//Create_Carrier(&CH1);
	//Create_Carrier(&CH2);

	//Create_AM_Modulation(&CH1);
	//Create_AM_Modulation(&CH2);

	//Create_FM_Modulation(&CH1);
	//Create_FM_Modulation(&CH2);






	/**************************
	 *  DMA DAC
	 **************************/
	DAC1->CR |= DAC_CR_DMAEN1 | DAC_CR_EN1 | DAC_CR_DMAEN2 | DAC_CR_EN2;
	//HAL_DMA_Start_IT(&hdma_dac1, 0x2001C000, 0x40007420, 4096);

	DMA1_Stream5->CR &= ~DMA_SxCR_EN; //Выключаем DMA
	DMA1_Stream5->NDTR = OUTPUTSAMPLES / 2;
	DMA1_Stream5->PAR = 0x40007420; //DAC 12R
	DMA1_Stream5->M0AR = (uint32_t) &OUTPUT; //(int)pOUTPUT;
	DMA1_Stream5->M1AR = (uint32_t) &OUTPUT + OUTPUTSAMPLES * 2; //(int)pOUTPUT;
	//DMA1_Stream5->CR |=  DMA_SxCR_CIRC;// | DMA_SxCR_HTIE;
	////DMA1_Stream5->CR &= ~DMA_SxCR_TCIE;
	DMA1_Stream5->CR |= DMA_SxCR_TCIE | DMA_SxCR_CIRC | DMA_SxCR_DBM;// DMA_SxCR_HTIE; //Прерывание | Циклический режим | //Двойной буффер
	DMA1_Stream5->CR |= DMA_SxCR_EN;

	HAL_TIM_Base_Start(&htim6);  //Запуск таймера6 для DAC
}

void GENERATOR::syg_update(int Buffer_Current) {
	volatile static uint16_t O1;
	volatile static uint16_t O2;

	volatile static uint32_t i;
	volatile static uint32_t i_max;

	if (Buffer_Current) {
		i = OUTPUTSAMPLES / 2;
		i_max = OUTPUTSAMPLES;
	} else {
		i = 0;
		i_max = OUTPUTSAMPLES / 2;
	}

	CH1.rC = convertHzToR(CH1.Carrier_fr);
	CH2.rC = convertHzToR(CH2.Carrier_fr);
	CH1.rAM = convertHzToR(CH1.AM_fr);
	CH2.rAM = convertHzToR(CH2.AM_fr);
	CH1.rFM = convertHzToR(CH1.FM_mod_fr);
	CH2.rFM = convertHzToR(CH2.FM_mod_fr);

	CH1.Volume = 0.65F;
	CH2.Volume = 0.65F;

	for (i; i < i_max; i++) {
		if  (CH1.CH_EN)
		{
		if (CH1.FM_EN) {
			CH1.phase_accumulator_fm = CH1.phase_accumulator_fm + CH1.rFM;
			CH1.phase_accumulator_carrier =	CH1.phase_accumulator_carrier + (uint32_t) ((CH1.buffer_fm[CH1.phase_accumulator_fm	>> 23]) * 16384.0F / 3.798F * 2.0F);
		} else
			CH1.phase_accumulator_carrier += CH1.rC;

		if (CH1.AM_EN) {
			CH1.phase_accumulator_mod = CH1.phase_accumulator_mod + CH1.rAM;
			O1 = 2048 + CH1.Volume * (CH1.buffer_carrier[CH1.phase_accumulator_carrier >> 22] - 2048.F)	* (CH1.buffer_am[CH1.phase_accumulator_mod >> 22]/ 4095.0F);
		} else
			O1 = 2048 + CH1.Volume * (CH1.buffer_carrier[CH1.phase_accumulator_carrier>> 22] - 2048.0F);
		}
		else
			O1 = 2048;

		if  (CH2.CH_EN)
		{
		if (CH2.FM_EN) {
			CH2.phase_accumulator_fm = CH2.phase_accumulator_fm + CH2.rFM;
			CH2.phase_accumulator_carrier =	CH2.phase_accumulator_carrier + (uint32_t) ((CH2.buffer_fm[CH2.phase_accumulator_fm	>> 23]) * 16384.0F / 3.798F * 2.0F);
		} else
			CH2.phase_accumulator_carrier += CH2.rC;

		if (CH2.AM_EN) {
			CH2.phase_accumulator_mod = CH2.phase_accumulator_mod + CH2.rAM;
			O2 = 2048 + CH2.Volume * (CH2.buffer_carrier[CH2.phase_accumulator_carrier >> 22] - 2048.F)	* (CH2.buffer_am[CH2.phase_accumulator_mod >> 22]/ 4095.0F);
		} else
			O2 = 2048 + CH2.Volume * (CH2.buffer_carrier[CH2.phase_accumulator_carrier>> 22] - 2048.0F);
		}
		else
			O2 = 2048;

		OUTPUT[i] = (O2 << 16) | O1;

	}

}

void GENERATOR::read_save_ini_to_structure_ch(void)
{
	/*

	 timber.print("\x1B[38;05;10m>read_save_ini_to_structure_ch()\n");

	//char str[16];

	const size_t bufferLen = 80;
	char buffer[bufferLen];

	const char *filename = "/Config/save.ini";

	IniFile ini(filename);

	if (!ini.open()) {
		 timber.print("Ini файл /Config/save.ini does не найден\r\n");
	    // Cannot do anything else
	    while (1)
	      ;
	  }
	 timber.print("Ini /Config/save.ini файл найден\n");

    if (ini.getValue("CH1", "CR En", buffer, bufferLen, CH1.CH_EN))
    	 timber.print("%sCH1.CR EN   =  %s\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH1] key 'CR En'\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH1", "CR Fr", buffer, bufferLen, CH1.Carrier_fr))
    	 timber.print("%sCH1.CR Fr   =  %s\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else { timber.print("%sError [CH1] key 'CR Fr'\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH1", "CR Mod", buffer, bufferLen, &CH1.Carrier_mod[0], 20))
    	 timber.print("%sCH1.CR Mod  =  %s \r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH1] key 'CR Mod'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH1", "AM En", buffer, bufferLen, CH1.AM_EN))
    	 timber.print("%sCH1.CR EN   =  %s \r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH1] key 'AM En'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH1", "AM Fr", buffer, bufferLen, CH1.AM_fr))
    	 timber.print("%sCH1.AM Fr   =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH1] key 'AM Fr'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH1", "AM Mod", buffer, bufferLen, &CH1.AM_mod[0],20))
    	 timber.print("%sCH1.AM Mod  =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH1] key 'AM Mod'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH1", "FM En", buffer, bufferLen, CH1.FM_EN))
    	 timber.print("%sCH1.FM En   =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH1] key 'FM En'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH1", "FM Base", buffer, bufferLen, CH1.FM_Base))
    	 timber.print("%sCH1.FM Base =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH1] key 'FM Base'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH1", "FM Div", buffer, bufferLen, CH1.FM_Dev))
    	 timber.print("%sCH1.FM Div  =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH1] key 'FM Div'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH1", "FM Fr", buffer, bufferLen, CH1.FM_mod_fr))
    	 timber.print("%sCH1.FM Fr   =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH1] key 'FM Fr'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH1", "FM Mod", buffer, bufferLen, &CH1.FM_mod[0], 20))
        timber.print("%sCH1.FM Mod  =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH1] key 'FM Mod'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

////////////////
    timber.print("%s-----------------------------'\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN);

    if (ini.getValue("CH2", "CR En", buffer, bufferLen, CH2.CH_EN))
    	 timber.print("%sCH2.CR EN   =  %s \r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH2] key 'CR En'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH2", "CR Fr", buffer, bufferLen, CH2.Carrier_fr))
    	 timber.print("%sCH2.CR Fr   =  %s \r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH2] key 'CR Fr'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH2", "CR Mod", buffer, bufferLen, &CH2.Carrier_mod[0], 20))
    	 timber.print("%sCH2.CR Mod  =  %s \r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH2] key 'CR Mod'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH2", "AM En", buffer, bufferLen, CH2.AM_EN))
    	 timber.print("%sCH2.CR EN   =  %s \r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH2] key 'AM En'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH2", "AM Fr", buffer, bufferLen, CH2.AM_fr))
    	 timber.print("%sCH2.AM Fr   =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH2] key 'AM Fr'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH2", "AM Mod", buffer, bufferLen, &CH2.AM_mod[0],20))
    	 timber.print("%sCH2.AM Mod  =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else { timber.print("%sError [CH2] key 'AM Mod'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH2", "FM En", buffer, bufferLen, CH2.FM_EN))
    	 timber.print("%sCH2.FM En   =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH2] key 'FM En'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH2", "FM Base", buffer, bufferLen, CH2.FM_Base))
    	 timber.print("%sCH2.FM Base =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH2] key 'FM Base'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH2", "FM Div", buffer, bufferLen, CH2.FM_Dev))
    	 timber.print("%sCH2.FM Div  =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH2] key 'FM Div'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH2", "FM Fr", buffer, bufferLen, CH2.FM_mod_fr))
    	 timber.print("%sCH2.FM Fr   =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer);
    else {  timber.print("%sError [CH2] key 'FM Fr'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    if (ini.getValue("CH2", "FM Mod", buffer, bufferLen, &CH2.FM_mod[0], 20))
    	 timber.print("%sCH2.FM Mod  =  %s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN , buffer );
    else {  timber.print("%sError [CH2] key 'FM Mod'\r\n", RTT_CTRL_TEXT_BRIGHT_RED); ini.printErrorMessage(); }

    ini.close();

 */

}

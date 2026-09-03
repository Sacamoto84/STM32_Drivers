#include "main.h"

#include "BLE.h"

extern DMA_HandleTypeDef hdma_usart3_tx;

char str2[32];

extern void BLE_Code_98(int PosS, int len);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Кольцевой буффер
////////////////////
//https://chipenable.ru/index.php/programming-avr/44-uchebnyy-kurs-organizatsiya-obmena-po-usart-u-s-ispolzovaniem-koltsevogo-bufera.html

void BLE::init(UART_HandleTypeDef *rx) {
	//Log.i((char*)"Инициализация BLE");

	_huart = rx;

}

//////////////////////////////////////
//Очистка буффера
//////////////////////////////////////
void BLE::FlushBuf(void) {
	tail = 0;
	head = 0;
	count = 0;
}

//////////////////////////////////////
//Положить символ в буффер
//////////////////////////////////////
void BLE::PutChar(unsigned char sym) {
	if (count < SIZE_BUF) {                //если в буфере еще есть место
		cycleBuf[tail] = sym;             //помещаем в него символ
		count++;                          //инкрементируем счетчик символов
		tail++;                           //и индекс хвоста буфера
		if (tail == SIZE_BUF)
			tail = 0;
	}
}

//////////////////////////////////////
//Взять символ из буффера
//////////////////////////////////////
unsigned char BLE::GetChar(void) {
	unsigned char sym = 0;
	if (count > 0) {                            //если буфер не пустой
		sym = cycleBuf[head];              //считываем символ из буфера
		count--;                                   //уменьшаем счетчик символов
		head++;                            //инкрементируем индекс головы буфера
		if (head == SIZE_BUF)
			head = 0;
	}
	return sym;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BLE_UART_Decode_Level_1(int PosS, int PosCRC);

uint32_t BLE_UART_Timer;               //50 ms inc timer
uint32_t BLE_UART_Timer_Last;          //50 ms inc timer

void BLE_UART_Decode(void);

volatile float ii19 = 0.1F;
char buffer20[30];
char buffer30[30];
volatile uint8_t i19 = 0;

volatile uint32_t crc = 0xaa;

//void DMA2_Stream6_IRQHandler(void)
//{
//  HAL_DMA_IRQHandler(&hdma_usart6_tx);
//}

void BLE::USART_RX_IRQHandler(void) {
	static uint8_t USART_data;

	if ((_huart->Instance->SR) & USART_SR_ORE) { }

	//При приеме помещаем в буффер данные
	if (
			(_huart->Instance->SR & USART_SR_RXNE)
			//&& ((_huart->Instance->SR & USART_SR_ORE) == 0)
	)
	{
		USART_data = _huart->Instance->DR;
		PutChar(USART_data);          //принятый байт

		if (USART_data == 0x24) //$
		{
			fPacketEnd = 1;
			countCommand++;
		}

	}

	//HAL_UART_IRQHandler(_huart);

}

//Возвращает количество байт в буффере
uint16_t BLE::available(void) {
	return count;
}

//Прочитать один байт с буффера
uint8_t BLE::read(void) {
	return GetChar();
}

void BLE::BLE_UART_Decode(void) {
	volatile int i;
	volatile int Max_Big_Buf;
	volatile int PosS, PosE, PosCRC;
	volatile uint8_t temp;

	local_crc = 0;
	str_crc[0] = 0x30;
	str_crc[1] = 0x30;
	str_crc[2] = 0x30;

	PosS = -1;         //Стартовая позиция Начала пакета !
	PosE = -1;         //Стартовая позиция Конца пакета  $
	PosCRC = -1;       //Стартовая позиция Начала CRC    ;

	//Заполняем большой 8K буффер из кольцевого до символа $ вклюительно
	i = 0;
	do {
		temp = GetChar();
		big_buffer[i++] = temp;
		if(i>4096) return;
	} while (temp != '$');

	PosE = i - 1; //Позиция конца пакета

	//Level 0
	Max_Big_Buf = i; //Максимальный индекс в буффере
	big_buffer[Max_Big_Buf] = 0;

	if (PosE < 64) {
		//Log.i("%s", big_buffer);
	}

	//Ищем индекс начала пакета
	for (i = 0; i < Max_Big_Buf; i++) {
		if (big_buffer[i] == '!') {
			PosS = i;
			break;
		}
	}

	//Если нет начала пакета
	if (PosS == -1) {
		//Log.e("L0 > Нет начала пакета > PosS == -1");
		return;
	}
	//Есть начало и конец

	//Ищем символ начала CRC
	for (i = PosS; i < PosE; i++) {
		if (big_buffer[i] == ';')
			PosCRC = i;
	}

	//Если нет начала CRC
	if (PosCRC == -1) {
		//Log.e("L0 > Нет начала CRC > PosCRC == -1\n");
		return;
	}

	//Нашли начало CRC
	if ((PosE - PosCRC) > 4) {
		//Log.e((char *)"L0 > Error > (PosE - PosCRC) > 4\n");
		return;
	}

	//Читаем сиволы CRC
	temp = PosE - PosCRC;
	switch (temp) {
	case 2:
		str_crc[2] = big_buffer[PosCRC + 1];
		break;
	case 3:
		str_crc[1] = big_buffer[PosCRC + 1];
		str_crc[2] = big_buffer[PosCRC + 2];
		break;
	case 4:
		str_crc[0] = big_buffer[PosCRC + 1];
		str_crc[1] = big_buffer[PosCRC + 2];
		str_crc[2] = big_buffer[PosCRC + 3];
		break;
	}

	//Вычеляем значение CRC
	local_crc = (str_crc[0] - 0x30) * 100 + (str_crc[1] - 0x30) * 10 + (str_crc[2] - 0x30);

	//Расчитываем CRC
	temp = CRC8(&big_buffer[PosS + 1], PosCRC - PosS - 1);

	if (local_crc != temp) {
		//Log.e("L0 > Error calculate CRC In:%d != Calc:%d\n", local_crc, temp);
		return;
	}

	//Level 0 ок
	//Передаем управление Level1
	//Передаем позицию начала и CRC
	//BLE_UART_Decode_Level_1(PosS, PosCRC);

	char comand[64] = {0};

	int ii = 0;
	for (i = PosS + 1; i < PosCRC; i++) {
		comand[ii]=big_buffer[i];
		ii++;
	}

	//Log.i(comand);


	if (MyCallback) {
	  MyCallback(comand);
	}

}

void BLE::Task(void) {

	if (fPacketEnd) {
		fPacketEnd = 0;
		//Log.w("Принят символ конец пакета %d", countCommand);
	}

	while (countCommand) {
		countCommand--;
		BLE_UART_Decode();
	}
}

/*
 Name  : CRC-8
 Poly  : 0x31    x^8 + x^5 + x^4 + 1
 Init  : 0xFF
 Revert: false
 XorOut: 0x00
 Check : 0xF7 ("123456789")
 MaxLen: 15 байт(127 бит) - обнаружение
 одинарных, двойных, тройных и всех нечетных ошибок
 */
uint8_t BLE::CRC8(char *pcBlock, unsigned int len) {
	unsigned char _crc = 0xFF;
	unsigned int i;

	while (len--) {
		_crc ^= *pcBlock++;

		for (i = 0; i < 8; i++)
			_crc = _crc & 0x80 ? (_crc << 1) ^ 0x31 : _crc << 1;
	}
	return _crc;
}


//Процедура формирования строки пересылки(короткий) 64
void BLE::Send(char *outstr) {
	char str[1024]={0};
	char crc;
	crc = CRC8(&outstr[0], strlen(outstr));
	sprintf(str, "!%s;%d$", outstr, crc);
	//Log.s(str);
	HAL_UART_Transmit(_huart, (uint8_t*) &str[0], strlen(str), 1000);
}



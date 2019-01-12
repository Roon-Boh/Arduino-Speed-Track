// Author © 2018-2019 Sergey Kordubin. Contacts: <root@roon-art.ru>
// License: https://opensource.org/licenses/GPL-3.0
// Source: https://github.com/Roon-Boh/Arduino-Speed-Track.git


#define DEF_IP 1 // Задаем адрес устройства 21.23.25.27.29 нечет для варот 1 , 20,22,24,26,28 чет для ворот 2
#define TEST false

#define ONOFF_PINS 12 // Пин кнопки включения в норме подтянут к +5В
#define SW2_PINS 2 // Пин кнопки 
#define SW3_PINS 6 // Пин кнопки 
#define SW4_PINS 4 // Пин кнопки 
//#define ID_TYPE   199 // Задаем тип устройства 199 - доплер


uint8_t const ANODS_PINS[3] = {9, 10, 11}; // Задаем пины для кажого разряда
uint8_t const SEGMENTS_PINS[7] = {8, 7, 6, 5, 4, 3, 2}; //Задаем пины для каждого сегмента
boolean led;
int32_t tempus;
int32_t tempus1;
uint32_t led_time = 0; // таймер активности millis() + 10000.
//0 - сотни, 1 - десятки, 2 - единицы, 3 - сумировать и сбросить
int8_t u8dig_count = 0; // указатель очереди считывания разряда
uint8_t reed_dig_count = 0; // счетчик суммарных считываний разрядов.
uint8_t max_reed_dig_count = 10;// максимальное кол-во считывания каждого из разрядов. 
uint8_t reed_dig[4][11] = {0}; // массив данных для считывания дисплея.
uint16_t au16data[4]; // массив данных для ModBus RTU RS-485


/**
 *  Setup procedure
 */
void setup() {
  //  инициализация портов ввода вывода для считывания сегментов
  ioSetup();
  
  // Управление включением SW_1
  pressStart();


  digitalWrite(13, HIGH ); // индикатор работы

  // start communications
  tempus = millis() + 100;
  digitalWrite(13, HIGH );
}

/**
 *  Loop procedure
 */
void loop() {


  
  // poll messages
  // blink led pin on each valid message

  
  
  int8_t u8summ = getDisplay();
  // diagnose communication
  


  
  
}//END loop()_________________________________________________________________END loop()



int getDisplay() {
  if(u8dig_count < 3){
    if(!digitalRead(ANODS_PINS[u8dig_count])){
      int8_t u8getseg = getSegments();
      if(u8getseg < 10){
        reed_dig[u8dig_count][reed_dig_count] = u8getseg;
        u8dig_count ++;
        return true;
      }
      else if (9 < u8getseg){
        for(int8_t u8i100 = 0; u8i100 < 4 ; u8i100++){
          for(int8_t u8c100 = 0; (u8c100 < (max_reed_dig_count + 1)) ; u8c100++){
            reed_dig[u8i100][u8c100] = 0;
          }
        }
        u8dig_count = 0;
        reed_dig_count = 0;
        return 0xFF;
      }
    }
    return false;
  }
  else {
    for(int8_t u8is = 0; u8is < max_reed_dig_count; u8is++){
      reed_dig[3][u8is] = ((reed_dig[0][u8is]*100)+(reed_dig[1][u8is]*10)+reed_dig[2][u8is]);
      if(reed_dig[3][max_reed_dig_count] < reed_dig[3][u8is]){
        reed_dig[3][max_reed_dig_count] = reed_dig[3][u8is];
      }
    }
    int8_t u8retsumm = reed_dig[3][max_reed_dig_count];
    for(int8_t u8i100 = 0; u8i100 < 4 ; u8i100++){
      for(int8_t u8c100 = 0; (u8c100 < (max_reed_dig_count + 1)) ; u8c100++){
        reed_dig[u8i100][u8c100] = 0;
      }
    }
    u8dig_count = 0;
    reed_dig_count = 0;
    return u8retsumm;
  }
}




//
// функция извлекает символ в разряде дисплея и возвращает его или 100 в случае если дисплей не горит
//
int getSegments() {
  uint8_t buf_gd = 0;
  for (uint8_t i_gd = 0; i_gd < 7; i_gd++) {
    bitWrite(buf_gd, (i_gd), !digitalRead(SEGMENTS_PINS[i_gd]));
    if (i_gd == 6) {
      bitWrite(buf_gd, 7, 0);
    }
  }
  switch (uint8_t(buf_gd)) {
    case B1111110: //Цифра 0  //126   //0x7E
      return 0;
    case B0110000: //Цифра 1
      return 1;
    case B1101101: //Цифра 2
      return 2;
    case B1111001: //Цифра 3
      return 3;
    case B0110011: //Цифра 4
      return 4;
    case B1011011: //Цифра 5
      return 5;
    case B1011111: //Цифра 6
      return 6;
    case B1110000: //Цифра 7
      return 7;
    case B1111111: //Цифра 8
      return 8;
    case B1111011: //Цифра 9
      return 9;
    case B0000000: //Пусто
      return 0;
    case B1100000: //1 1-ого р-да  //96  //0x60
      return 1;
    case B0001000: // LED1
      return 103;
    case B0010000: // LED2
      return 104;
    default:
      return 100;
  }
}

void pressStart(){
  int32_t u32tonoff = (millis() + 6000);
  for(; millis() < u32tonoff;){
    if(!digitalRead(ANODS_PINS[0]) && digitalRead(ANODS_PINS[1])){
      u32tonoff = 0;
    }
  }
  if(0 < u32tonoff){
    pinMode(ONOFF_PINS, OUTPUT);
    pinMode(SEGMENTS_PINS[3], OUTPUT);
    digitalWrite(ONOFF_PINS, LOW);
    digitalWrite(SEGMENTS_PINS[3], LOW);
    digitalWrite(13, HIGH );
    for(int32_t u32ton = (millis() + 1000); millis() < u32ton;){ }
    digitalWrite(ONOFF_PINS, LOW);
    digitalWrite(13, LOW );
    pinMode(ONOFF_PINS, INPUT);
    pinMode(SEGMENTS_PINS[3], INPUT_PULLUP);
    for(int32_t u32ton = (millis() + 1000); millis() < u32ton;){ }
    pinMode(SW2_PINS, OUTPUT);
    pinMode(SW4_PINS, OUTPUT);
    digitalWrite(13, HIGH );
    digitalWrite(SW2_PINS, LOW );
    digitalWrite(SW4_PINS, LOW );
    for(int32_t u32ton = (millis() + 2000); millis() < u32ton;){ }
    pinMode(SW2_PINS, INPUT_PULLUP);
    pinMode(SW4_PINS, INPUT_PULLUP);
    digitalWrite(13, LOW );
    for(int32_t u32ton = (millis() + 1000); millis() < u32ton;){ }
    pinMode(SW4_PINS, OUTPUT);
    digitalWrite(SW4_PINS, LOW);
    digitalWrite(13, HIGH);
    for(int32_t u32ton = (millis() + 1000); millis() < u32ton;){ }
    pinMode(SW4_PINS, INPUT_PULLUP);
    digitalWrite(13, LOW);
  }
  else {
    for(int8_t countblink = 0; countblink < 3; countblink++){
      for(int32_t u32ton = (millis() + 500); millis() < u32ton;){ }
      digitalWrite(13, HIGH);
      for(int32_t u32ton = (millis() + 100); millis() < u32ton;){ }
      digitalWrite(13, LOW);
      for(int32_t u32ton = (millis() + 100); millis() < u32ton;){ }
      digitalWrite(13, HIGH);
      for(int32_t u32ton = (millis() + 100); millis() < u32ton;){ }
      digitalWrite(13, LOW);
    }
  }
}



  void ioSetup() 
  {
    // Аноды-Сегменты от A до G) дисплея
    pinMode(SEGMENTS_PINS[0], INPUT_PULLUP); // SEG_A      SW_2
    pinMode(SEGMENTS_PINS[1], INPUT_PULLUP); // SEG_B
    pinMode(SEGMENTS_PINS[2], INPUT_PULLUP); // SEG_C LD_2 SW_4
    pinMode(SEGMENTS_PINS[3], INPUT_PULLUP); // SEG_D LD_1 SW_1
    pinMode(SEGMENTS_PINS[4], INPUT_PULLUP); // SEG_E      SW_3
    pinMode(SEGMENTS_PINS[5], INPUT_PULLUP); // SEG_F
    pinMode(SEGMENTS_PINS[6], INPUT_PULLUP); // SEG_G
  
    // Разряды-Катоды дисплея
    pinMode(ANODS_PINS[0], INPUT);   // DIG_1 (Led)
    pinMode(ANODS_PINS[1], INPUT);  // DIG_2
    pinMode(ANODS_PINS[2], INPUT);  // DIG_3
  }

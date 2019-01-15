#include <SPI.h>
#include <UIPEthernet.h>
#include <utility/logging.h>

// Author © 2018-2019 Sergey Kordubin. Contacts: <root@roon-art.ru>
// License: https://opensource.org/licenses/GPL-3.0
// Source on GitHub: https://github.com/Roon-Boh/Arduino-Speed-Track.git
//
//
//
//Cathodes-Segments Sensor A to G
//_PIN  PIN     Array[]         
//  8    8    SEGMENTS[0]    SEG_A
//  7    7    SEGMENTS[1]    SEG_B
//  6    6    SEGMENTS[2]    SEG_C, SW_3  (Catod LD_2) (Pin on the "ReCall" button)
//  5    5    SEGMENTS[3]    SEG_D, SW_1  (Catod LD_1) (Pin via resistor to the ON/Off button)
//  4    4    SEGMENTS[4]    SEG_E, SW_4  (Pin on the button "Km/H")
//  3    3    SEGMENTS[5]    SEG_F
//  2    2    SEGMENTS[6]    SEG_G, SW_2  (Pin on the "MPH" button)
// 
//Digits-Anodes Sensor
//_PIN  PIN    Array[]           
//  9    A0    DIG[0]    DIG_1 (Hundreds), (LED Anode)
// 10    A1    DIG[1]    DIG_2 (Tens)
// 11    A2    DIG[2]    DIG_3 (Units) 
//
//
//




#define ON_PIN 9 // Пин кнопки включения в норме подтянут к +5В
#define SW_2 2 // Пин кнопки 
#define SW_3 6 // Пин кнопки 
#define SW_4 4 // Пин кнопки 



uint8_t const DIG[3] = {A2, A1, A0}; // Задаем пины для кажого разряда
uint8_t const SEGMENTS[7] = {8, 7, 6, 5, 4, 3, 2}; //Задаем пины для каждого сегмента
int8_t u8dig_count = 0; // указатель очереди считывания разряда
uint8_t reed_dig_count = 0; // счетчик суммарных считываний разрядов.
uint8_t max_reed_dig_count = 10;// максимальное кол-во считывания каждого из разрядов. 
//0 - сотни, 1 - десятки, 2 - единицы, 3 - сумировать и сбросить
uint8_t reed_dig[4][11] = {0}; // массив данных для считывания дисплея.


// определяем конфигурацию сети
byte mac[] = {0xAE, 0xB2, 0x26, 0xE4, 0x4A, 0x5C}; // MAC-адрес
//byte ip[] = {192, 168, 0, 10}; // IP-адрес
//byte myDns[] = {192, 168, 0, 1}; // адрес DNS-сервера
//byte gateway[] = {192, 168, 0, 1}; // адрес сетевого шлюза
//byte subnet[] = {255, 255, 255, 0}; // маска подсети

EthernetServer server(2000); // создаем сервер, порт 2000
EthernetClient client; // объект клиент
boolean clientAlreadyConnected= false; // признак клиент уже подключен

/**
 *  Setup procedure
 */
void setup() {
  //  инициализация портов ввода вывода для считывания сегментов
  Serial.begin(9600);
  ioSetup();
  
  // Управление включением SW_1
  pressStart();

  
  //Ethernet.begin(mac, ip, myDns, gateway, subnet); // инициализация контроллера
  // инициализация контроллера
  Serial.println("Getting IP address using DHCP");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure using DHCP");
    while(true) ; // зависаем по ошибке
  }


  // вывод IP адреса 
  Serial.print("IP address: "); 
  IPAddress ip = Ethernet.localIP();
  for (byte i = 0; i < 4; i++) {
    Serial.print(ip[i], DEC);
    if(i < 3){Serial.print(".");}
    else {Serial.print("\n");}
  }
  Serial.println();
  server.begin(); // включаем ожидание входящих соединений
  //Serial.println(Ethernet.localIP()); // выводим IP-адрес контроллера

}

/**
 *  Loop procedure
 */
void loop() {
  int8_t u8summ = getDisplay();
  if(u8summ > 1){Serial.println(u8summ);}
  // diagnose communication
  
  client = server.available(); // ожидаем объект клиент
  if (client) {
    // есть данные от клиента
    if (clientAlreadyConnected == false) {
      // сообщение о подключении
      Serial.println("Client connected");
      client.println("Server ready"); // ответ клиенту
      clientAlreadyConnected= true; 
    }

    while(client.available() > 0) {
      char chr = client.read(); // чтение символа
      server.write(chr); // передача клиенту
      Serial.write(chr);
    } 
  }


  
  
}//END loop()_________________________________________________________________END loop()



int getDisplay() {
  if(u8dig_count < 3){
    if(!digitalRead(DIG[u8dig_count])){
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
    bitWrite(buf_gd, (i_gd), !digitalRead(SEGMENTS[i_gd]));
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
    case B1100000: // цифра "1" 1-ого р-да  //96  //0x60
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
    if(!digitalRead(DIG[0]) && digitalRead(DIG[1])){
      u32tonoff = 0;
      Serial.println("Датчик Уже Включен");
    }
  }
  if(0 < u32tonoff){
    pinMode(ON_PIN, OUTPUT);
    pinMode(SEGMENTS[3], OUTPUT);
    digitalWrite(ON_PIN, LOW);
    digitalWrite(SEGMENTS[3], LOW);
    for(int32_t u32ton = (millis() + 1000); millis() < u32ton;){ }
    digitalWrite(ON_PIN, LOW);
    pinMode(ON_PIN, INPUT);
    pinMode(SEGMENTS[3], INPUT_PULLUP);
    for(int32_t u32ton = (millis() + 1000); millis() < u32ton;){ }
    pinMode(SW_2, OUTPUT);
    pinMode(SW_4, OUTPUT);
    digitalWrite(SW_2, LOW );
    digitalWrite(SW_4, LOW );
    for(int32_t u32ton = (millis() + 2000); millis() < u32ton;){ }
    pinMode(SW_2, INPUT_PULLUP);
    pinMode(SW_4, INPUT_PULLUP);
    for(int32_t u32ton = (millis() + 1000); millis() < u32ton;){ }
    pinMode(SW_4, OUTPUT);
    digitalWrite(SW_4, LOW);
    for(int32_t u32ton = (millis() + 1000); millis() < u32ton;){ }
    pinMode(SW_4, INPUT_PULLUP);
    Serial.println("Стартую, Датчик включен и настроен");
  }
  else {
    Serial.println("Стартую, Датчик был включен");
  }
}



  void ioSetup() 
  {
    // Аноды-Сегменты от A до G) дисплея
    pinMode(SEGMENTS[0], INPUT_PULLUP); // SEG_A      
    pinMode(SEGMENTS[1], INPUT_PULLUP); // SEG_B
    pinMode(SEGMENTS[2], INPUT_PULLUP); // SEG_C
    pinMode(SEGMENTS[3], INPUT_PULLUP); // SEG_D
    pinMode(SEGMENTS[4], INPUT_PULLUP); // SEG_E
    pinMode(SEGMENTS[5], INPUT_PULLUP); // SEG_F
    pinMode(SEGMENTS[6], INPUT_PULLUP); // SEG_G
  
    // Разряды-Катоды дисплея
    pinMode(DIG[0], INPUT);   // DIG_1
    pinMode(DIG[1], INPUT);  // DIG_2
    pinMode(DIG[2], INPUT);  // DIG_3
  }



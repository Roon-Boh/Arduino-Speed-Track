// Author © 2018-2019 Sergey Kordubin. Contacts: <root@roon-art.ru>
// License: https://opensource.org/licenses/GPL-3.0
// Source on GitHub: https://github.com/Roon-Boh/Arduino-Speed-Track.git
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
#include <SPI.h>
#include <UIPEthernet.h>
#include <EEPROM.h>

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
boolean uart_status = false;
int8_t u8summbuf = 0; // буфер скорости для передачи удаленному серверу
unsigned long buftime; // время последнего запроса
char post;
int buflife;
boolean reset = false;
// (порт 80 по умолчанию для HTTP):
EthernetServer server(80);

/**
 *  Setup procedure
 */
void setup() {
  reset = false; 
  // Если A3 поддтянут к земле то чищу EEPROM
  if(!digitalRead(A3)) {
    delay(1);
    //unsigned long ittime = millis();
    for (unsigned long i = millis() + 10000; millis() < i; ){
      if(!digitalRead(A3)){
       delay(100);
       if(millis() > i && !digitalRead(A3))
         for(int i = 0; i < 512; i++){
            EEPROM.write(i, 0xFF);
          }
      }
    }
  }
  
  // задаем мак по умолчанию
  byte mac[6]; // = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  // если мак есть в памяти то переопределяем его
  if(EEPROM.read(10) == 127){
      mac[0] = EEPROM.read(11);
      mac[1] = EEPROM.read(12);
      mac[2] = EEPROM.read(13);
      mac[3] = EEPROM.read(14);
      mac[4] = EEPROM.read(15);
      mac[5] = EEPROM.read(16);
  } else {
      mac[0] = 0xDE;
      mac[1] = 0xAD;
      mac[2] = 0xBE;
      mac[3] = 0xEF;
      mac[4] = 0xFE;
      mac[5] = 0xED;
  }

  byte ip_set[4]; // = {192, 168, 0, 77};
  if(EEPROM.read(20) == 127){
      ip_set[0] = EEPROM.read(21);
      ip_set[1] = EEPROM.read(22);
      ip_set[2] = EEPROM.read(23);
      ip_set[3] = EEPROM.read(24);
  } else {
      ip_set[0] = 192;
      ip_set[1] = 168;
      ip_set[2] = 0;
      ip_set[3] = 77;
  }
  if(EEPROM.read(30) == 127){
      buflife = (EEPROM.read(31)*0x100) + EEPROM.read(32);
  } else {
      buflife = 5000;
  }
  IPAddress ip(ip_set[0], ip_set[1], ip_set[2], ip_set[3]);
  
  Serial.begin(9600);
   if(Serial){
    uart_status = true;
  }
  
  //  инициализация портов ввода вывода для считывания сегментов
  ioSetup();
  
  // Управление включением SW_1
  pressStart();
  
  // Запустите соединение Ethernet и сервер:
  Ethernet.begin(mac, ip);

  // start the server
  server.begin();
  Serial.println(Ethernet.localIP());
}


/**
 *  Loop procedure
 */
void loop() {
  if(reset == true) {
    reset = false;
    asm volatile("jmp 0x00");
  }
  // получаем значение скорости
  int8_t u8summ = getDisplay();

  // Готовим большее значение
  if(u8summ > 1){
    
    // если значение старое то сбрасываем его
    if((millis() - buftime) >= 2000){
      u8summbuf = 0;
    }
    
    if(u8summbuf < u8summ){
      u8summbuf = u8summ;
    }
    buftime = millis(); // время обновления буфера
    
    if(u8summbuf > 1) {
      Serial.println(u8summ);
    }
  }
  
  // слушать входящих клиентов
  EthernetClient client = server.available();
  if (client) {
    // HTTP-запрос заканчивается пустой строкой
    boolean currentLineIsBlank = true;
    String buffer = String(""); // буфер от клиента
    String printbuffer = String("");  // буфер для клиента
    String msg = String("");
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // если вы дошли до конца строки (получили символ новой строки) и строка пуста,
        // http-запрос завершен, поэтому вы можете отправить ответ
        if (c == '\n' && currentLineIsBlank) {

          
          while (client.available()) {     //Обработка запроса POST(находится после пустой строки заголовка)
            post = client.read();
            if (buffer.length() <= 120) {
              buffer += post;
            }
          }
          if (buffer.indexOf("cmd=") >= 0) {
            if (buffer.indexOf("%3A") >= 0) {
              buffer.replace("%3A", ":");
            }
            if (buffer.indexOf("%2C") >= 0) {
              buffer.replace("%2C", ",");
            }
            String cmdbuf = String(buffer);
            cmdbuf.replace("cmd=", "");
            if(cmdbuf.indexOf("ip:")>= 0){
              // извлекаем значение
              cmdbuf.replace("ip:", "");
              //byte index = cmdbuf.indexOf(".");
              byte ip_set[4];
              //String prs;
              for(byte i = 0; i < 4 ; i++) {
                String separator = ".";
                if(i == 3) {
                  separator = ",";
                }
                byte index = cmdbuf.indexOf(separator);
                String prs = cmdbuf.substring(0, index);
                ip_set[i] = prs.toInt();
                cmdbuf.replace(prs + separator, "");
              }

              // записываем в EEPROM
              if(ip_set[0] == 192 && ip_set[1] == 168){
                  if(ip_set[2] + ip_set[3] >= 2) {
                    EEPROM.write(20, 127);
                    EEPROM.write(21, ip_set[0]);
                    EEPROM.write(22, ip_set[1]);
                    EEPROM.write(23, ip_set[2]);
                    EEPROM.write(24, ip_set[3]);
                  }
              }
            }
            
            if(cmdbuf.indexOf("mac:")>= 0){
              // извлекаем значение
              cmdbuf.replace("mac:", "");
              //byte index = cmdbuf.indexOf(".");
              byte mac_set[4];
              //String prs;
              for(byte i = 0; i < 4 ; i++) {
                String separator = ".";
                if(i == 3) {
                  separator = ",";
                }
                byte index = cmdbuf.indexOf(separator);
                String prs = cmdbuf.substring(0, index);
                mac_set[i] = prs.toInt();
                cmdbuf.replace(prs + separator, "");
              }

              // записываем в EEPROM
              if(mac_set[0] >= 1 && mac_set[1] >= 2){
                  if(mac_set[2] + mac_set[3] >= 2) {
                    EEPROM.write(10, 127);
                    for(byte i = 0; i <6; i++){
                      EEPROM.write(11 + i, mac_set[i]);
                    }
                  }
              }
            }

             
            if(cmdbuf.indexOf("buflife:")>= 0){
              cmdbuf.replace("buflife:", "");
              for(byte i = 0; i < 1 ; i++) {
                byte index = cmdbuf.indexOf(",");
                String prs = cmdbuf.substring(0, index);
                buflife = prs.toInt();
                cmdbuf.replace(prs + ",", "");
              }
              EEPROM.write(30, 127);
              EEPROM.write(31, highByte(buflife)) ;
              EEPROM.write(32, lowByte(buflife));
            }
            
            if(cmdbuf.indexOf("reset:true")>= 0){
              for(int i = 0; i < 512; i++){
                EEPROM.write(i, 0xFF);
              }
              cmdbuf.replace("reset:true,", "");
            }
            
            if(cmdbuf.indexOf("restart:true")>= 0){
              reset = true;
            }
          }

          
          // Отправить стандартный заголовок HTTP-ответа
          if(buflife < (millis() - buftime)){
            u8summbuf = 0;
          }
          client.println("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<!DOCTYPE HTML><html><h1>" + String(u8summbuf) + "</h1>");
          client.println("<form method='POST'><input type='text' name='cmd'></form></html>");
          Serial.println(buffer);  // Распечатка POST запроса
          break;
        }
        if (c == '\n') {
          // Вы начинаете новую строку
          currentLineIsBlank = true;
        } else if (c != '\r') {
          //вы получили символ в текущей строке
          currentLineIsBlank = false;
        }
      }
    }
    // дать веб-браузеру время для получения данных
    delay(1);
    // закрыть соединение:
    client.stop();
    Serial.println("Disconnected");
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
  }
}



  void ioSetup() 
  {
    // reset eeprom button
    pinMode(A3, INPUT_PULLUP); // reset eeprom button
    
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

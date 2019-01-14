 Author © 2018-2019 Sergey Kordubin. Contacts: <root@roon-art.ru>
 License: https://opensource.org/licenses/GPL-3.0
 Source on GitHub: https://github.com/Roon-Boh/Arduino-Speed-Track.git


 PIN
  0   ON_PIN  (пин на кнопку On/Off)
  0   LED_3 (светодиод состояния считывателя)

Катоды-Сегменты дисплея от A до G
_PIN  PIN     Array[]         
  0    0    SEGMENTS[0]    SEG_A
  0    0    SEGMENTS[1]    SEG_B
  0    0    SEGMENTS[2]    SEG_C    SW_3    (CLD_2) (пин на кнопку "ReCall")
  0    0    SEGMENTS[3]    SEG_D    SW_1    (CLD_1) (пин через резистор на кнопку On/Off)
  0    0    SEGMENTS[4]    SEG_E    SW_4    (пин на кнопку "Km/H")
  0    0    SEGMENTS[5]    SEG_F
  0    0    SEGMENTS[6]    SEG_G    SW_2    (пин на кнопку "MPH")
 
Разряды-Аноды дисплея
_PIN  PIN    Array[]           
  0    0    DIG[0]    DIG_1    ALED    (LED Anode)
  0    0    DIG[1]    DIG_2        
  0    0    DIG[2]    DIG_3

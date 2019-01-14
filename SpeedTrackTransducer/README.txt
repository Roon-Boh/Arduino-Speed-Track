 Author © 2018-2019 Sergey Kordubin. Contacts: <root@roon-art.ru>
 License: https://opensource.org/licenses/GPL-3.0
 Source on GitHub: https://github.com/Roon-Boh/Arduino-Speed-Track.git


_PIN PIN
 12   0   ON_PIN  (On/Off button)
 13   0   LED_3 (Reader status LED)

Cathodes-Segments Sensor A to G
_PIN  PIN     Array[]         
  8    8    SEGMENTS[0]    SEG_A
  7    7    SEGMENTS[1]    SEG_B
  6    6    SEGMENTS[2]    SEG_C    SW_3    (CLD_2) (Pin on the "ReCall" button)
  5    5    SEGMENTS[3]    SEG_D    SW_1    (CLD_1) (Pin via resistor to the ON/Off button)
  4    4    SEGMENTS[4]    SEG_E    SW_4    (Pin on the button "Km/H")
  3    3    SEGMENTS[5]    SEG_F
  2    2    SEGMENTS[6]    SEG_G    SW_2    (Pin on the "MPH" button)
 
Digits-Anodes Sensor
_PIN  PIN    Array[]           
  9    A1    DIG[0]    DIG_1    ALED    (LED Anode)
 10    A2    DIG[1]    DIG_2        
 11    A3    DIG[2]    DIG_3

Module MINI ENC28J60
12	4	SO
11	5	SI
13	6	SCK
10	7	CS
RES	8	RST
3.3 V	9	VCC
GND	10	GND

PIN_UNO     ENC28J60   
  10        CS -> 7   (Chip Select, вход выбора чипа, интерфейс SPI) 
  11        SI -> 5   (вход данных, интерфейс SPI) 
  12        SO -> 4   (выход данных, интерфейс SPI)
  13       SCK -> 6   (вход тактирования, интерфейс SPI) 
 RES       RST -> 8   (вывод сброса) 
+3.3V      VCC -> 9   (вход внешнего питания 3.3 вольта)
 GNG       GND -> 10  (общий вывод)
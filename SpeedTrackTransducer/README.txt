 Author © 2018-2019 Sergey Kordubin. Contacts: <root@roon-art.ru>
 License: https://opensource.org/licenses/GPL-3.0
 Source on GitHub: https://github.com/Roon-Boh/Arduino-Speed-Track.git



PIN_UNO     ENC28J60   
  10        CS -> 7   (Chip Select, вход выбора чипа, интерфейс SPI) 
  11        SI -> 5   (Data input, SPI interface) 
  12        SO -> 4   (Data output, SPI interface)
  13       SCK -> 6   (clocking input, SPI interface) 
 RES       RST -> 8   (reset output) 
+3.3V      VCC -> 9   (input external power 3.3 volts)
 GNG       GND -> 10  (General conclusion)
 
_PIN PIN
 12   9   ON_PIN  (On/Off button)

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
  9    A0    DIG[0]    DIG_1    ALED    (LED Anode)(Hundreds)
 10    A1    DIG[1]    DIG_2            (Tens)
 11    A2    DIG[2]    DIG_3            (Units)
 
 
 Разьем на ENC28J60
            --------- 
   2  1*  >|[ 1] [ 2]|
	 4  3    |[ 3] [ 4]|
	 6  5    |[ 5] [ 6]
	 8  7    |[ 7] [ 8]|
	10  9    |[ 9] [10]|
            ---------
  1  ->  2
  2  ->  1
  3  ->  4
	4  ->  3
	5  ->  6
	6  ->  5
	7  ->  8
	8  ->  7
	9  -> 10
	10 ->  9
 
 Разьем на к Transducer
                 ------
OLD_D11 UNO_A0 >|[ 1][ 2]| GND
OLD_D10 UNO_A1  |[ 3][ 4]| GND
OLD_D9  UNO_A2  |[ 5][ 6]| VCC
OLD_D8  UNO_D8  |[ 7][ 8]| GND
OLD_D7  UNO_D7  |[ 9][10]  GND
OLD_D6  UNO_D6  |[11][12]  GND
OLD_D5  UNO_D5  |[13][14]| GND
OLD_D4  UNO_D4  |[15][16]| VCC
OLD_D3  UNO_D3  |[17][18]| GND
OLD_D2  UNO_D2  |[19][20]| UNO_D9 OLD_D12 
                 ------





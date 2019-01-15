 Author © 2018-2019 Sergey Kordubin. Contacts: <root@roon-art.ru>
 License: https://opensource.org/licenses/GPL-3.0
 Source on GitHub: https://github.com/Roon-Boh/Arduino-Speed-Track.git




 UNO   ENC28J60   Title
 NA    [1] CLK  (Programmable output, clock output)
 NA     2  NT   (Interrupt output)
 NA     3  WOL  (Wake-up on LAN, Interrupt output)
 D12    4  SO   (Data output, SPI interface)
 D11    5  SI   (Data input, SPI interface)
 D13    6  SCK  (Clocking input, SPI interface)
 D10    7  CS   (Chip select input, SPI interface)
 RES    8  RST  (Reset output) 
 +3.3   9  VCC  (Input external power 3.3 volts)
 GNG    10 GND  (General conclusion)

_PIN PIN
 12   9   ON_PIN  (On/Off button)

Cathodes-Segments Sensor A to G
_PIN  PIN     Array[]         
  8    8    SEGMENTS[0]    SEG_A
  7    7    SEGMENTS[1]    SEG_B
  6    6    SEGMENTS[2]    SEG_C, SW_3  (Catod LD_2) (Pin on the "ReCall" button)
  5    5    SEGMENTS[3]    SEG_D, SW_1  (Catod LD_1) (Pin via resistor to the ON/Off button)
  4    4    SEGMENTS[4]    SEG_E, SW_4  (Pin on the button "Km/H")
  3    3    SEGMENTS[5]    SEG_F
  2    2    SEGMENTS[6]    SEG_G, SW_2  (Pin on the "MPH" button)
 
Digits-Anodes Sensor
_PIN  PIN    Array[]           
  9    A0    DIG[0]    DIG_1 (Hundreds), (LED Anode)
 10    A1    DIG[1]    DIG_2 (Tens)
 11    A2    DIG[2]    DIG_3 (Units)
 
 

 
                Transducer
                 ------
OLD_D11 -> DIG_1 -> A0 >|[ 1][ 2]| GND
OLD_D10 -> DIG_2 -> A1  |[ 3][ 4]| GND
OLD_D9  -> DIG_3 -> A2  |[ 5][ 6]| VCC
OLD_D8  -> SEG_A -> D8  |[ 7][ 8]| GND
OLD_D7  -> SEG_B -> D7  |[ 9][10]  GND
OLD_D6  -> SEG_C -> D6  |[11][12]  GND
OLD_D5  -> SEG_D -> D5  |[13][14]| GND
OLD_D4  -> SEG_E -> D4  |[15][16]| VCC
OLD_D3  -> SEG_F -> D3  |[17][18]| GND
OLD_D2  -> SEG_G -> D2  |[19][20]| UNO_D9 <- OLD_D12 <- ON_PIN  (On/Off button)
                 ------





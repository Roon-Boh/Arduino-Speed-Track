/**
    Modbus slave example 2:
    The purpose of this example is to link the Arduino digital and analog
    pins to an external device.

    Recommended Modbus Master: QModbus
    http://qmodbus.sourceforge.net/
*/

#include <SPI.h>
#include <DMD2.h>
#include <fonts/BigMonoFont32x48.h>
#define PINBRIGHT A6 // переключатель HBRIGHT\LBRIGHT
#define LBRIGHT 10 // подсветка минимальная
#define HBRIGHT 200 // подсветка максимал
SoftDMD dmd(2, 3); // DMD controls the entire display
// SoftDMD::SoftDMD(byte panelsWide, byte panelsHigh, byte pin_noe, byte pin_a, byte pin_b, byte pin_sck, byte pin_clk, byte pin_r_data)
unsigned long display_time;
uint8_t display_boof = 0;
uint8_t display_summ = 0;
////////

/**
    Setup procedure
*/
void setup() {
  dmd.setBrightness(LBRIGHT);
  dmd.selectFont(BigMonoFont32x48);
  dmd.begin();
  Serial.begin(19200);

  pinMode(13, OUTPUT);
  pinMode(PINBRIGHT, INPUT);
  if (analogRead(PINBRIGHT)> 10) {
    dmd.setBrightness(HBRIGHT);
  }
}
void loop() {
  if (Serial.available() >= 3 && Serial.read() == ':') { // если пришли даные
    
    char ichar1= Serial.read();
    char ichar2= Serial.read();
    uint8_t summsr;
    if(isHexadecimalDigit(ichar1) && isHexadecimalDigit(ichar2)){
      summsr = digchar(ichar2)+(digchar(ichar1)*0x10);
    }
    else {summsr = 0;}
      oledWrite(summsr);
  }
}




unsigned char digchar(unsigned char v)
{ v -= '0';
  if (v > 41) return v - 39; /* a .. f */
  if (v > 9) return v - 7; /* A .. F */
  return v; /* 0 .. 9 */
}


/**
  управляет дисплеем из P10 модулей,
  при отсутствии данных, очищает дисплей,
  оставляет дисплей неизменным.
*/
int oledWrite (uint8_t ol_u8summ)
{
  if (0 < ol_u8summ) { // если не ноль
    int8_t n = -1; // задаем начальное смещение влево на пиксель
    if (ol_u8summ < 10) n = 15; // если один знак то центрируем
    else if (99 < ol_u8summ) n = -15; // если три знака то центрируем по 2-му
    if (display_boof != ol_u8summ) { // сравниваем с буфером
      if (10 < display_boof && ol_u8summ < 10) dmd.clearScreen();
      display_boof = ol_u8summ;
      display_time = millis() + 10000;
      dmd.drawString(n, -1, String(ol_u8summ));
    }
    if (millis() > display_time) {    // давно не приходят данные
      ol_u8summ = 0;                  // то очищаем дисплей
      display_boof = 0;
      dmd.clearScreen();
      display_time = millis() + 60000;
      /*для отладки*/
      dmd.drawString(15, -1, String(ol_u8summ));
    }
  }
}

#include <SPI.h>
#include <UIPEthernet.h>
#include <DMD2.h>
#include <fonts/BigMonoFont32x48.h>
#define DISPLAYS_WIDE 2
#define DISPLAYS_HIGH 3
#define PIN_NOE    A0  // 1
#define PIN_A      A1  // 2
#define PIN_B      A2  // 4
#define PIN_CLK    A3  // 8  default to h/w SPI SCK D13
#define PIN_SCK    A4  // 10
#define PIN_R_DATA A5  // 12 default to h/w SPI MOSI D11
// SoftDMD dmd(DISPLAYS_WIDE,DISPLAYS_HIGH, 9, 6, 7, 8, 11, 13);
SoftDMD dmd(DISPLAYS_WIDE, DISPLAYS_HIGH, PIN_NOE,  PIN_A, PIN_B, PIN_SCK, PIN_CLK, PIN_R_DATA);
EthernetServer server(80);
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 0, 77);

// the setup routine runs once when you press reset:
void setup() {
    Ethernet.begin(mac, ip);
    server.begin();
    Serial.println(Ethernet.localIP());
    
    dmd.setBrightness(150);
    dmd.selectFont(BigMonoFont32x48);
    dmd.begin();
}

int phase = 0; // 0-3, 'phase' value determines direction

// the loop routine runs over and over again forever:
void loop() {
    dmd.drawString(-20, 0, "123");
    //dmd.clearScreen();
    
    EthernetClient client = server.available();
    if (client) {
    // HTTP-запрос заканчивается пустой строкой
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // если вы дошли до конца строки (получили символ новой строки) и строка пуста,
        // http-запрос завершен, поэтому вы можете отправить ответ
        if (c == '\n' && currentLineIsBlank) {
          // Отправить стандартный заголовок HTTP-ответа
          client.println("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<!DOCTYPE HTML><html><h1>HELLO I`M P10!</h1><pre>");
          client.println("</pre></html>");
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

}

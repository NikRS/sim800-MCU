#include <SPI.h>               // include SPI library
#include <Adafruit_GFX.h>      // include adafruit graphics library
#include <Adafruit_PCD8544.h>  // include adafruit PCD8544 (Nokia 5110) library
 

// Adafruit_PCD8544(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST);
// Adafruit_PCD8544(int8_t SCLK, int8_t DIN, int8_t DC, int8_t RST);
// Adafruit_PCD8544(int8_t DC, int8_t CS, int8_t RST);
//                                           SLK DIN  DC
  Adafruit_PCD8544 display = Adafruit_PCD8544(D6, D7, D8, 25);
  boolean VisibilityOfDelimeter = true;

void i_Nokia5110Display(){
  display.begin();                                         // Инициализирует модуль
  display.cp437(true);                                     // Выбор кодировки символов
  display.setContrast(45);
  display.setTextColor(BLACK);
  display.setTextSize(3);                                  // Установит размер текста
  display.clearDisplay();                                  // Очитит экран и буфер
  display.display();                                       // Покажет содержимое буфера
}


void displayTime(){
  display.setTextSize(3);

    display.setCursor(0, 0);                               // Часы
    display.print(formatDigit(db.RTChour, 2));

    display.setCursor(48, 0);                              // Минуты
    display.println(formatDigit(db.RTCminute, 2));

    if(VisibilityOfDelimeter){                             // Разделитель
      display.setCursor(32, 0);
      display.print(':');
      VisibilityOfDelimeter = false;
    }else{
      VisibilityOfDelimeter = true;
    }

    display.display();
}


void displayDate(){
    display.setTextSize(2);
    display.setCursor(54, 23);
    display.println(formatDigit(db.RTCdayOfMonth, 2));     // День
    display.display();
}


void displayDHT(){
  if (isnan(db.climate[0]) || isnan(db.climate[1])) {      // Проверяем, нет ли ошибки измерения
    Serial.println("Failed to read from variables climate data - NaN");
  }else{
    display.setTextSize(2);
    display.setCursor(4, 23);
    display.print(db.climate[0], 0);                       // Темпреатура
    display.drawBitmap(28, 25, imgTemperatureSymbol, 8, 4, BLACK);
    display.display();

    display.setTextSize(1);
    display.setCursor(4, 39);
    display.print(db.climate[1], 1);                       // Влажность
    display.print('%');
    display.display();
  }
}


// Служебные функции

void clearDisplay(){
  display.clearDisplay();
}

String utf8rus(String source){
  int i, k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
            n = source[i]; i++;
            if (n == 0x81) {
              n = 0xA8;
              break;
            }
            if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
            break;
          }
        case 0xD1: {
            n = source[i]; i++;
            if (n == 0x91) {
              n = 0xB8;
              break;
            }
            if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
            break;
          }
      }
    }
    m[0] = n; target = target + String(m);
  }
  return target;
}

void displayScreen(){                                      // Тут описана логика отображения, что за чем отображать
  clearDisplay();
  displayTime();
  displayDate();
  displayDHT();

                                                           // 4px зазор между иконками статуса
  if (db.simBatteryVoltage < 3500) {                       // Напряжение на модуле СИМ800 ниже 3.5 В
    display.drawBitmap(40, 40, imgBatteryUndervoltage, 8, 7, BLACK); display.display();
  }  
                                                           // Статус сообщений
  if (db.simEvent == 1) {                                  // Есть новое событие не отправленное на сервер
    display.drawBitmap(52, 40, imgRecieveSMS, 8, 7, BLACK); display.display();
  }else if(db.simEvent == 2){                              // Все события сохранены на сервере
    display.drawBitmap(52, 40, imgEventSaved, 8, 7, BLACK); display.display();
  }

                                                           // Статус Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {                     // Если подключен, то отображает иконку
    display.drawBitmap(64, 40, imgWiFiGood, 8, 7, BLACK); display.display();
  }

                                                           // Статус GSM
  if (db.simAuth == 1) {                                   // Авторизован в домашней сети
                                                           // Отобразим уровень сигнала
    if (db.simSignalStrength > 10) {
      display.drawBitmap(76, 40, imgGSMhigh, 8, 7, BLACK);display.display();
    }else if (db.simSignalStrength > 0) {
      display.drawBitmap(76, 40, imgGSMmedium, 8, 7, BLACK);display.display();
    }else{
      display.drawBitmap(76, 40, imgGSMnoSignal, 8, 7, BLACK);display.display();
    }
  }
  else if (db.simAuth == 2) {                               // В поиске оператора
    display.drawBitmap(76, 40, imgGSMattemptAuthorize, 8, 7, BLACK);display.display();
  }
  else if (db.simAuth == 5) {                               // Роуминг
    display.setTextSize(1);
    display.setCursor(76, 40);
    display.print("R");
    display.display();
  }
  else{                                                     // Не авторизован
    display.drawBitmap(76, 40, imgGSMauthorizationProhibited, 8, 7, BLACK);display.display();
  }

}

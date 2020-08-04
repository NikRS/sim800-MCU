// Глобальная переменная

struct data {                         // создаём ярлык data
  String WiFi_SSID;                   // Название Wi-Fi сети SSID
  String WiFi_PASS;                   // Пароль Wi-Fi сети

  String URL_host;                    // Адрес сервера для обращений
  String URL_get;                     // URI для получения команд
  String URL_set;                     // URI для отправки данных с модуля на сервер
  String WC_directive;                // WebClient команда полученная с сервера
  String WC_argument;                 // WebClient аргументы к команде полученной с сервера
  String WC_wetRequests;              // Переменная для хранения запросов, не отправленноых на сервер
  uint8_t WC_emptyCalls;              // Количество "пустых" обращений
  uint32_t WC_timeInterval;           // Интервал обращений к серверу 60 сек .. 300 сек
  uint32_t WC_timer;                  // Значение таймера обращений к серверу

  byte RTCsecond, RTCminute, RTChour, RTCdayOfWeek, RTCdayOfMonth, RTCmonth, RTCyear;   // Временная метка часов реального времени

  uint8_t simEvent;                   // Наличие события для обработки 0 - нет событий, 1 - есть НЕ ОБРАБОТАННЫЕ, 2 - ОБРАБОТАНЫ ВСЕ события
  uint32_t SIM_timer;                 // Значение таймера обращений
  uint16_t simBatteryVoltage;         // Напряжение на аккумуляторе
  uint8_t simSignalStrength;          // Уровень сигнала 0..99
  uint8_t simAuth;                    // Авторизация в сети 
                                      // 0 — незарегистрирован, не ищет нового оператора для регистрации, 
                                      // 1 — зарегистрирован в домашней сети, 
                                      // 2 — незарегистрирован, но в поиске нового оператора для регистрации, 
                                      // 3 — регистрация запрещена, 
                                      // 4 — неизвестно, 
                                      // 5 — зарегистрирован, в роуминге

  float climate[1];                   // Температура[0] и влажность[1]
} db;                                 // и сразу создаём структуру db


// Прототипы функций, используемх до объявления

byte wcSetCmd(String SetCmd);
String wcGETformat(String _str);
void wcTimeOut(bool status);


// Подключаем модули

#include "0z_ESP8266_pinout.h"
#include "1z_DHT_temp.h"
#include "2z_DS3231_clock.h"
#include "3z_SIM_800.h"
#include "4z_Wi-Fi.h"
#include "5z_WEB.h"
#include "6z_SerialCommunication.h"
#include "7z_Images.h"
#include "8z_Nokia_5110_display.h"

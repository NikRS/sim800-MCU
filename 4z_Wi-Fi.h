// Работа с WiFi
// Код написан для работы по HTTPS
// Подключение библиотек
#define USING_AXTLS
#include <ESP8266WiFi.h>


// force use of AxTLS (BearSSL is now default)
#include <WiFiClientSecureAxTLS.h>
using namespace axTLS;


// Подключение к Wi-Fi сети
byte i_WiFi(String SSID, String Password){
    Serial.println("Connecting to " + String(SSID) + '[' + String(Password) + ']');           // Информация для отладки о названи Wi-Fi сети и пароле
    WiFi.mode(WIFI_STA);                                                                      // Определение режима работы Wi-Fi
    WiFi.begin(SSID, Password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\nWiFi connected\nIP address: ");                                           // Информация о выдном IP адресе
    Serial.println(WiFi.localIP());
}


// возможность подключения к другой сети
void changeWiFiAuthorizationData(){
    Serial.println("Введите новый SSID для Wi-Fi");
    while (Serial.available() <= 0);                            // Обертка для получения строки от пользователя
        if (Serial.available() > 0) {
            db.WiFi_SSID = Serial.readStringUntil('\n');        // Отправка команды sim
        }
    Serial.println("Введите новый пароль для Wi-Fi");
    while (Serial.available() <= 0);                            // Обертка для получения строки от пользователя
        if (Serial.available() > 0) {
            db.WiFi_PASS = Serial.readStringUntil('\n');        // Отправка команды sim
        }
    Serial.println("SSID: " + String(db.WiFi_SSID) + ", пароль: " + String(db.WiFi_PASS));
}

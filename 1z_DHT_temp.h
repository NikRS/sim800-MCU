// Работа с датчиком температуры и влажности
// Подключаем библиотеку DHT для ESP

#include "DHTesp.h"

DHTesp dht;                                                         // Создание экземпляра класса


void i_DHT(int pin){                                                // Инициализация
  dht.setup(pin, DHTesp::DHT22);                                    // Выбор пина и типа 
}


void getDHT(){                                                      // Опрос датчика
  if (isnan(dht.getHumidity()) || isnan(dht.getTemperature())) {    // Проверяем, нет ли ошибки измерения
      Serial.println("Failed to read from DHT sensor!");            // При отсутствии показадний с датчика вывести информацию о ошибке
   }else{
      db.climate[0] = dht.getTemperature();                         // Обновление значения температуры в структуре
      db.climate[1] = dht.getHumidity();                            // Обновление значения влажности в структуре
  }
}

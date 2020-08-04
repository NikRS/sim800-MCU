// Работа с веб службой
// Обработка запросов и ответов от сервера



uint8_t wcResponseAnalyzer(String type, bool debug, WiFiClientSecure* _client){           // debug - Выведет ответ в Serial если TRUE, type - отсылаем или запрашиваем данные
    String _responseLine;


    while (_client->connected()) {                                                        // Читаем заготовки до начала тела ответа
        _responseLine = _client->readStringUntil('\n');

        if (debug == true) { Serial.println(_responseLine); }

        if (_responseLine == "HTTP/1.1 404 Not Found\r") { return 2; }                  // Ошибка сервера
        if (_responseLine == "\r") { _client->readStringUntil('\n'); break; }
    }

    if (type == "GET"){
        db.WC_directive = _client->readStringUntil('\n');                               // Читаем первую строку. Тут хранится информация о команде
        db.WC_argument = _client->readStringUntil('\n');                                // Читаем вторую строку. Тут хранится аргумент к команде
        _responseLine = _client->readStringUntil('\n');
        Serial.println(db.WC_directive + " - directive");
        Serial.println(db.WC_argument + " - srgument");
        Serial.println(_responseLine + " - line");
        if(_responseLine.length() > 2 and _responseLine.length() < 14){
            _client->stop();
            sendSMSinPDU(db.WC_argument, _responseLine);
            wcSetCmd("&simSUCCESS&q=" + db.WC_directive + wcGETformat("\\") + "n" + db.WC_argument);
            db.WC_directive = "";
            db.WC_argument = "";
        }else{
                if (db.WC_directive == "AT") {             // AT команды только для SIM модуля
                    String _ATresponse;
                    wcTimeOut(true);                    // Переход в активный режим опроса сервера
                    String querry = String(db.WC_directive) + String(db.WC_argument);
                    Serial.println("\nПринята команда с сервера: " + querry);

                    _ATresponse = sendATCommand(querry, true);

                    if (_ATresponse) {                  // Если команда выполнена
                        Serial.println("Успешно выполнена\n" + String(_ATresponse));
                        querry = "&simSUCCESS&q=" + wcGETformat(db.WC_directive) + wcGETformat("\\") + "n" + wcGETformat(db.WC_argument) + "%&a=" + wcGETformat(_ATresponse);
                        wcSetCmd(querry);               // Отправить на сервер информацию о выполненой команде
                        db.WC_directive = "";
                        db.WC_argument = "";
                    }
                    else{                               // При неуспешном выполнении
                        Serial.println("Ошибка при выполнении\n" + String(_ATresponse));
                        querry = "&simERROR&q=" + wcGETformat(querry) + "&a=" + wcGETformat(_ATresponse);
                        wcSetCmd(querry);               // Отправить на сервер информацию о невыполненой команде
                    }
                }
                else if (db.WC_directive == "STATUS") {   // Отправит на сервер диагстическую информацию (состояние заряда, GSM, Wi-Fi, время, температуру и влажность)
                    wcTimeOut(true);                    // Переход в активный режим опроса сервера
                    Serial.println("Отправка информации на сервер");
                    String querry = "&STATUS&bat=" + String(db.simBatteryVoltage) +                  // Уровень заряда батареи
                                    "&al=" + wcGETformat(String(db.simAuth)) +                               // Тип авторизации в сети
                                    "&ss=" + wcGETformat(String(db.simSignalStrength)) +                     // Уровень сигнала
                                    "&time=" + wcGETformat(getTimeStr()) +                           // Текущее время
                                    "&t=" + wcGETformat(String(db.climate[0])) +                             // Температура
                                    "&h=" + wcGETformat(String(db.climate[1]));                              // Влажность
                    wcSetCmd(querry);                   // Отправляет на сервер информацию
                }
        }
        _client->stop();
        return 0;
    }
    else if(type == "SET"){
      while (_client->connected()) {                                                        // Читаем заготовки до начала тела ответа
        _responseLine = _client->readStringUntil('\n');
        if (debug == true) { Serial.println(_responseLine); }
        if (_responseLine == "\r") { _client->readStringUntil('\n'); break; }
      }
      return 0;
      /*
      _responseLine = _client->readStringUntil('\n');
        if(_responseLine != "ok\r"){ Serial.println(" - Сервер не принял команду");Serial.println(_responseLine);Serial.println(_client->readStringUntil('\n'));Serial.println(_client->readStringUntil('\n')); _client->stop(); return 2; }
        else{ return 0; }*/
    }
}


byte wcGetCmd(){ // Запрашивает команду с сервера. 0 - ЕСТЬ команды, 1 - НЕТ кманд, 2 - ОШИБКА СЕРВЕРА 

    WiFiClientSecure client;                                                            // Открываем новое соединение

    if (!client.connect(db.URL_host, 443)) {                                            // host, port
        Serial.println("\nConnection to " + String(db.URL_host) + "failed");                        
        return 1;
    }


    client.print(String("GET ") + String(db.URL_get) + " HTTP/1.1\r\n" +                // Формируем заголовки и сразу их отправляем
               "Host: " + String(db.URL_host) + "\r\n" +
               "User-Agent: Inception 1.4 (iPod touch; iPhone OS 4.3.3; en_US)\r\n" +
               "Connection: close\r\n\r\n");


    return wcResponseAnalyzer("GET", true, &client);
}


byte wcSetCmd(String SetCmd){ // Отправляет команду на сервер, формируя GET строку. 0 - УСПЕШНО отправлена, 1 - НЕ УДАЛОСЬ подключиться, 2 - ОШИБКА сервера

    WiFiClientSecure client;                                                            // Открываем новое соединение

    if (!client.connect(db.URL_host, 443)) {                                            // host, port
        Serial.println("\nConnection to " + String(db.URL_host) + "failed");                        
        return 1;
    }

Serial.println(String("GET ") + String(db.URL_set) + String(SetCmd) + " HTTP/1.1\r\n" +                // Формируем заголовки и сразу их отправляем
               "Host: " + String(db.URL_host) + "\r\n" +
               "User-Agent: Inception 1.4 (iPod touch; iPhone OS 4.3.3; en_US)\r\n" +
               "Connection: close\r\n\r\n");
    client.print(String("GET ") + String(db.URL_set) + String(SetCmd) + " HTTP/1.1\r\n" +                // Формируем заголовки и сразу их отправляем
               "Host: " + String(db.URL_host) + "\r\n" +
               "User-Agent: Inception 1.4 (iPod touch; iPhone OS 4.3.3; en_US)\r\n" +
               "Connection: close\r\n\r\n");


    return wcResponseAnalyzer("SET", true, &client);
}


void wcTimeOut(bool status){                                // Изменяет периодичность опроса сервера
    if (status) {
//        Serial.println("Пустые запросы = 0");
        db.WC_emptyCalls = 0;                               // Обнуляем счётчик пустых запросов
//        Serial.println("Интервал = 60 сек");
        db.WC_timeInterval = 60000;                         // Меняем интервал между опросами на 1 запрос в минуту
//        Serial.println("Запускаем таймер");
        db.WC_timer = millis() + db.WC_timeInterval;        // Запускаем таймер на следующий промежуток
    }
    else{
//        Serial.println("Увеличим кол-во пустых обращений - отсутствуют поддерживаемые команды");
        ++db.WC_emptyCalls;                                 // Увеличим счётчик пустых обращений
        db.WC_timer = millis() + db.WC_timeInterval;        // Запускаем таймер на следующий промежуток
        if (db.WC_emptyCalls >= 5) {                        // Прошло несколько запросов на сервер, а задач не появилось
            db.WC_emptyCalls = 5;
//          Serial.println("Изменим интервал на 5 мин");
            db.WC_timeInterval = 300000;                    // Меняем интервал между опросами на 5 мин между запросами
        }
    }

}


String wcGETformat(String _str){                            // Заменяет спецсимвол "&" на escape последовательность "%26"
//    $entities =       array('%21', '%2A', '%27', '%28', '%29', '%3B', '%3A', '%40', '%26', '%3D', '%2B', '%24', '%2C', '%2F', '%3F', '%25', '%23', '%5B', '%5D');
//    $replacements =   array('!',   '*',   "'",   "(",   ")",   ";",   ":",   "@",   "&",   "=",   "+",   "$",   ",",   "/",   "?",   "%",   "#",   "[",   "]");
    _str.replace("&", "%26");
    _str.replace("=", "%3D");
    _str.replace("+", "%2B");
    _str.replace("\r", "%0D");
    _str.replace("\n", "%0A");
    _str.replace("\t", "%09");
    _str.replace("\\", "%5C");
    _str.replace("/", "%2F");
    _str.replace(" ", "%20");
    return _str;
}


void wcScanTasks(bool force){                       // Загрузка заданий с сервера по таймеру или принудительно
//  Serial.println("Запрос задач с сервера");
    if (WiFi.status() == WL_CONNECTED) {
     
        if (millis() > db.WC_timer || force) {          // Как только значение таймера больше отсчитанного времени
            byte requestStatus = wcGetCmd();            // Сделаем запрос к серверу

            if (requestStatus == 0) {                   // Успешный ответ от сервера
                Serial.println("Accept data");
            }
            else if (requestStatus == 1) {
                                                                        // Возникла ошибка при обращении к серверу
                                                                        // Ничего не предпринимаем
                                                                        // Таймер и так сработает, обращение к серверу повторится на следущей итерации цикла
            }
            else if (requestStatus == 2) {              // Пока выполнять нечего
                wcTimeOut(false);
            }
        }
    }
}


void wcWetRequests() {
    if (db.WC_wetRequests.length() > 10) {
    // При наличии строк в переменной
    Serial.println("Попытка отправить не отправленные запросы");
    Serial.println(db.WC_wetRequests);
    Serial.println(db.WC_wetRequests.length());
    Serial.println("______");

        // Возьмём первую строку и выполним запрос к серверу
        if (wcSetCmd(db.WC_wetRequests.substring(0, db.WC_wetRequests.indexOf("\n")))) {
            // При успешном выполнении запроса
                // Удаляем строку из переменной
            db.WC_wetRequests = db.WC_wetRequests.substring(db.WC_wetRequests.indexOf("\n")+1);
                // Вызываем функцию рекурсивно
            wcWetRequests();
        }
        else {
            // При неуспешном выполнении запроса
            // Меняем статус событий на "Есть необработанные события"
            db.simEvent = 1;                            // Добавим необработаеное событие
        }
    }
}

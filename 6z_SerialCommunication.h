// Обработка команд из последовательного порта


void testMessage(bool _refresh){
    // Обновим данные о состоянии или берём сохранённые данные и выводим
    if (_refresh) {
        refreshSIMstatus(true);
    }
    Serial.println("Временная метка:          " + getTimeStr());
    
    Serial.println("Температура и влажность:  " + String(db.climate[0]) + "C; " + String(db.climate[1]) + "%");
    
    Serial.println("Авторизация в GSM сети:   " + String(db.simAuth));
    Serial.println("Уровень сигнала GSM сети: " + String(db.simSignalStrength));
    Serial.println("Напряжение на модуле:     " + String(db.simBatteryVoltage));
    
    Serial.print("Данные Wi-Fi сети:        ");
    Serial.print("SSID: ");
    Serial.print(db.WiFi_SSID);
    Serial.print("; PASS: ");
    Serial.println(db.WiFi_PASS);

    Serial.print("Состояние Wi-Fi сети:     ");
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Connected; IP: ");
        Serial.println(WiFi.localIP());
    }else{
        Serial.println("Disconnected");
    }
}

void helpMessage(){
    Serial.println("get – для опроса модулей");
    Serial.println("   serv – проверить доступность команд с сервера");
    Serial.println("   servTask – выполнитьзадачу с сервера");
    Serial.println("   temp – узнать температуру и влажность");
    Serial.println("   time – прочитать временную метку");
    Serial.println("   SMS – прочитать и отправить на сервер первое сообщение");
    Serial.println("set – для управления параметрами");
    Serial.println("   serv – отправить запрос на сервер");
    Serial.println("   simAT – управление сим 800 модулем (необходимо использовать АТ команды)");
    Serial.println("   time – установить дату и время");
    Serial.println("   wifi – редактирование данных о Wi-Fi сети");
    Serial.println("test – Вывод основной информации о модулях, есть аргумент '-r' для обновления данных перед выводом");
}


void SerialListen(){
    if (Serial.available() > 0) {
        String serialComand = Serial.readStringUntil('\n');                                             // Слушаем монитор порта


        if (serialComand == "get") {                                                                    // Ищем нужную команду
            Serial.print("\r\nget>");                       // Отобразим введённую команду
            while (Serial.available() <= 0);                // Ожидание ввода пользователя
            if (Serial.available() > 0) {                   // Появились данные в буфере
                serialComand = Serial.readStringUntil('\n');  // Читаем буфер
                
                if (serialComand == "time") {
                    Serial.println("time");
                    getTime();                              // Обновление временной метки
                    Serial.println(getTimeStr());
                    Serial.println("..");
                }
                else if (serialComand == "temp") {
                    Serial.println("temp");
                    getDHT();                               // Обновим показания температуры
                    Serial.println("Температура: " + String(db.climate[0]) + "\r\nВлажность: " + String(db.climate[1]) + "\r\n");
                    Serial.println("..");
                }
                else if (serialComand == "serv") {
                    Serial.println("serv");
                    wcGetCmd();                             // Проверим доступность команд с сервера
                    Serial.println("..");
                }
                else if (serialComand == "servTask") {
                    Serial.println("servTask");
                    wcScanTasks(true);                      // Получим задание с сервера и обработаем его
                    Serial.println("..");
                }
                else if (serialComand == "SMS") {          // Читаем первое сохранённое сообщение и отправляем его на сервер
                    Serial.println("SMS");
                    String _SIMresponse;
                    _SIMresponse = sendATCommand("AT+CMGR=1,1;", true);
                    if(_SIMresponse.startsWith("+CMGR:")) {
                        String _SMSheader, _SMSbody, _request, _ATcmd;
                        uint8_t _SMSnumber;


                        _SMSheader = _SIMresponse.substring(0, _SIMresponse.indexOf("\r"));                               // Первая строка
                        _SMSbody = _SIMresponse.substring(_SMSheader.length()+2, _SMSbody.lastIndexOf("\r\n0"));     // Вторая строка
                        _SMSbody = UCS2ToString(_SMSbody);


                        _request =  "SIM&localTime=" + wcGETformat(getTimeStr()) +   // Сформируем запрос на сервер
                                    "&t=iSMS" +                                      // Type
                                    "&h=" + wcGETformat(_SMSheader) +                // Headers
                                    "&b=" + wcGETformat(_SMSbody);                   // Body
                                                                                     // При успешной отправке удалить сообщение
                        if(wcSetCmd(_request) != 0){                                 // При НЕ успешной отправке сохранить запрос для повторной отправки
                          db.WC_wetRequests += _request + String("\n");
                        }
                        else{
                          db.simEvent = 2;                                           // Отобразим успешную отправку данных
                          _ATcmd = "AT+CMGD=" + String(_SMSnumber) + ",4";           // АТ команда удаления конкретного сообщения
                          sendATCommand(_ATcmd, true);
                        }
                    }
                    Serial.println("..");
                }
                else {
                    Serial.print("-x\r\n..");
                }
            }
        }
        if (serialComand == "set") {                                                                    // Установка / управление параметрами
            Serial.print("\r\nset>");                       // Отобразим введённую команду
            while (Serial.available() <= 0);                // Ожидание ввода пользователя
            if (Serial.available() > 0) {                   // Появились данные в буфере
                serialComand = Serial.readStringUntil('\n');  // Читаем буфер

                if (serialComand == "serv") {               // Отправим запрос на сервер
                    Serial.println("serv");
                    while (Serial.available() <= 0);        // Обертка для получения строки от пользователя
                        if (Serial.available() > 0) {
                            String _query = Serial.readStringUntil('\n');     // Отправка запроса на сервер
                            if (wcSetCmd(_query) > 0) {
                                Serial.println("Не удалось отправить данные");
                            }
                        }
                    Serial.println("..");
                }
                else if (serialComand == "simAT") {         // Ввести АТ команды
                    Serial.println("simAT");
                    while (Serial.available() <= 0);        // Обертка для получения строки от пользователя
                        if (Serial.available() > 0) {
                            String _query = Serial.readStringUntil('\n');     // Отправка команды sim
                            Serial.println(sendATCommand(_query, true));
                        }
                    Serial.println("..");
                }
                // else if (serialComand == "simPIN") {        // Ввести PIN код и отключить его проверку
                //     Serial.println("simPIN");
                //     while (Serial.available() <= 0);        // Обертка для получения строки от пользователя
                //         if (Serial.available() > 0) {
                //             String _query = Serial.readStringUntil('\n');     // Отправка команды sim

                //             Serial.println(sendATCommand(_query, true));
                //         }
                //     Serial.println("..");
                // }
                else if (serialComand == "time") {          // Изменить время
                    Serial.println("time");
                    setTime();
                    Serial.println("..");
                }
                else if (serialComand == "wifi") {          // Изменить SSID и пароль Wi-Fi
                    Serial.println("wifi");
                    changeWiFiAuthorizationData();
                    Serial.println("..");
                }
                else{
                    Serial.print("-x\r\n..");
                }
            }
        }
        if (serialComand == "test") {                                                                   // Вывод основной информации
            testMessage(false);
            Serial.println("..");
        }
        if (serialComand == "test-r") {                                                                 // Вывод основной информации
            testMessage(true);
            Serial.println("..");
        }
        if (serialComand == "help" || serialComand == "?") {                                            // Спарвка о доступных командах
            helpMessage();
            Serial.println("..");
        }
    }
}

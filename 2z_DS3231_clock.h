#include <Wire.h>

 
void i_RTClock(uint8_t SDA, uint8_t SCL) {
  Wire.begin(SDA, SCL);
}


// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ( (val / 10 * 16) + (val % 10) );
}


// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val / 16 * 10) + (val % 16) );
}


String formatDigit(int i, int len) {
  String s = String(i);
  while (s.length() < len) {
    s = "0" + s;
  }
  return (s);
}


void getTime() {                           // Чтение актуальной временной метки
  Wire.beginTransmission(0x68);
  Wire.write(byte(0x00));
  Wire.endTransmission();
  Wire.requestFrom(byte(0x68), 7);
  db.RTCsecond     = bcdToDec(Wire.read() & 0x7f);
  db.RTCminute     = bcdToDec(Wire.read());
  db.RTChour       = bcdToDec(Wire.read() & 0x3f);
  db.RTCdayOfWeek  = bcdToDec(Wire.read());
  db.RTCdayOfMonth = bcdToDec(Wire.read());
  db.RTCmonth      = bcdToDec(Wire.read());
  db.RTCyear       = bcdToDec(Wire.read());
 
//--------------------------------------------
    // Wire.beginTransmission(0x68);
    // Wire.write(0x11);
    // Wire.endTransmission();
    // Wire.requestFrom(0x68, 2);
//    temperature = (Wire.read() & B01111111) + ( (Wire.read() >> 6) * 0.25 );
}


String getTimeStr() {
 String str;
  //if(dayOfMonth < 10){str = "0";}
  str += formatDigit(db.RTCdayOfMonth, 2) + "." + formatDigit(db.RTCmonth, 2) + "." + formatDigit(db.RTCyear, 2) + " " + db.RTCdayOfWeek + " " + formatDigit(db.RTChour, 2) + ":" + formatDigit(db.RTCminute, 2) + ":" + formatDigit(db.RTCsecond, 2);
//  str += " temperature " + temperature;
  return str;
}


void setDateDS3231()          
{
  // Use of (byte) type casting and ascii math to achieve result.
  /*second = (byte) ((Serial.read() - 48) * 10 + (Serial.read() - 48));
  minute = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
  hour  = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
  dayOfWeek = (byte) (Serial.read() - 48);
  dayOfMonth = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
  month = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
  year= (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));*/
  Wire.beginTransmission(0x68);
  Wire.write(byte(0x00));
  Wire.write(decToBcd(db.RTCsecond));  // 0 to bit 7 starts the clock
  Wire.write(decToBcd(db.RTCminute));
  Wire.write(decToBcd(db.RTChour));    // If you want 12 hour am/pm you need to set
  // bit 6 (also need to change readDateDs1307)
  Wire.write(decToBcd(db.RTCdayOfWeek));
  Wire.write(decToBcd(db.RTCdayOfMonth));
  Wire.write(decToBcd(db.RTCmonth));
  Wire.write(decToBcd(db.RTCyear));
  Wire.endTransmission();
}


void setTime() {
  Serial.println("Введите последние две цифры года");
  while (Serial.available() <= 0);
  db.RTCyear = Serial.parseInt();
  Serial.println(db.RTCyear);
  Serial.println("Месяц");
  while (Serial.available() <= 0);
  db.RTCmonth = Serial.parseInt();
  Serial.println(db.RTCmonth);
  Serial.println("День");
  while (Serial.available() <= 0);
  db.RTCdayOfMonth = Serial.parseInt();
  Serial.println(db.RTCdayOfMonth);
  Serial.println("День недели");
  while (Serial.available() <= 0);
  db.RTCdayOfWeek = Serial.parseInt();        // Число 1..7 (Пн..Вс)
  Serial.println(db.RTCdayOfWeek);
  Serial.println("Час");
  while (Serial.available() <= 0);
  db.RTChour = Serial.parseInt();
  Serial.println(db.RTChour);
  Serial.println("Минута");
  while (Serial.available() <= 0);
  db.RTCminute = Serial.parseInt();
  Serial.println(db.RTCminute);
  Serial.println("Секунда");
  while (Serial.available() <= 0);
  db.RTCsecond = Serial.parseInt();
  Serial.println(db.RTCsecond);
 
  setDateDS3231();
  Serial.println("Время изменено: " + getTimeStr());
}

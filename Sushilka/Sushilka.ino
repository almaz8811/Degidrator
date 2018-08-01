#include <DHT.h>
#include <DHT_U.h>
#include <LCD_1602_RUS.h>
#include <Time.h>
#include <TimeLib.h>
#include <EEPROM.h>
#define DRIVER_VERSION 0    // 0 - маркировка драйвера дисплея кончается на 4АТ, 1 - на 4Т
#define DROP_ICON 1
#define DHTPIN 6

unsigned long last_time; // Большая переменная времени
unsigned long last_press; // Большая переменная против дребезга кнопок
time_t old_time; //Глобальная переменная времени
time_t new_time;
boolean butt_flag = 1;
boolean butt;
boolean led_flag = 1;
float t;
float h;

// -------- АВТОВЫБОР ОПРЕДЕЛЕНИЯ ДИСПЛЕЯ-------------
// Если кончается на 4Т - это 0х27. Если на 4АТ - 0х3f
#if (DRIVER_VERSION)
LCD_1602_RUS lcd(0x27, 16, 2);
#else
LCD_1602_RUS lcd(0x3f, 16, 2);
#endif
// -------- АВТОВЫБОР ОПРЕДЕЛЕНИЯ ДИСПЛЕЯ-------------

DHT dht(DHTPIN, DHT22);

void setup() {
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  dht.begin(); // Старт датчика DHT22
  EEPROM.get(0, old_time); // получаем значение времени по адресу 0 в переменную old_time
  lcd.init(); // Инициализация дисплея
  lcd.backlight();// Включаем подсветку дисплея
}

String printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  if (digits < 10) {
    return ("0" + String(digits));
  }
  else {
    return String(digits);
  }
}

void light() {
  butt = !digitalRead(5); // считать текущее положение кнопки
  if (butt == 1 && butt_flag == 0 && millis() - last_press > 50) {
    butt_flag = 1;
    //Serial.println("Button pressed");
    led_flag = !led_flag;
    if (led_flag == 0) {
      lcd.noBacklight();
    } else {
      lcd.backlight();
    };
    last_press = millis();
  }
  if (butt == 0 && butt_flag == 1 && millis() - last_press > 50) {
    butt_flag = 0;
    if (led_flag == 0) {
      lcd.noBacklight();
    } else {
      lcd.backlight();
    };
    last_press = millis();
  }
}

void loop() {
  light();
  if (millis() - last_time > 3000) {
    h = dht.readHumidity(); // Считываем влажность
    t = dht.readTemperature(); // Считываем температуру
  }
  if (!digitalRead(4) > 0) {
    lcd.setCursor(0, 0);
    lcd.print("Сброс!!!        ");
    old_time = 0;
    delay(1000);
  };
  lcd.setCursor(0, 0); lcd.print("Темп " + String(t, 0) + " Влаж " + String(h, 0) + " ");
  new_time = old_time + now();
  if (second(new_time) == 0) {
    EEPROM.put(0, new_time);
  }; // записываем новое время работы в ячейку 0 каждую минуту
  // Устанавливаем курсор на вторую строку и нулевой символ.
  lcd.setCursor(0, 1);
  String s = ("Время " + printDigits(day(new_time) - 1) + ":" + printDigits(hour(new_time)) + ":" + printDigits(minute(new_time)));
  lcd.print(s);
}

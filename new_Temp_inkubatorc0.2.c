#include <EEPROM.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>


#ifdef ARDUINO_ESP32C3_DEV
const uint16_t kRecvPin = D5;  // 14 on a ESP32-C3 causes a boot loop.
#else  // ARDUINO_ESP32C3_DEV
const uint16_t kRecvPin = D5;
#endif  // ARDUINO_ESP32C3_DEV


#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;
#define DHT11_PIN D7

IRrecv irrecv(kRecvPin);

decode_results results;

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte customChar[] = {
  0x0A,
  0x0C,
  0x0C,
  0x0A,
  0x00,
  0x15,
  0x0E,
  0x04
};
byte HumanChars[] = {
  0x0E,
  0x0A,
  0x0E,
  0x1F,
  0x0E,
  0x0E,
  0x0A,
  0x0A
};
byte Celsy[] = {
  0x07,
  0x05,
  0x05,
  0x07,
  0x00,
  0x00,
  0x00,
  0x00
};
byte Rain[] = {
  0x15,
  0x00,
  0x15,
  0x0A,
  0x01,
  0x14,
  0x09,
  0x14
};
byte setChars[] = {
  0x1D,
  0x10,
  0x1D,
  0x04,
  0x1D,
  0x00,
  0x01,
  0x00
};

byte lineChars[] = {
  0x0A,
  0x0E,
  0x11,
  0x11,
  0x11,
  0x15,
  0x1F,
  0x1F
};
byte toauch[] = {
  0x00,
  0x00,
  0x0E,
  0x0A,
  0x0E,
  0x00,
  0x00,
  0x00
};
bool present = false;
int state = 0;
String myCharStr;
int count = 0;
int value;
int reseme = 0;
float SAVE_TEMP = 0;
float SAVE_HUMID = 0;
float SAVE_TIMER = 0;
bool change = true;
void setup()
{

  // initialize the LCD
  EEPROM.begin(12);
  Serial.begin(9600);
  lcd.begin(D3,D4);  // sda=0, scl=
  lcd.backlight();
  lcd.createChar(1, customChar);
  lcd.createChar(2, HumanChars);
  lcd.createChar(3, Celsy);
  lcd.createChar(4, Rain);
  lcd.createChar(5, setChars);
  lcd.createChar(6, lineChars);
  lcd.createChar(7, toauch);
  pinMode(D1, OUTPUT);
  pinMode(D3, INPUT);
  irrecv.enableIRIn();  // Start the receiver
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);
  
  
  
  myCharStr = "\1 BigKurka \1";
}

void loop()
{

if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value, HEX);
    Serial.println("");
    irrecv.resume();

    if(results.value == 0xFF629D) // Resume (CH)
    {
      
      if(reseme == 0)
      {
        state = 1;
        count = 0;
        reseme = 1;
        lcd.clear();
      }
      else if(reseme == 1)
      {
        state = 2;
        count = 0;
        reseme = 2;
        lcd.clear();
      }
      else if(reseme == 2)
      {
        state = 3;
        count = 0;
        reseme = 3;
        lcd.clear();
      }else
      {
        state = 0;
        count = 0;
        reseme = 0;
        lcd.clear();
      } 
    }else if(results.value == 0xFFE01F) // Minus
    {
       count = 0;
       if(reseme == 1)
       {
        SAVE_TEMP = SAVE_TEMP - 0.1;
        if(SAVE_TEMP < 30.0) SAVE_TEMP = 40.0;
       }
       else if(reseme == 2)
       {

        SAVE_HUMID = SAVE_HUMID - 0.1; 
        if(SAVE_HUMID < 30.0) SAVE_HUMID = 40.0;
       }
       else if(reseme == 3)
       {
        
        SAVE_TIMER = SAVE_TIMER - 0.1;
        if(SAVE_TIMER < 30.0) SAVE_TIMER = 40.0;
       }
    }
    else if(results.value == 0xFFA857) // Plus
    {
       count = 0;
       if(reseme == 1)
       {
        SAVE_TEMP = SAVE_TEMP + 0.1;
        if(SAVE_TEMP > 40.0) SAVE_TEMP = 30.0;
       }
       else if(reseme == 2)
       {

        SAVE_HUMID = SAVE_HUMID + 0.1;
        if(SAVE_HUMID > 40.0) SAVE_HUMID = 30.0;
       }
       else if(reseme == 3)
       {
        SAVE_TIMER = SAVE_TIMER + 0.1;
        if(SAVE_TIMER > 40.0) SAVE_TIMER = 30.0;
       }
    }else if(results.value == 0xFF906F)//Save (EQ)
    {
      state = 4;
      count = 900;
      lcd.clear();
      if(reseme == 1)
       {
         EEPROM.write(0, SAVE_TEMP);
         EEPROM.commit();
         reseme = 0;
         change = true;
       }
       else if(reseme == 2)
       {

          EEPROM.write(4, SAVE_HUMID);
          EEPROM.commit();
          reseme = 0;
          change = true;
       }
       else if(reseme == 3)
       {
          EEPROM.write(8, SAVE_TIMER);
          EEPROM.commit();
          reseme = 0;
          change = true;
       }
    }
  }

  
 if(!present){
  lcd.setCursor(2,0);
  lcd.print(myCharStr);
  lcd.setCursor(1,1);
  lcd.print("Inkubator v0.2");
  delay(5000);
  present = true;
  lcd.clear();
  Serial.println("EROORRR");
 }
 value = digitalRead(D2);

   // выводим информацию на монитор порта
   Serial.println("VALUE - " + String(value));
   Serial.println(" ");

  count++;
  if(count > 1000)
  {
    count = 0;
    state = 0;
    reseme = 0;
    lcd.clear();
  }
 if(state == 0)
 {
    DHT.read(DHT11_PIN);
    if(DHT.humidity >= 80)
    {
      digitalWrite(D1, HIGH);
    }
    else
    {
      digitalWrite(D1, LOW);
    }
    if(change)
    {
      SAVE_TEMP = EEPROM.read(0);
      SAVE_HUMID = EEPROM.read(4);
      SAVE_TIMER = EEPROM.read(8);
    }
    change = false;
    Serial.printf("SAVE_TEMP - :%d", SAVE_TEMP);
    lcd.setCursor(0,0);
    lcd.printf("\2 %d\3 \4 %d \% \6",DHT.temperature,DHT.humidity);
    lcd.setCursor(0,1);
    lcd.printf("\5 %02.1f\3 \4 %02.1f\% \7",SAVE_TEMP, SAVE_HUMID);
    lcd.setCursor(0,1);
    lcd.printf("\5 %02.1f\3 \4 %02.1f\% |",SAVE_TEMP, SAVE_HUMID);
    lcd.setCursor(0,1);
    lcd.printf("\5 %02.1f\3 \4 %02.1f\% \7",SAVE_TEMP, SAVE_HUMID);
    lcd.setCursor(0,1);
    lcd.printf("\5 %02.1f\3 \4 %02.1f\% |",SAVE_TEMP, SAVE_HUMID);
    Serial.print("temp:");
    Serial.print(DHT.temperature);
    Serial.print("  humi:");
    Serial.println(DHT.humidity);
    
  }else if(state == 1)
  {
    lcd.setCursor(4,0);
    lcd.printf("Set Temp");
    lcd.setCursor(3,1);
    lcd.printf("< %02.1f >",SAVE_TEMP);
  }
  else if(state == 2)
  {
   
    lcd.setCursor(3,0);
    lcd.printf("Set Humid");
    lcd.setCursor(3,1);
    lcd.printf("< %02.1f >",SAVE_HUMID);
  }
  else if(state == 3)
  {
   
    lcd.setCursor(3,0);
    lcd.printf("Set Timer");
    lcd.setCursor(3,1);
    lcd.printf("< %02.1f >",SAVE_TIMER);
  }
  else if(state == 4)
  {
   
    lcd.setCursor(3,0);
    lcd.printf("-- SAVE --");
    lcd.setCursor(0,0);
    lcd.printf(" ");
  }

  
 
}
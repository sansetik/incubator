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
void setup()
{

  // initialize the LCD
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

    if(results.value == 0xFF629D)
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
    }
  }

  
 if(!present){
  lcd.setCursor(2,0);
  lcd.print(myCharStr);
  lcd.setCursor(1,1);
  lcd.print("Inkubator v0.1");
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
  if(count > 800)
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
    lcd.setCursor(0,0);
    lcd.printf("\2 %d\3 \4 %d \% \6",DHT.temperature,DHT.humidity);
    lcd.setCursor(0,1);
    lcd.print("\5 37.5\3 \4 70% \7");
    lcd.setCursor(0,1);
    lcd.print("\5 37.5\3 \4 70% |");
    lcd.setCursor(0,1);
    lcd.print("\5 37.5\3 \4 70% \7");
    lcd.setCursor(0,1);
    lcd.print("\5 37.5\3 \4 70% |");
    Serial.print("temp:");
    Serial.print(DHT.temperature);
    Serial.print("  humi:");
    Serial.println(DHT.humidity);
    
  }else if(state == 1)
  {
    lcd.setCursor(4,0);
    lcd.printf("Set Temp");
    lcd.setCursor(6,1);
    lcd.printf("37.5");
  }
  else if(state == 2)
  {
   
    lcd.setCursor(3,0);
    lcd.printf("Set Humid");
    lcd.setCursor(7,1);
    lcd.printf("65");
  }
  else if(state == 3)
  {
   
    lcd.setCursor(4,0);
    lcd.printf("Set Timer");
    lcd.setCursor(6,1);
    lcd.printf("00:37");
  }

  
 
}
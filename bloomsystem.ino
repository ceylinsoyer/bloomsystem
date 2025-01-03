#include <Wire.h>                
#include <LiquidCrystal_I2C.h>    
#include <SFE_BMP180.h>           
#include <DHT.h>        

#define DHTPIN 2                 
#define DHTTYPE DHT11             
#define SOIL_SENSOR_PIN A0        
#define ALTITUDE 3                

int sensor_pin = A0; 
int in1 = 4; 
int in2 = 5;
int sensor_degeri;
int buzzpin = 7;                  
int trigpin = 10;                
int echopin = 9;                  
int ledPin = 12;             
int buzzpin2=13;

const unsigned long pageInterval = 6000;  
const unsigned long updateInterval = 3000;

LiquidCrystal_I2C lcd(0x27, 16, 2); 

SFE_BMP180 pressure;             
DHT dht(DHTPIN, DHTTYPE);   

unsigned long previousPageMillis = 0;  
unsigned long previousUpdateMillis = 0; 
int currentPage = 0;             

void setup() {
  Serial.begin(9600);          
  
  pinMode(buzzpin, OUTPUT);      
  pinMode(trigpin, OUTPUT);       
  pinMode(echopin, INPUT);          
  pinMode(ledPin, OUTPUT);       
 pinMode(buzzpin2, OUTPUT);
 pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  lcd.init();                     
  lcd.backlight();                
  lcd.setCursor(0, 0);
  lcd.print("Sens. Baslaniyor");
  delay(2000);
  lcd.clear();

  if (pressure.begin()) {
    Serial.println("BMP180 initialization successful");
    Serial.println("Temperature (C)\tPressure (hPa)\tSoil Humidity (%)");
  } else {
    Serial.println("BMP180 initialization failed");
    while (1);  
      }

  
  dht.begin();

  Serial.println("Sensörler başlatılıyor...");
}

void loop() {
  unsigned long currentMillis = millis();



  if (currentMillis - previousUpdateMillis >= updateInterval) {
    previousUpdateMillis = currentMillis;  

   
    int soilValue = analogRead(SOIL_SENSOR_PIN);
    soilValue = map(soilValue, 400, 1023, 100, 0);

    double T, P, p0;
    char status;
    status = pressure.startTemperature();
    if (status != 0) {
      delay(status); 
      status = pressure.getTemperature(T);
      if (status != 0) {
        status = pressure.startPressure(3);
        if (status != 0) {
          delay(status);
          status = pressure.getPressure(P, T);
          if (status != 0) {
            p0 = pressure.sealevel(P, ALTITUDE);
          } else {
            Serial.println("Basınç ölçümü başarısız.");
          }
        }
      }
    }

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("DHT11 sensör okumada hata.");
    } else {
      Serial.print("Sıcaklık: ");
      Serial.print(temperature);
      Serial.println(" °C");
      Serial.print("Nem: ");
      Serial.print(humidity);
      Serial.println(" %");

      if (temperature >= 30) {
        digitalWrite(buzzpin, HIGH);
        delay(200);
        digitalWrite(buzzpin, LOW);
      }
    }

    int distance = measureDistance();
    Serial.print("Mesafe: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance < 15) {
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
    if (distance < 15) {
        digitalWrite(buzzpin2, HIGH);
        delay(200);
        digitalWrite(buzzpin2, LOW);
      }

       sensor_degeri = analogRead(sensor_pin);
  sensor_degeri = map(sensor_degeri, 0, 1023, 0, 100);

   Serial.println(sensor_degeri);
  if (soilValue < 90)
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

}
else if(soilValue>110){   

digitalWrite(in1, LOW);
digitalWrite(in2, HIGH);
}

  if (currentMillis - previousPageMillis >= pageInterval) {
    previousPageMillis = currentMillis;
    currentPage = (currentPage + 1) % 2; 

  if (currentPage == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Basin: ");
    lcd.print((int)p0);
    lcd.print("hPa");
  } else if (currentPage == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Toprak: ");
    lcd.print(soilValue);
    lcd.print("% ");
    lcd.setCursor(0, 1);
    lcd.print("Sic: ");
    lcd.print((int)temperature);
    lcd.print("C ");
  }
}
}
}
int measureDistance() {
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);
  return pulseIn(echopin, HIGH) * 0.034 / 2;
}
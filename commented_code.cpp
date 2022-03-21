#include <OneWire.h>            // Library untuk 1-wire 
#include <DallasTemperature.h>  // Library untuk sensor suhu
#include "DFRobot_PH.h"         // Library untuk sensor pH
#include <Wire.h>               // Library untuk komunikasi dengan I2C
#include <LiquidCrystal_I2C.h>  // Library untuk LCD
#include <EEPROM.h>             // Library untuk ROM mikrokontroler
#include "GravityTDS.h"         // Library untuk sensor TDS

// deklarasi dan inisialisasi variabel
float voltage, phValue, temperature=25, far, tdsValue = 0; 

// deklarasi dan inisialisasi LCD
LiquidCrystal_I2C lcd(0x27, 20, 4); 

// menetapkan pin A1 sebagai ONE_WIRE_BUS
#define ONE_WIRE_BUS A1 

// inisialisasi variabel 'oneWire'
OneWire oneWire(ONE_WIRE_BUS);

// inisialisasi sensor suhu dan meneruskan nilai 'oneWire' sebagai pin-nya
DallasTemperature sensors(&oneWire);

// menetapkan pin A0 sebagai PH_PIN
#define PH_PIN A0

// deklarasi variabel ph
DFRobot_PH ph;

// menetapkan pin A2 sebagai TdsSensorPin
#define TdsSensorPin A2

// deklarasi variabel gravityTDS
GravityTDS gravityTDS;

void setup()
{
    Serial.begin(115200);             // memulai serial moniter pada baud rate 115200
    ph.begin();                       // memulai sensor pH
    sensors.begin();                  // memulai sensor suhu
    lcd.init();                       // memulai LCD
    
    gravityTDS.setPin(TdsSensorPin);  // menentukan pin untuk sensor TDS
    gravityTDS.setAref(5.0);          // menentukan nilai tegangan untuk sensor TDS
    gravityTDS.setAdcRange(1023);     // menentukan rentang Analog-to-Digital Converter (Arduino Uno = 10 bit)
    gravityTDS.begin();               // memulai sensor TDS
    
    // Menampilkan tulisan "pondomo" ketika LCD menyala
    lcd.backlight();
    lcd.setCursor(6,1);
    lcd.print("pondomo");
    delay(2000);
    lcd.clear();
}

void loop()
{
  static unsigned long timepoint = millis();
  sensors.requestTemperatures(); // memulai
  
  if(millis()-timepoint>1000U){                   // interval waktu = 1 detik
    timepoint   = millis();                                     // memperbarui nilai 'timepoint'
    
    temperature = sensors.getTempCByIndex(0);                   // membaca nilai dari sensor suhu
    far         = DallasTemperature::toFahrenheit(temperature); // konversi celcius ke fahrenheit
    
    voltage     = analogRead(PH_PIN) / 1023.0 * 5000;               // membaca nilai tegangan yang diterima pin 'PH_PIN'
    phValue     = ph.readPH(voltage,temperature);               // menghitung nilai pH dengan nilai tegangan dan kompensasi suhu
       
    gravityTDS.setTemperature(temperature);                     // menetapkan nilai suhu dan menghitung nilai kompensasi suhu
    gravityTDS.update();                                        // memperbarui data
    tdsValue    = gravityTDS.getTdsValue();                     // mendapatkan nilai TDS (ppm)

    // Menampilkan nilai 'pH' ke Serial Monitor
    Serial.print("pH : ");
    Serial.println(phValue,2);

    // Menampilkan nilai 'suhu' ke Serial Monitor
    Serial.print("temperature:");
    Serial.print(temperature,1);
    Serial.print("C");
    Serial.print ("|");
    Serial.print (far,2);
    Serial.println("F");

    // Menampilkan nilai 'TDS' ke Serial Monitor
    Serial.print("TDS:");
    Serial.print(tdsValue,0);
    Serial.println("ppm");
    Serial.println( " ");

    // Menampilkan tulisan "SAMPLING" ke baris pertama LCD
    lcd.setCursor(6,0);
    lcd.print ("SAMPLING");

    // Menampilkan nilai 'pH' ke baris kedua LCD
    lcd.setCursor(0,1);
    lcd.print("pH : ");
    lcd.print(phValue,2);

    // Menampilkan nilai 'suhu' ke baris ketiga LCD
    lcd.setCursor(0,2);
    lcd.print("SUHU: ");
    lcd.print(temperature,1);
    lcd.print((char)223);
    lcd.print("C");
    lcd.print("|");
    lcd.print(far,1);
    lcd.print((char)223);
    lcd.print("F");

    // Menampilkan nilai 'TDS' ke baris keempat LCD
    lcd.setCursor(0,3);
    lcd.print("TDS: "); 
    lcd.print(tdsValue,0);
    lcd.setCursor(9,3);
    lcd.print ("ppm");
  }
  // Proses kalibrasi melalui Serial Monitor CLI
  ph.calibration(voltage,temperature);
}
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "DFRobot_PH.h"
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "GravityTDS.h"
float voltage,phValue,temperature=25,far,tdsValue = 0;
LiquidCrystal_I2C lcd(0x27, 20, 4); //The LCD address and size. You can change according you yours
#define ONE_WIRE_BUS A1 //pin for sensor
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

#define PH_PIN A0
DFRobot_PH ph;

#define TdsSensorPin A2
GravityTDS gravityTDS;

void setup()
{
    Serial.begin(115200);  
    ph.begin();
     sensors.begin();
       lcd.init();
        gravityTDS.setPin(TdsSensorPin);
    gravityTDS.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTDS.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTDS.begin();  //initialization
    
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(6,1);
  lcd.print("pondomo");
 
  delay(2000);
  lcd.clear();
     
}
void loop()
{
  
    static unsigned long timepoint = millis();
     sensors.requestTemperatures(); // Send the command to get temperatures
    if(millis()-timepoint>1000U){    //time interval: 1s
      timepoint = millis();
       temperature = sensors.getTempCByIndex(0);        // read your temperature sensor to execute temperature compensation
        voltage = analogRead(PH_PIN)/1024.0*5000;  // read the voltage
        phValue = ph.readPH(voltage,temperature);  // convert voltage to pH with temperature compensation
         far = DallasTemperature::toFahrenheit(temperature);
            gravityTDS.setTemperature(temperature);  // set the temperature and execute temperature compensation
    gravityTDS.update();  //sample and calculate 
    tdsValue = gravityTDS.getTdsValue();  // then get the value

    
        Serial.print("pH : ");
        Serial.println(phValue,2);
        Serial.print("temperature:");
        Serial.print(temperature,1);
        Serial.print("C");
        Serial.print ("|");
        Serial.print (far,2);
        Serial.println("F");
        Serial.print("TDS:");
         Serial.print(tdsValue,0);
    Serial.println("ppm");
        Serial.println( " ");
        
        lcd.setCursor(6,0);
        lcd.print ("SAMPLING");
        lcd.setCursor(0,1);
        lcd.print("pH : ");
        lcd.print(phValue,2);
         lcd.setCursor(0,2);
        lcd.print("SUHU: ");
        lcd.print(temperature,1);
         lcd.print((char)223);
    lcd.print("C");
    lcd.print("|");
    
     lcd.print(far,1);
     lcd.print((char)223);
    lcd.print("F");
lcd.setCursor(0,3);
 lcd.print("TDS: ");
         
lcd.print(tdsValue,0);
lcd.setCursor(9,3);
lcd.print ("ppm");
 
   
    }
    
    ph.calibration(voltage,temperature);           // calibration process by Serail CMD
}

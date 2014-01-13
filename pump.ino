#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define SETPOINT 0.12
#define STD_ERR 0.01
#define LHL SETPOINT-STD_ERR
#define RHL SETPOINT+STD_ERR

//
LiquidCrystal lcd(2, 3, 4, 5, 6, 7, 10);

int salinity_trigger = 11;
int salinity_pin = A5;


void setup()
{
    lcd.begin(20, 4);
    Serial.begin(9600);
}

void loop()
{
    // for the main loop
    // we want to check the salinity
    // if salinity is below LHL OR above RHL, correct it
    int salinity = salinity();

    if (salinity < LHL) {

    } else if (salinity > RHL) {

    }

}




int salinity()
{
    digitalWrite(salinity_trigger, HIGH);
    delay(100);
    int x = analogRead(salinity_pin);
    digitalWrite(salinity_trigger, LOW);
    return x;
}
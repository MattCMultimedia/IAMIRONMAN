#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

// SETPOINT + LHL AND RHL
float SETPOINT = 0.125;
float STD_ERR = 0.011;
float LHL = SETPOINT-STD_ERR;
float RHL = SETPOINT+STD_ERR;

// Gain
#define GAIN 0.9
// delay after upset in ms
#define UPSET_DELAY 20000
// %wt of the salt in the salty container
#define PWT_SALT = 0.25
// flow rate in liters per second
float FLOW_RATE_DI = 0.408;
float FLOW_RATE_S = 0.4266;
#define OFR 0.15
#define MASS 0.073303037714

// lcd
LiquidCrystal lcd(2, 3, 4, 5, 6, 7, 10);

// pin that powers salinity sensor
int salinity_trigger = 11;
// analog pin that measures voltage drop over 10kOhm resistor
int salinity_pin = A0;
// soleniod salt
int ss = 9;
// solenoid di
int sdi = 8;

bool saltyState = false;
bool diState = false;
float salinity = 0;




void setup()
{

    pinMode(salinity_trigger, OUTPUT);
    pinMode(salinity_pin, INPUT);
    pinMode(ss, OUTPUT);
    pinMode(sdi, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(10, OUTPUT);

    lcd.begin(20, 4);
    Serial.begin(115200);

    initLCD();

    delay(5000);

}

void loop()
{
    static double target;
    // for the main loop
    // we want to check the salinity
    // if salinity is below LHL OR above RHL, correct it

    // test flow rate


    salinity = analogToSalinity(getSalinity());
    Serial.print("SALINITY: ");
    Serial.println(salinity, 4);
    // initLCD();
    updateLCD();
    // delay(10);

    if (salinity < LHL || salinity > RHL) {
        Serial.println("UPSET SHIT");

        // delay UPSET_DELAY
        unsigned long prev = millis();
        Serial.println("CHILLING FOR UPSET_DELAY");
        while (millis() - prev < UPSET_DELAY) {
            salinity = analogToSalinity(getSalinity());
            Serial.print("SALINITY: ");
            Serial.println(salinity, 4);
            updateLCD();
            delay(1000);
        }

        salinity = analogToSalinity(getSalinity());
        Serial.print("NEW SALINITY: ");
        Serial.println(salinity, 4);


        // salinity is in %wt
        if (salinity < LHL) {
            // needs salt

            // calulate target using gain
            target = (salinity + (SETPOINT-salinity)*GAIN);
            Serial.print("TARGET: ");
            Serial.println(target, 4);

            openSaltForSeconds(getSaltSecondsForSetpoint(target));

        } else if (salinity > RHL) {
            // needs DI water

            // calc target
            target = (salinity - (salinity-SETPOINT)*GAIN);
            Serial.print("TARGET: ");
            Serial.println(target, 4);

            openDIForSeconds(getDISecondsForSetpoint(target));

        } else {
            // eh. whatver, it fixed itself I guess.
        }

    } else {

    }

}




float getSalinity()
{
    // averages salinity over course of 2 seconds.
    float x = 0.0;
    int samples = 2;

    for(int i=0; i<samples; i++){
        digitalWrite(salinity_trigger, HIGH);
        delay(100);
        x += analogRead(salinity_pin);
        digitalWrite(salinity_trigger, LOW);
        delay(100);
    }
    return (float)(x/samples);
}

void openDIForSeconds(float seconds)
{
    Serial.print("Opening DI for Seconds: ");
    Serial.println(seconds, 5);
    diState = true;
    updateLCD();
    digitalWrite(sdi, HIGH);
    delay(seconds*1000);
    digitalWrite(sdi, LOW);
    diState = false;
    updateLCD();
}

void openSaltForSeconds(float seconds)
{
    Serial.print("Opening Salt for Seconds: ");
    Serial.println(seconds, 5);
    saltyState = true;
    updateLCD();
    digitalWrite(ss, HIGH);
    delay(seconds*1000);
    digitalWrite(ss, LOW);
    saltyState = false;
    updateLCD();
}

float salinityToAnalog(float s)
{
    return 85.0841365834 * log(s) + 757.7391914623;
}

float analogToSalinity(float a)
{
    return 0.5775389110 * log(a) - 3.5466300402;
}

float getSaltSecondsForSetpoint(double target)
{
    // using equation developed in HW #7
    float m_needed = ((target-salinity)*MASS)/((1.0-OFR)*salinity);
    return (60.0 * (m_needed/FLOW_RATE_S));
}
float getDISecondsForSetpoint(double target)
{
    // using equation developed in HW #7
    // returns seconds
    float m_needed = ((salinity-target)*MASS)/((1.0-OFR)*salinity);
    return 60.0 * (m_needed/FLOW_RATE_DI);
}

void initLCD()
{
    lcd.setCursor(1,0);
    lcd.print("LCL    SP     UCL");
    lcd.setCursor(1,1);
    lcd.print(LHL, 3);
    lcd.setCursor(7,1);
    lcd.print(SETPOINT, 3);
    lcd.setCursor(14,1);
    lcd.print(RHL, 3);

    // salty valve state
    lcd.setCursor(1,2);
    lcd.print("OFF");

    // current salinity
    lcd.setCursor(7,2);
    lcd.print("0.000");

    // DI state
    lcd.setCursor(15,2);
    lcd.print("OFF");

    lcd.setCursor(0,3);
    lcd.print("SALTY CURRENT  DI");

}

void updateLCD()
{
    delay(500);
    // salty valve state
    lcd.setCursor(1,2);
    lcd.print("   ");
    lcd.setCursor(1,2);
    lcd.print(saltyState?"ON":"OFF");

    // current salinity
    lcd.setCursor(7,2);
    lcd.print("     ");
    lcd.setCursor(7,2);
    lcd.print(salinity, 3);


    // DI state
    lcd.setCursor(15,2);
    lcd.print("   ");
    lcd.setCursor(15,2);
    lcd.print(diState?"ON":"OFF");
    delay(500);
}
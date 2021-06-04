#include "mbed.h"
#include "DHT.h"
#include "TextLCD.h"
#include <vector>
#include <string>
#include <stdio.h>  //float-to-string
#include "scd30.h"

#define MAX_SAMPLE_COUNT        3
#define ERROR_VALUE             -99
#define LCD_TIMEOUT_MS          5000 

/**
 * Note:
 * the D0 and D1 pins are not available per default as 
 * they are used by the STLink Virtual Comm Port
 */ 

DigitalOut led1(LED1);
DigitalIn btn(USER_BUTTON);

DHT dht(D8, DHT::DHT22);

// Sharp LM16A21 16x2 LCD, 5V - 4-bit parallel bus
// note: wire VCC to 5V
// note: wire R/W pin to GND (=write)
// note: wire VO with a 10k pot between 5V and GND
TextLCD lcd(D2, D3, D4, D5, D6, D7, TextLCD::LCD16x2);  //rs, e, d4-d7
bool lcdIsOn;
bool btnEventHandled;

SCD30 co2sensor(D14, D15, 400000);


std::vector<float> tempbuffer;     //buffer to mean out the temperature results
std::vector<float> humbuffer;      //buffer to mean out the humidity results
std::vector<float> co2buffer;   //buffer to mean out the eCO2 results


void waitAndBlink50ms(int loops)
{
    while (loops>0) 
    {
        led1 = !led1;
        wait_ms(50);
        loops--;
    }
    led1 = false;
}


void readSensors()
{
    // read temp sensor
    int dhtReadOk = dht.read();

    //read co2sensor
    uint8_t co2readOk = 0; //(ok)
    if(co2sensor.getReadyStatus() == SCD30::SCDnoERROR 
        && co2sensor.scdSTR.ready == SCD30::SCDisReady) 
    {
        co2readOk = co2sensor.readMeasurement();
    }

    if (dhtReadOk == DHT::SUCCESS && co2readOk == SCD30::SCDnoERROR) 
    {
        // on success add sample to buffer and remove oldest sample if we have 3
        tempbuffer.insert (tempbuffer.begin(), dht.getTemperature());
        if(tempbuffer.size() >  MAX_SAMPLE_COUNT)
            tempbuffer.pop_back();
        humbuffer.insert (humbuffer.begin(), dht.getHumidity());
        if(humbuffer.size() >  MAX_SAMPLE_COUNT)
            humbuffer.pop_back();
        co2buffer.insert (co2buffer.begin(), co2sensor.scdSTR.co2f);
        if(co2buffer.size() >  MAX_SAMPLE_COUNT)
            co2buffer.pop_back();
    } else {
        //on error just pop older results
        tempbuffer.pop_back();
        humbuffer.pop_back();
        co2buffer.pop_back();
    }
}


float getMean(std::vector<float>& buffer)
{
    if(buffer.size() != MAX_SAMPLE_COUNT)
        return ERROR_VALUE;

    // iterate and calculate mean value
    float total = 0;
    for(unsigned i=0; i<buffer.size(); i++) {
        total += buffer[i];
    }
    return total/buffer.size();
}


std::string fl2str(float val, const std::string& format="%0.1f")
{
    int len = snprintf(NULL, 0, format.c_str(), val);
    char *buff = (char *)malloc(len + 1);
    snprintf(buff, len + 1, format.c_str(), val);
    // do stuff with result
    std::string result(buff);
    free(buff);
    return result;
}


void toggleDisplayOnOff()
{
    lcdIsOn = !lcdIsOn;
    if(lcdIsOn) {
        lcd.setMode(TextLCD::LCDMode::DispOn);
    } else {
        lcd.setMode(TextLCD::LCDMode::DispOff);
    }
}


void waitAndHandleButton(int ms)
{
    for(int i=0; i<ms; i+=100) 
    {
        if(btn == 1) { //button NOT PRESSED
            btnEventHandled = false;
        } else { //button PRESSED
            if(!btnEventHandled) {
                btnEventHandled = true;
                toggleDisplayOnOff();
            }
        }
        wait_ms(100); 
    }
}


void loop()
{
    led1 = true; //illuminate LED while reading
    readSensors();
    wait_ms(50); //give LED some time to illuminate
    led1 = false;


    lcd.cls();
    float temp = getMean(tempbuffer);
    float hum = getMean(humbuffer);
    float co2 = getMean(co2buffer);
    if(temp != ERROR_VALUE && hum != ERROR_VALUE) {
        printf("%.1f;%.1f;%.1f\n", temp, hum, co2);
        std::string lcdtxt = 
            std::string("T:") +
            fl2str(temp) +
            std::string("C RH:") +
            fl2str(hum) +
            std::string("\nCO2:") +
            fl2str(co2, "%0.0f") +
            std::string(" PPM");
        
        lcd.printf(lcdtxt.c_str());
    }
    else {
        printf("ERR\n");
        lcd.printf("Collecting...");
    }

    //wait 3 seconds before taking next sample
    waitAndHandleButton(3000);
}


bool initSCD30()
{
    co2sensor.softReset();
    // give sensor 2 seconds to reset
    waitAndBlink50ms(40);

    if(co2sensor.getSerialNumber() != SCD30::SCDnoERROR)
        return false;

    co2sensor.setMeasInterval(5);
    co2sensor.startMeasurement(0);
    return true;
}


int main()
{
    printf("START\n");
    lcd.setCursor(TextLCD::LCDCursor::CurOff_BlkOff);
    lcd.printf("Sensor init...\n");

    bool sensorOK = initSCD30();

    if(sensorOK == true) {
        while (true) 
        {
            loop();
        }
    } else {
        toggleDisplayOnOff();
        printf("ERR: %d\n", sensorOK);
        lcd.printf("Sensor error!\n");
        while(true) 
        {
            waitAndBlink50ms(20);
            if(btn == 1)  //button NOT PRESSED
                btnEventHandled = true;
            if(btn == 0 && btnEventHandled)
                system_reset();
        }
    }

    return 0;
}

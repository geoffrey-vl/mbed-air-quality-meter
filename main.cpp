#include "mbed.h"
#include "DHT.h"
#include "TextLCD.h"
#include <vector>
#include <string>
#include <stdio.h>  //float-to-string

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
int lcdMillisOn;

std::vector<float> tempbuffer; //buffer to mean out the temperature results
std::vector<float> humbuffer;  //buffer to mean out the humidity results


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


void readSensor()
{
    int err = dht.read();
    if (err == DHT::SUCCESS) 
    {
        // on success add sample to buffer and remove oldest sample if we have 3
        tempbuffer.insert (tempbuffer.begin(), dht.getTemperature());
        if(tempbuffer.size() >  MAX_SAMPLE_COUNT)
            tempbuffer.pop_back();
        humbuffer.insert (humbuffer.begin(), dht.getHumidity());
        if(humbuffer.size() >  MAX_SAMPLE_COUNT)
            humbuffer.pop_back();
    } else {
        //on error just pop older results
        tempbuffer.pop_back();
        humbuffer.pop_back();
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


std::string tostring(float val)
{
    int len = snprintf(NULL, 0, "%0.1f", val);
    char *buff = (char *)malloc(len + 1);
    snprintf(buff, len + 1, "%0.1f", val);
    // do stuff with result
    std::string result(buff);
    free(buff);
    return result;
}


void handleBtnOn() 
{
    if(lcdMillisOn != 0) {
        lcd.setMode(TextLCD::LCDMode::DispOn);
        lcdMillisOn = 0;
    }
}


void handleBtnOff()
{
    if(lcdMillisOn < LCD_TIMEOUT_MS)
        lcdMillisOn += 100;
    else {
        lcd.setMode(TextLCD::LCDMode::DispOff);
    }
}


void waitAndHandleButton(int ms)
{
    for(int i=0; i<ms; i+=100) 
    {
        if(btn == 1) {
            handleBtnOff();
        } else {
            handleBtnOn();
        }
        wait_ms(100); 
    }
}


void loop()
{
    led1 = true; //illuminate LED while reading

    lcd.cls();
    readSensor();

    float temp = getMean(tempbuffer);
    float hum = getMean(humbuffer);
    if(temp != ERROR_VALUE && hum != ERROR_VALUE) {
        printf("%.1f;%.1f\n", temp, hum);
        std::string lcdtxt = 
            std::string("TEMP: ") +
            tostring(temp) +
            std::string(" gr.C\n HUM: ") +
            tostring(hum) +
            std::string(" pc");
        
        lcd.printf(lcdtxt.c_str());
    }
    else {
        printf("ERR\n");
        lcd.printf("Collecting...");
    }

    //give LED some time to illuminate, and wait
    //3 seconds before taking next sample
    wait_ms(50);
    led1 = false;
    waitAndHandleButton(3000);
}


int main()
{
    //give sensor 1 second to init
    waitAndBlink50ms(20);

    lcd.setCursor(TextLCD::LCDCursor::CurOff_BlkOff);
    lcdMillisOn = -5000; //normally is reset to 0, but at startup we want 5s additional time
    
    while (true) 
    {
        loop();
    }
    return 0;
}

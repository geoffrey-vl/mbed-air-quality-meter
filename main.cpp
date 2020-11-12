#include "mbed.h"
#include "DHT.h"
#include <vector>

#define MAX_SAMPLE_COUNT        3
#define ERROR_VALUE             -99

DigitalOut led1(LED1);
DigitalIn btn(USER_BUTTON);

DHT dht(D8, DHT::DHT22);

std::vector<float> tempbuffer; //buffer to mean out the results
std::vector<float> humbuffer; //buffer to mean out the results

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


int main()
{
    //give sensor 1 second to init
    waitAndBlink50ms(20);
    
    while (true) 
    {
        led1 = true; //illuminate LED while reading

        readSensor();

        float temp = getMean(tempbuffer);
        float hum = getMean(humbuffer);
        if(temp != ERROR_VALUE && hum != ERROR_VALUE)
            printf("%.1f;%.1f\n", temp, hum);
        else
            printf("ERR\n");

        //give LED some time to illuminate, and wait
        //3 seconds before taking next sample
        wait_ms(50);
        led1 = false;
        wait_ms(3000);     
    }
    return 0;
}

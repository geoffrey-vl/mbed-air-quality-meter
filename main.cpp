#include "mbed.h"
#include "DHT.h"
#include <vector>

#define MAX_SAMPLE_COUNT        3

DigitalOut led1(LED1);
DigitalIn btn(USER_BUTTON);

DHT dht(D8, DHT::DHT22);

std::vector<float> buffer; //buffer to mean out the results


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


int main()
{
    //give sensor 1 second to init
    waitAndBlink50ms(20);
    
    while (true) 
    {
        led1 = true; //illuminate LED while reading

        int err = dht.read();
        if (err == DHT::SUCCESS) 
        {
            // on succes add sample to buffer and remove oldest sample if we have 3
            float sample = dht.getTemperature();
            buffer.insert (buffer.begin(), sample);
            if(buffer.size() >  MAX_SAMPLE_COUNT)
                buffer.pop_back();
        } else {
            //on error just pop older results
            buffer.pop_back();
        }


        if(buffer.size() > 0) 
        {
            // iterate and calculate mean value
            float total = 0;
            for(unsigned i=0; i<buffer.size(); i++) {
                total += buffer[i];
            }
            float mean = total/buffer.size();
            printf("%.1f\n", mean);
        } else
        {
            printf("ERR\n");
        }

        //give LED some time to illuminate, and wait
        //3 seconds before taking next sample
        wait_ms(50);
        led1 = false;
        wait_ms(3000);     
    }
    return 0;
}

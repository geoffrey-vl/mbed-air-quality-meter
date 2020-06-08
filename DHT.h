/*
 *  DHT Library for Digital-output Humidity and Temperature sensors
 *
 *  Tested with DHT11, DHT22
 *  Compatible with SEN11301P, SEN51035P, AM2302, HM2303
 *
 *  Copyright (C) Anthony Hinsinger
 *                Inspired from Wim De Roeve MBED library code
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _DHT_H_
#define _DHT_H_

#include "mbed.h"

/** Read DHT11/22 humidity and temperature sensors
 * 
 * DHT11
 * - Ultra low cost
 * - 3 to 5V power and I/O
 * - 2.5mA max current use during conversion (while requesting data)
 * - Good for 20-80% humidity readings with 5% accuracy
 * - Good for 0-50°C temperature readings ±2°C accuracy
 * - No more than 1 Hz sampling rate (once every second)
 * - Body size 15.5mm x 12mm x 5.5mm
 * - 4 pins with 0.1" spacing
 * 
 * DHT22
 * - Low cost
 * - 3 to 5V power and I/O
 * - 2.5mA max current use during conversion (while requesting data)
 * - Good for 0-100% humidity readings with 2-5% accuracy
 * - Good for -40 to 80°C temperature readings ±0.5°C accuracy
 * - No more than 0.5 Hz sampling rate (once every 2 seconds)
 * - Body size 15.1mm x 25mm x 7.7mm
 * - 4 pins with 0.1" spacing
 * 
 * As you can see, the DHT22 is a little more accurate and good over a slightly larger range. 
 * Both use a single digital pin and are 'sluggish' in that you can't query 
 * them more than once every second or two.
 * 
 * Example:
 * @code
 * // read DHT22 every 3 minutes
 *
 * #include "mbed.h"
 * #include "DHT.h"
 * 
 * DHT sensor(D8, DHT::DHT22);
 *
 * int main(void) {
 *     while(1) {
 *         wait(3);
 *         int err = sensor.read();
 *         if (err == DHT::SUCCESS) {
 *             printf("T: %.1f\r\n", sensor.getTemperature(DHT::CELCIUS));
 *         } else {
 *             printf("Error code : %d\r\n", err);
 *         }
 *     }
 * }
 * @endcode
 */
class DHT {

public:

    enum Family {
        DHT11,
        DHT22
    };

    enum Status {
        SUCCESS,
        ERROR_BUS_BUSY,
        ERROR_NOT_DETECTED,
        ERROR_BAD_START,
        ERROR_SYNC_TIMEOUT,
        ERROR_DATA_TIMEOUT,
        ERROR_BAD_CHECKSUM,
        ERROR_TOO_FAST,
    };

    enum Unit {
        CELCIUS,
        FARENHEIT,
        KELVIN,
    };

    DHT(PinName pin, Family DHTtype);
    ~DHT();

    /** Read data on sensor
     * 
     * @returns an error code. See ::DHTError
     */
    int read(void);

    /** Get pointer to raw data (can be useful to send it througt a low-power WAN)
    */
    int* getRawData();

    /** Get humidity from the last succesful read
    */
    float getHumidity(void);

    /** Get temperature from the last succesful read
    */
    float getTemperature(Unit unit = CELCIUS);

private:
    PinName _pin;
    Family _family;
    time_t  _lastReadTime;
    float _lastTemperature;
    float _lastHumidity;
    int _data[5];
    float calcTemperature();
    float calcHumidity();
    float toFarenheit(float);
    float toKelvin(float);
};

#endif
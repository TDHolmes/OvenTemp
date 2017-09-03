/*! A prototype of the OvenTemp project using
 *  the Adalogger M0+ feather dev board.
 */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define V_IN           (3.3f)
#define DISP_DP_MASK   (0b0100000000000000)
#define DISP_ALL_ON    (0b1111111111111111)
#define NUM_AVERAGES   (10)

#define WRITE_ALL_ON


static const uint16_t num_to_disp_lookup[] PROGMEM =  {
    0b0000110000111111, // 0
    0b0000000000000110, // 1
    0b0000000011011011, // 2
    0b0000000010001111, // 3
    0b0000000011100110, // 4
    0b0010000001101001, // 5
    0b0000000011111101, // 6
    0b0000000000000111, // 7
    0b0000000011111111, // 8
    0b0000000011101111, // 9
};

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
// TODO: make this a switch
bool convert_to_f = true;

    
void setup()
{
    // Enable serial output
    // Serial.begin(115200);
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);
    
    alpha4.begin(0x70);  // pass in the address
    alpha4.clear();
    alpha4.setBrightness(1);
    alpha4.writeDisplay();
}


void loop()
{
    int start_time = millis();
    float temperature = getReading();
    if (convert_to_f) {
        temperature = c2f(temperature);
    }
    displayTemp(temperature);
    // Serial.print("Temp output: ");
    // Serial.println(temperature);
    delay(100);
    // Serial.print("Loop took ");
    // Serial.print(millis() - start_time);
    // Serial.println(" miliseconds");
}


/*! Gets a reading from the thermocouple, returning
 *  the value in celsius.
 */
float getReading(void)
{
    float sensor = 0;
    float v1_25 = 0;
    // Turn on the thermocouple and wait for it to come on
    digitalWrite(10, LOW);
    delay(1);

    // Gather some readings and average them
    for (int i = NUM_AVERAGES; i > 0; i--) {
        sensor += (float)analogRead(A1);
        v1_25 += (float)analogRead(A2);
    }
    digitalWrite(10, HIGH);

    // Normalize and send out
    sensor = sensor / NUM_AVERAGES;
    v1_25 = v1_25 / NUM_AVERAGES;
    float vout = sensor * (1.25 / v1_25);
    return (vout - 1.25) / 0.005;
}


float c2f(float celsius_data)
{
    return celsius_data * (9.0 / 5.0) + 32.0;
}


void displayTemp(float temp)
{
    uint8_t a,b,c,d = 0;
    if (temp < 100) {
        a = (uint8_t)(((uint32_t)temp % 100) / 10);   // 10's place
        b = (uint8_t)((uint32_t)temp % 10);           // 1's place
        c = (uint8_t)((uint32_t)(temp*10.0) % 10);      // 10ths place
        d = (uint8_t)((uint32_t)(temp*100.0) % 10);     // 100ths place

        // Write the dot out
        alpha4.writeDigitRaw(1, num_to_disp_lookup[b] | DISP_DP_MASK);
        alpha4.writeDigitRaw(2, num_to_disp_lookup[c]);
    } else {
        a = (uint8_t)(((uint32_t)temp % 1000) / 100); // 100's place
        b = (uint8_t)(((uint32_t)temp % 100) / 10);   // 10's place
        c = (uint8_t)((uint32_t)temp % 10);           // 1's place
        d = (uint8_t)((uint32_t)(temp*10.0) % 10);      // 10ths place
        
        // Write the dot out
        alpha4.writeDigitRaw(1, num_to_disp_lookup[b]);
        alpha4.writeDigitRaw(2, num_to_disp_lookup[c] | DISP_DP_MASK);
    }

    //write out the rest
    if (temp < 10) {
        alpha4.writeDigitRaw(0, 0);
    } else {
        alpha4.writeDigitRaw(0, num_to_disp_lookup[a]);
    }
    alpha4.writeDigitRaw(3, num_to_disp_lookup[d]);

    #ifdef WRITE_ALL_ON
        alpha4.writeDigitRaw(0, DISP_ALL_ON);
        alpha4.writeDigitRaw(1, DISP_ALL_ON);
        alpha4.writeDigitRaw(2, DISP_ALL_ON);
        alpha4.writeDigitRaw(3, DISP_ALL_ON);
    #endif
    alpha4.writeDisplay();
}


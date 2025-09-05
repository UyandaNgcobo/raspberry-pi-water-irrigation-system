/*The following program's code is responsible for making the water pump we've hooked up to our pico work through the use 
*of data it recieves from the soil moisture sensor. It's been programmed by second year computer science students:
*Andile Mbokazi
*Lesiluse Maghebe
*Uyanda Ngcobo
*Date: 2024-06-15
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "soil_moisture.h"   // use the modular soil moisture functions

#define WATER_PUMP_PIN 14
#define SOIL_MOISTURE_THRESHOLD 30.0
#define CHECK_INTERVAL_MS 5000
#define PUMP_DURATION_MS 30000

void setup() {
    stdio_init_all();
    gpio_init(WATER_PUMP_PIN);
    gpio_set_dir(WATER_PUMP_PIN, GPIO_OUT);
    gpio_put(WATER_PUMP_PIN, 0);

    // Init soil moisture sensor (with DMA)
    soil_moisture_init();
}

int main() {
    setup();

    while (1) {
        float soil_moisture = soil_moisture_read();
        printf("Soil Moisture: %.2f%%\n", soil_moisture);

        if (soil_moisture < SOIL_MOISTURE_THRESHOLD) {
            printf("Soil moisture low → Pump ON\n");
            gpio_put(WATER_PUMP_PIN, 1);
            sleep_ms(PUMP_DURATION_MS);
            gpio_put(WATER_PUMP_PIN, 0);
            printf("Pump OFF\n");
        } else {
            printf("Soil moisture OK → Pump OFF\n");
        }

        sleep_ms(CHECK_INTERVAL_MS);
    }
}

// End of water_pump.c


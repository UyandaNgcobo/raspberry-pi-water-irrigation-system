soil_moisture.c
/*This program calculates soil moisture content based on input parameters.
It uses a simple empirical formula for demonstration purposes.
Author: Your Andile Mbokazi, Uyanda Ngcobo and Lesiluse maghebe
Date: 2024-06-15
Version: 1.0
Second year computer science students at eduvos
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define SOIL_MOISTURE_PIN 26 // GPIO pin connected to the soil moisture sensor
#define ADC_MAX_VALUE 4095.0 // Maximum value for 12-bit ADC
#define VREF 3.3 // Reference voltage for ADC
#define CALIBRATION_DRY 3000 // ADC value for dry soil (calibration)
#define CALIBRATION_WET 1000 // ADC value for wet soil (calibration)
#define MEASUREMENT_INTERVAL_MS 5000 // Measurement interval in milliseconds
#define LED_PIN 25 // On-board LED pin

float calculate_soil_moisture(int adc_value) {
    // Simple linear interpolation between dry and wet calibration values
    if (adc_value >= CALIBRATION_DRY) {
        return 0.0; // 0% moisture
    } else if (adc_value <= CALIBRATION_WET) {
        return 100.0; // 100% moisture
    } else {
        return ((CALIBRATION_DRY - adc_value) / (CALIBRATION_DRY - CALIBRATION_WET)) * 100.0;
    }
}

void setup() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(SOIL_MOISTURE_PIN);
    adc_select_input(0); // Select ADC input 0 (GPIO 26)
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

int main() {
    setup();
    while (1) {
        int adc_value = adc_read(); // Read ADC value
        float soil_moisture = calculate_soil_moisture(adc_value); // Calculate soil moisture percentage

        // Print the results
        printf("ADC Value: %d, Soil Moisture: %.2f%%\n", adc_value, soil_moisture);

        // Blink the LED to indicate a measurement has been taken
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);

        sleep_ms(MEASUREMENT_INTERVAL_MS); // Wait for the next measurement
    }
    return 0;
}

// End of soil_moisture.c
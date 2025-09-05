/*The following program's code is responsible for making the water pump we've hooked up to our pico work through the use of 
data it recieves from the soil moisture sensor. It's been programmed by second year computer science students:
*Andile Mbokazi
*Lesiluse Maghebe
*Uyanda Ngcobo
*Date: 2024-06-15
*/

#include <stdio.h>
#include "pico/stdlib.h"   
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/adc.h"
#include "soil_moisture.c" // Include the soil moisture sensor code

#define WATER_PUMP_PIN 14 // GPIO pin connected to the water pump
#define SOIL_MOISTURE_THRESHOLD 30.0 // Soil moisture threshold percentage to activate the pump
#define CHECK_INTERVAL_MS 5000 // Interval to check soil moisture in milliseconds
#define PUMP_DURATION_MS 3000 // Duration to run the pump in milliseconds
#define SOIL_MOISTURE_PIN 26 // GPIO pin connected to the soil moisture sensor
#define ADC_MAX_VALUE 4095.0 // Maximum value for 12-bit ADC
#define VREF 3.3 // Reference voltage for ADC
#define CALIBRATION_DRY 3000 // ADC value for dry soil (calibration)
#define CALIBRATION_WET 1000 // ADC value for wet soil (calibration)
#define MEASUREMENT_INTERVAL_MS 5000 // Measurement interval in milliseconds

void setup() {
    stdio_init_all();
    gpio_init(WATER_PUMP_PIN);
    gpio_set_dir(WATER_PUMP_PIN, GPIO_OUT);
    gpio_put(WATER_PUMP_PIN, 0); // Ensure pump is off initially
    adc_init();
    adc_gpio_init(SOIL_MOISTURE_PIN);
    adc_select_input(0); // Select ADC input 0 (GPIO 26)
}

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

int main() {
    setup();
    while (1) {
        int adc_value = adc_read(); // Read ADC value
        float soil_moisture = calculate_soil_moisture(adc_value); // Calculate soil moisture percentage

        // Print the results
        printf("ADC Value: %d, Soil Moisture: %.2f%%\n", adc_value, soil_moisture);

        // Check if soil moisture is below the threshold
        if (soil_moisture < SOIL_MOISTURE_THRESHOLD) {
            printf("Soil moisture below threshold. Activating water pump.\n");
            gpio_put(WATER_PUMP_PIN, 1); // Turn on the pump
            sleep_ms(PUMP_DURATION_MS); // Run the pump for the specified duration
            gpio_put(WATER_PUMP_PIN, 0); // Turn off the pump
            printf("Water pump deactivated.\n");
        } else {
            printf("Soil moisture above threshold. Pump remains off.\n");
        }

        sleep_ms(CHECK_INTERVAL_MS); // Wait before the next check
    }
    return 0;
}

// End of water_pump.c


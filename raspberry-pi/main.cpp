/*
 * Smart Irrigation System - Raspberry Pi Pico 2W
 * Authors: Andile Mbokazi, Uyanda Ngcobo, Lesiluse Magheba
 * Description:
 *   This program monitors soil moisture sensors and controls relay modules
 *   to activate sprinklers automatically when soil dryness is detected.
 *   Built for ITCAA2-33 Project, Question 2 functionalities.
 */

#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"

// ---------------- Pin Configuration ---------------- //
#define THRESHOLD 2000  // Moisture threshold value (0-4095 from ADC)

// Soil moisture sensors (analog pins)
const uint SENSOR_PINS[2] = {26, 27};  // ADC0 (GP26), ADC1 (GP27)

// Relay control pins (digital output)
const uint RELAY_PINS[2]  = {2, 3};    // GP2, GP3

// ---------------- Function Prototypes ---------------- //
void init_system();
uint16_t read_moisture(uint channel);
void control_sprinkler(uint zone, uint16_t moisture);

// ---------------- Main Program ---------------- //
int main() {
    stdio_init_all();
    init_system();

    while (true) {
        for (int zone = 0; zone < 2; zone++) {
            uint16_t moisture = read_moisture(zone);

            // Print moisture levels for debugging
            printf("Zone %d Moisture: %d\n", zone + 1, moisture);

            // Control sprinkler for this zone
            control_sprinkler(zone, moisture);
        }

        sleep_ms(1000);  // Check every second
    }
}

// ---------------- Function Definitions ---------------- //

// Initialize ADC for sensors and GPIO for relays
void init_system() {
    // Initialize ADC hardware
    adc_init();
    for (int i = 0; i < 2; i++) {
        adc_gpio_init(SENSOR_PINS[i]);
    }

    // Initialize relay control pins as outputs
    for (int i = 0; i < 2; i++) {
        gpio_init(RELAY_PINS[i]);
        gpio_set_dir(RELAY_PINS[i], true);
        gpio_put(RELAY_PINS[i], 0);  // Ensure sprinklers start OFF
    }

    printf("System Initialized...\n");
}

// Read moisture level from a specific ADC channel (0 or 1)
uint16_t read_moisture(uint channel) {
    adc_select_input(channel);
    return adc_read();
}

// Control sprinkler relay for a specific zone
void control_sprinkler(uint zone, uint16_t moisture) {
    if (moisture < THRESHOLD) {
        gpio_put(RELAY_PINS[zone], 1);  // Turn ON sprinkler
        printf("Zone %d: DRY - Sprinkler ON\n", zone + 1);
    } else {
        gpio_put(RELAY_PINS[zone], 0);  // Turn OFF sprinkler
        printf("Zone %d: WET - Sprinkler OFF\n", zone + 1);
    }
}
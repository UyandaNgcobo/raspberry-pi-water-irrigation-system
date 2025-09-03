#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define PIR_PIN 2  // GPIO pin 2 (same as Arduino example)

int main() {
    // Initialize stdio for serial communication
    stdio_init_all();
    
    // Initialize GPIO
    gpio_init(PIR_PIN);
    gpio_set_dir(PIR_PIN, GPIO_IN);
    
    // Optional: Enable pull-down resistor
    gpio_pull_down(PIR_PIN);
    
    printf("PIR Motion Sensor initialized\n");
    printf("Monitoring for animal movement...\n");
    
    while (true) {
        // Read PIR sensor value
        bool pir_value = gpio_get(PIR_PIN);
        
        if (pir_value == true) {  // Motion detected (HIGH signal)
            printf("ANIMAL DETECTED! Activating deterrent...\n");
            
            // Here you can add your deterrent actions:
            // - Activate a buzzer/speaker
            // - Turn on lights
            // - Trigger a water sprayer
            // - Send notification via WiFi
            
            sleep_ms(2000);  // Wait 2 seconds before checking again
        }
        
        sleep_ms(100);  // Small delay to prevent excessive polling
    }
    
    return 0;
}
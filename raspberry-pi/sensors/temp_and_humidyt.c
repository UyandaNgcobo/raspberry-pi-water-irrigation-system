/*the following code has been written by Andie mbokazi second year compyuter science 
*student at Eduvos. It's meant to take readigs as well as display readings of temperature and humidity
*from a DHT11 sensor connected to a Raspberry Pi Pico. The code uses the Pico SDK for GPIO and timing functions.
*This code is part of a project for ITCAA2-33, Question 2 functionalities*/

#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "dht11.h"  // Include a DHT11 library for sensor interaction

// ---------------- Pin Configuration ---------------- //
#define DHT11_PIN 15  // GPIO pin connected to DHT11 data pin
#define READ_INTERVAL 2000  // Interval between readings in milliseconds
#define MAX_RETRIES 5  // Maximum retries for reading sensor data
#define RETRY_DELAY 1000  // Delay between retries in milliseconds
#define TIMEOUT 1000  // Timeout for sensor response in microseconds
#define ERROR_VALUE -1  // Error value for failed readings
#define TEMP_OFFSET 0  // Calibration offset for temperature
#define HUMIDITY_OFFSET 0  // Calibration offset for humidity
#define TEMP_THRESHOLD 30  // Temperature threshold for alert
#define HUMIDITY_THRESHOLD 70  // Humidity threshold for alert
#define ALERT_PIN 25  // GPIO pin for alert (e.g., LED or buzzer)
#define ALERT_DURATION 5000  // Duration to keep alert active in milliseconds
#define ALERT_COOLDOWN 60000  // Cooldown period between alerts in milliseconds
#define ALERT_CHECK_INTERVAL 1000  // Interval to check for alert conditions in milliseconds
#define MAX_ALERTS 3  // Maximum number of alerts before stopping further alerts

// ---------------- Function Prototypes ---------------- //
void init_system();
int read_dht11(int *temperature, int *humidity);
void check_alert_conditions(int temperature, int humidity);
void trigger_alert();
void reset_alert_state();
bool is_alert_active = false;
int alert_count = 0;
absolute_time_t last_alert_time = {0};
absolute_time_t alert_start_time = {0};
absolute_time_t last_check_time = {0};

int main() {
    stdio_init_all();
    init_system();

    int temperature = 0;
    int humidity = 0;

    while (true) {
        if (read_dht11(&temperature, &humidity) == 0) {
            printf("Temperature: %d C, Humidity: %d%%\n", temperature, humidity);
            check_alert_conditions(temperature, humidity);
        } else {
            printf("Failed to read from DHT11 sensor.\n");
        }

        sleep_ms(READ_INTERVAL);
    }
}

// ---------------- Function Definitions ---------------- //
// Initialize GPIO for DHT11 and alert pin
void init_system() {
    gpio_init(DHT11_PIN);
    gpio_set_dir(DHT11_PIN, GPIO_IN);
    gpio_pull_up(DHT11_PIN);  // Enable pull-up resistor for DHT11 data line

    gpio_init(ALERT_PIN);
    gpio_set_dir(ALERT_PIN, GPIO_OUT);
    gpio_put(ALERT_PIN, 0);  // Ensure alert pin is low initially
}

// Read temperature and humidity from DHT11 sensor
int read_dht11(int *temperature, int *humidity) {
    for (int attempt = 0; attempt < MAX_RETRIES; attempt++) {
        if (dht11_read(DHT11_PIN, temperature, humidity) == DHT11_OK) {
            *temperature += TEMP_OFFSET;
            *humidity += HUMIDITY_OFFSET;
            return 0;  // Success
        }
        sleep_ms(RETRY_DELAY);
    }
    return ERROR_VALUE;  // Failed after retries
}
// Check if alert conditions are met and trigger alert if necessary
void check_alert_conditions(int temperature, int humidity) {
    absolute_time_t current_time = get_absolute_time();

    // Reset alert state if cooldown period has passed
    if (is_alert_active && absolute_time_diff_us(alert_start_time, current_time) > ALERT_DURATION * 1000) {
        reset_alert_state();
    }

    // Check if we can trigger a new alert
    if (!is_alert_active && (absolute_time_diff_us(last_alert_time, current_time) > ALERT_COOLDOWN * 1000)) {
        if ((temperature > TEMP_THRESHOLD || humidity > HUMIDITY_THRESHOLD) && alert_count < MAX_ALERTS) {
            trigger_alert();
            last_alert_time = current_time;
            alert_start_time = current_time;
            alert_count++;
        }
    }
}
// Trigger the alert by activating the alert pin
void trigger_alert() {
    gpio_put(ALERT_PIN, 1);  // Activate alert (e.g., turn on LED or buzzer)
    is_alert_active = true;
    printf("ALERT: Temperature or Humidity threshold exceeded!\n");
}
// Reset the alert state and deactivate the alert pin
void reset_alert_state() {
    gpio_put(ALERT_PIN, 0);  // Deactivate alert
    is_alert_active = false;
    printf("Alert deactivated.\n");
}
// ---------------- End of File ---------------- //
/*the following code has been written by Andie mbokazi second year compyuter science
*student at Eduvos. It's meant to take readigs as well as display readings of temperature and humidity
*from a DHT11 sensor connected to a Raspberry Pi Pico. The code uses the Pico SDK for GPIO and timing functions.
*This code is part of a project for ITCAA2-33, Question 2 functionalities*/
/*

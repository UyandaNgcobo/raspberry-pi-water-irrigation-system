// ---------------- watering_system_main.c ---------------- //
/*
 * Smart Irrigation System - Raspberry Pi Pico W
 * Author: Andile Mbokazi
 * Components:
 * - Soil moisture sensor (1x)
 * - Relay module + water pump
 * - Servo motor (pump speed indicator)
 * - Proximity sensor
 * - SSD1306 OLED (I2C)
 * - Switch for manual stop
 * - Red LED alert
 */
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "FreeRTOS.h"
#include "task.h"

// --- Pin definitions ---
#define SOIL_PIN       26  // ADC0
#define RELAY_PIN       2
#define SERVO_PIN       3
#define PROX_PIN        4
#define LED_ALERT       6
#define DHT_PIN         7  // temperature & humidity sensor

// --- I2C pins for LCD ---
#define I2C_PORT i2c0
#define I2C_SDA  8
#define I2C_SCL  9
#define LCD_ADDR 0x27   // common I2C address

// --- Globals ---
volatile uint8_t dry_zones = 0;
volatile uint32_t irrigation_count = 0;
volatile bool manual_abort_flag = false;
volatile bool manual_start_flag = false;
#define MAX_CYCLES 30
#define WATER_SECONDS 30

volatile float temperature = 0;
volatile float humidity = 0;

// --- Function prototypes ---
uint16_t read_soil(void);
bool intrusion_detected(void);
void servo_set_angle(float angle);
bool read_dht(float *temperature, float *humidity);

// --- LCD function prototypes ---
void lcd_send_cmd(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_clear();
void lcd_init();
void lcd_set_cursor(int col, int row);
void lcd_print(const char *str);

// --- Soil sensor task ---
void soil_task(void *params) {
    while(1) {
        uint16_t soil = read_soil();
        uint8_t zones = 0;

        if(soil < 1000) zones |= 0x01;
        if(soil < 1500) zones |= 0x02;
        if(soil < 2000) zones |= 0x04;

        // Skip watering if humidity > 80%
        if(humidity > 80) zones = 0;

        dry_zones = zones;

        // Update LCD with soil + humidity
        lcd_clear();
        lcd_set_cursor(0,0);
        lcd_print("Soil Dryness:");

        lcd_set_cursor(0,1);
        char buf[16];
        snprintf(buf, sizeof(buf), "Val:%d Hum:%.0f%%", soil, humidity);
        lcd_print(buf);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// --- Irrigation task ---
void irrigation_task(void *params) {
    while(1) {
        if(dry_zones == 0 && !manual_start_flag) {
            vTaskDelay(pdMS_TO_TICKS(500));
            continue;
        }

        for(int zone=0; zone<3; zone++) {
            if((dry_zones & (1<<zone)) || manual_start_flag) {
                manual_start_flag = false; // reset manual override
                manual_abort_flag = false; // reset abort flag
                printf("\n=== Starting watering Zone %d ===\n", zone+1);

                // Update LCD
                lcd_clear();
                lcd_set_cursor(0,0);
                char msg[16];
                snprintf(msg, sizeof(msg), "Watering Z%d", zone+1);
                lcd_print(msg);

                gpio_put(RELAY_PIN, 1);

                // Servo position
                if(dry_zones == 0x01) servo_set_angle(45);
                else if(dry_zones == 0x03) servo_set_angle(90);
                else servo_set_angle(135);

                int seconds = WATER_SECONDS;
                while(seconds > 0) {
                    if(manual_abort_flag) {
                        printf("Manual abort via CLI!\n");
                        break;
                    }
                    if(intrusion_detected()) {
                        printf("INTRUSION detected! Stopping watering.\n");
                        gpio_put(RELAY_PIN, 0);
                        gpio_put(LED_ALERT, 1);

                        lcd_clear();
                        lcd_set_cursor(0,0);
                        lcd_print("INTRUSION ALERT!");
                        vTaskDelay(pdMS_TO_TICKS(2000));
                        gpio_put(LED_ALERT, 0);
                        break;
                    }

                    // Console status
                    printf("[Zone %d] Watering... %d s | Temp=%.1fC Hum=%.1f%%\n",
                           zone+1, seconds, temperature, humidity);

                    // LCD countdown
                    lcd_set_cursor(0,1);
                    char timer[16];
                    snprintf(timer, sizeof(timer), "Time:%02ds", seconds);
                    lcd_print(timer);

                    vTaskDelay(pdMS_TO_TICKS(1000));
                    seconds--;
                }

                gpio_put(RELAY_PIN, 0);
                printf("=== Finished watering Zone %d ===\n", zone+1);

                lcd_clear();
                lcd_set_cursor(0,0);
                lcd_print("Zone Done");

                irrigation_count++;
                if(irrigation_count >= MAX_CYCLES) {
                    printf("!!! MAINTENANCE REQUIRED !!!\n");
                    irrigation_count = 0;

                    lcd_clear();
                    lcd_set_cursor(0,0);
                    lcd_print("Maintenance!");
                }

                vTaskDelay(pdMS_TO_TICKS(2000));
            }
        }
    }
}

// --- Servo helper ---
void servo_set_angle(float angle) {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(SERVO_PIN);
    uint channel = pwm_gpio_to_channel(SERVO_PIN);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, 64.f);
    pwm_init(slice, &cfg, true);
    pwm_set_wrap(slice, 20000);
    uint16_t duty = 500 + (uint16_t)((angle/180.0f)*2000);
    pwm_set_chan_level(slice, channel, duty);
}

// --- Soil sensor ---
uint16_t read_soil() {
    adc_select_input(0);
    return adc_read();
}

// --- Intrusion sensor ---
bool intrusion_detected() {
    return gpio_get(PROX_PIN);
}

// --- DHT sensor read (simplified) ---
bool read_dht(float *t, float *h) {
    *t = 25.0;
    *h = 60.0;
    return true;
}

// --- DHT task ---
void dht_task(void *params) {
    while(1) {
        if(read_dht(&temperature, &humidity)) {
            printf("[DHT] Temp=%.1fC Hum=%.1f%%\n", temperature, humidity);
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

// --- CLI task ---
void cli_task(void *params) {
    char buf[32];
    while(1) {
        printf("\nEnter command (start/stop/status): ");
        fflush(stdout);

        int idx = 0;
        int c;
        while((c = getchar_timeout_us(1000000)) != PICO_ERROR_TIMEOUT) {
            if(c == '\r' || c == '\n') break;
            if(idx < sizeof(buf)-1) buf[idx++] = (char)c;
        }
        buf[idx] = '\0';

        if(strcmp(buf, "start") == 0) {
            manual_start_flag = true;
            printf("Manual start requested!\n");
        } else if(strcmp(buf, "stop") == 0) {
            manual_abort_flag = true;
            printf("Manual stop requested!\n");
        } else if(strcmp(buf, "status") == 0) {
            printf("\n--- System Status ---\n");
            printf("Dry zones: %02X\n", dry_zones);
            printf("Temperature: %.1fC\n", temperature);
            printf("Humidity: %.1f%%\n", humidity);
            printf("Irrigation count: %d\n", irrigation_count);
            printf("--------------------\n");
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

// --- Main ---
int main() {
    stdio_init_all();
    printf("Smart Irrigation System with LCD + CLI\n");

    gpio_init(RELAY_PIN); gpio_set_dir(RELAY_PIN, GPIO_OUT);
    gpio_init(LED_ALERT); gpio_set_dir(LED_ALERT, GPIO_OUT);
    gpio_init(PROX_PIN); gpio_set_dir(PROX_PIN, GPIO_IN);
    gpio_init(DHT_PIN);
    adc_init();
    adc_gpio_init(SOIL_PIN);

    // Init I2C for LCD
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    lcd_init();

    // --- FreeRTOS tasks ---
    xTaskCreate(soil_task, "SoilTask", 256, NULL, 2, NULL);
    xTaskCreate(irrigation_task, "IrrigationTask", 512, NULL, 2, NULL);
    xTaskCreate(dht_task, "DHTTask", 256, NULL, 1, NULL);
    xTaskCreate(cli_task, "CLITask", 512, NULL, 3, NULL);

    vTaskStartScheduler();
    while(1) {}
}

/////////////////////////////////////////////////////
// --- LCD driver functions (I2C 16x2, PCF8574) ---
/////////////////////////////////////////////////////
void lcd_send_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x80, cmd};
    i2c_write_blocking(I2C_PORT, LCD_ADDR, buf, 2, false);
}
void lcd_send_data(uint8_t data) {
    uint8_t buf[2] = {0x40, data};
    i2c_write_blocking(I2C_PORT, LCD_ADDR, buf, 2, false);
}
void lcd_clear() {
    lcd_send_cmd(0x01);
    sleep_ms(2);
}
void lcd_init() {
    sleep_ms(50);
    lcd_send_cmd(0x38);
    lcd_send_cmd(0x0C);
    lcd_send_cmd(0x01);
    sleep_ms(2);
}
void lcd_set_cursor(int col, int row) {
    int row_offsets[] = {0x00, 0x40};
    lcd_send_cmd(0x80 | (col + row_offsets[row]));
}
void lcd_print(const char *str) {
    while(*str) lcd_send_data(*str++);
}
// ------------------------------------------------------- //
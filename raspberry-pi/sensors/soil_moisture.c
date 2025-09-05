soil_moisture.c
/*This program calculates soil moisture content based on input parameters and gives an average soil moisture value using
multiple DMA readings. It uses DMA to efficiently transfer ADC data to a buffer, minimizing CPU load.
It periodically samples the soil moisture sensor and processes the data to provide a real-time soil moisture percentage.
Author: Your Andile Mbokazi, Uyanda Ngcobo and Lesiluse maghebe
Date: 2024-06-15
Version: 1.0
Second year computer science students at eduvos
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

#define SOIL_MOISTURE_PIN 26
#define BUFFER_SIZE 10

static uint16_t adc_buffer[BUFFER_SIZE];
static int dma_chan;

// Initialize ADC + DMA
void soil_moisture_init() {
    adc_init();
    adc_gpio_init(SOIL_MOISTURE_PIN);
    adc_select_input(0);

    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_dreq(&cfg, DREQ_ADC);

    dma_channel_configure(
        dma_chan, &cfg,
        adc_buffer,          // destination buffer
        &adc_hw->fifo,       // source
        BUFFER_SIZE,         // number of transfers
        false                // don’t start yet
    );

    adc_fifo_setup(
        true,    // enable FIFO
        true,    // enable DMA request
        1, false, false
    );
}

// Take multiple DMA samples and return average soil moisture %
float soil_moisture_read() {
    // Start DMA + ADC
    dma_channel_set_read_addr(dma_chan, &adc_hw->fifo, true);
    adc_run(true);

    // Wait until DMA transfer is done
    dma_channel_wait_for_finish_blocking(dma_chan);
    adc_run(false);

    // Average buffer
    uint32_t sum = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        sum += adc_buffer[i];
    }
    uint16_t avg_adc = sum / BUFFER_SIZE;

    // Convert ADC → moisture percentage
    float soil_moisture = (avg_adc / 4095.0f) * 100.0f;
    return soil_moisture;
}

// End of soil_moisture.c
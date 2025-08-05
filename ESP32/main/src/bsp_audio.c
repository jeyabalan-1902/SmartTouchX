#include "bsp_audio.h"
#include "driver/i2s.h"

#define I2S_NUM         I2S_NUM_0
#define I2S_BCLK        41
#define I2S_WS          40
#define I2S_DOUT        42

void bsp_audio_init(void) {
    i2s_config_t i2s_cfg = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = true,
    };

    i2s_pin_config_t pin_cfg = {
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_DOUT,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_driver_install(I2S_NUM, &i2s_cfg, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_cfg);
}

void bsp_audio_write(int16_t *samples, size_t sample_count) {
    size_t bytes_written;
    i2s_write(I2S_NUM, samples, sample_count * sizeof(int16_t), &bytes_written, portMAX_DELAY);
}

void bsp_audio_deinit(void) {
    i2s_driver_uninstall(I2S_NUM);
}

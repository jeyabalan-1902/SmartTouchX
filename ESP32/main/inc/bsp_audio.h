#ifndef BSP_AUDIO_H
#define BSP_AUDIO_H

#include <stdint.h>
#include <stddef.h>

void bsp_audio_init(void);
void bsp_audio_write(int16_t *samples, size_t sample_count);
void bsp_audio_deinit(void);

#endif // BSP_AUDIO_H

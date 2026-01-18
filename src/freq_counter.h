#ifndef FREQ_COUNTER_H
#define FREQ_COUNTER_H

#include <stdbool.h>
#include <stdint.h>

// Pin definitions for frequency measurement
#define PIN_FREQ_PCLK  20
#define PIN_FREQ_R0    21
#define PIN_FREQ_CSYNC 22

// Measured frequencies (updated by freq_counter_update)
extern volatile uint32_t freq_pclk_hz;
extern volatile uint32_t freq_r0_hz;
extern volatile uint32_t freq_csync_hz;

// Initialize frequency counter hardware
void freq_counter_init(void);

// Update frequency measurements (call periodically from main loop)
// Returns true if new measurements are available
bool freq_counter_update(void);

#endif // FREQ_COUNTER_H

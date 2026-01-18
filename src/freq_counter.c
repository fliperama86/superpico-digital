#include "freq_counter.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"

// Measured frequencies
volatile uint32_t freq_pclk_hz = 0;
volatile uint32_t freq_r0_hz = 0;
volatile uint32_t freq_csync_hz = 0;

// PIO instance
static PIO pio_freq = pio0;

// State machines for each input
static uint sm_pclk;
static uint sm_r0;
static uint sm_csync;

// PIO program offset
static uint prog_offset;

// Last sample time
static absolute_time_t last_sample_time;

// Sample interval in microseconds (250ms gives good resolution for both high and low freq)
#define SAMPLE_INTERVAL_US 250000

// PIO program: count rising edges
// Uses proper SDK encoding
static uint16_t freq_count_program[4];

static void build_freq_count_program(void) {
    // wait 0 pin 0  - wait for pin to go low
    freq_count_program[0] = pio_encode_wait_pin(false, 0);
    // wait 1 pin 0  - wait for pin to go high (rising edge)
    freq_count_program[1] = pio_encode_wait_pin(true, 0);
    // jmp x-- 0     - decrement X and jump back to start (counts edges)
    freq_count_program[2] = pio_encode_jmp_x_dec(0);
}

#define FREQ_PROG_LEN 3

static void init_sm_for_pin(PIO pio, uint sm, uint offset, uint pin) {
    // Configure GPIO
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, false);  // Input
    gpio_pull_down(pin);
    gpio_set_input_hysteresis_enabled(pin, true);

    // Configure state machine
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset, offset + FREQ_PROG_LEN - 1);
    sm_config_set_in_pins(&c, pin);      // Set IN base pin for wait pin instruction
    sm_config_set_clkdiv(&c, 1.0f);      // Run at full speed

    // Initialize SM
    pio_sm_init(pio, sm, offset, &c);

    // Set X to max value (0xFFFFFFFF) - we count down
    // set x, 0 then mov x, ~x
    pio_sm_exec(pio, sm, pio_encode_set(pio_x, 0));
    pio_sm_exec(pio, sm, pio_encode_mov_not(pio_x, pio_x));

    // Start SM
    pio_sm_set_enabled(pio, sm, true);
}

// Read X register value from a state machine
static uint32_t read_and_reset_x(PIO pio, uint sm, uint offset) {
    // Stop SM briefly to read X
    pio_sm_set_enabled(pio, sm, false);

    // Move X to ISR and push to FIFO
    pio_sm_exec(pio, sm, pio_encode_mov(pio_isr, pio_x));
    pio_sm_exec(pio, sm, pio_encode_push(false, false));

    // Read value from FIFO
    uint32_t x_val = pio_sm_get_blocking(pio, sm);

    // Calculate count (we counted down from 0xFFFFFFFF)
    uint32_t count = 0xFFFFFFFF - x_val;

    // Reset X to max (0xFFFFFFFF)
    pio_sm_exec(pio, sm, pio_encode_set(pio_x, 0));
    pio_sm_exec(pio, sm, pio_encode_mov_not(pio_x, pio_x));

    // Restart SM at beginning of program
    pio_sm_exec(pio, sm, pio_encode_jmp(offset));
    pio_sm_set_enabled(pio, sm, true);

    return count;
}

void freq_counter_init(void) {
    // Build the PIO program using SDK helpers
    build_freq_count_program();

    // Add program to PIO
    prog_offset = pio_add_program(pio_freq, &(const pio_program_t){
        .instructions = freq_count_program,
        .length = FREQ_PROG_LEN,
        .origin = -1
    });

    // Claim state machines
    sm_pclk = pio_claim_unused_sm(pio_freq, true);
    sm_r0 = pio_claim_unused_sm(pio_freq, true);
    sm_csync = pio_claim_unused_sm(pio_freq, true);

    // Initialize each frequency counter
    init_sm_for_pin(pio_freq, sm_pclk, prog_offset, PIN_FREQ_PCLK);
    init_sm_for_pin(pio_freq, sm_r0, prog_offset, PIN_FREQ_R0);
    init_sm_for_pin(pio_freq, sm_csync, prog_offset, PIN_FREQ_CSYNC);

    // Initialize timing
    last_sample_time = get_absolute_time();
}

bool freq_counter_update(void) {
    absolute_time_t now = get_absolute_time();
    int64_t elapsed_us = absolute_time_diff_us(last_sample_time, now);

    if (elapsed_us < SAMPLE_INTERVAL_US) {
        return false;
    }

    // Read counts and reset
    uint32_t count_pclk = read_and_reset_x(pio_freq, sm_pclk, prog_offset);
    uint32_t count_r0 = read_and_reset_x(pio_freq, sm_r0, prog_offset);
    uint32_t count_csync = read_and_reset_x(pio_freq, sm_csync, prog_offset);

    // Calculate frequencies: freq = count / time
    // freq_hz = count * 1000000 / elapsed_us
    if (elapsed_us > 0) {
        freq_pclk_hz = (uint32_t)((uint64_t)count_pclk * 1000000ULL / (uint64_t)elapsed_us);
        freq_r0_hz = (uint32_t)((uint64_t)count_r0 * 1000000ULL / (uint64_t)elapsed_us);
        freq_csync_hz = (uint32_t)((uint64_t)count_csync * 1000000ULL / (uint64_t)elapsed_us);
    }

    // Update timestamp
    last_sample_time = now;

    return true;
}

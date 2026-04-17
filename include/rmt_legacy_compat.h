#ifndef RMT_LEGACY_COMPAT_H
#define RMT_LEGACY_COMPAT_H

/**
 * Compatibility header for legacy RMT API types and functions
 * that were removed in ESP-IDF v6.1.
 *
 * Provides:
 *  - rmt_item32_t (alias for rmt_symbol_word_t)
 *  - RMT_CHANNEL_0..7 enums
 *  - rmt_isr_register / rmt_isr_deregister wrappers
 *  - periph_ll_enable_clk_clear_rst(PERIPH_RMT_MODULE) shim
 */

#include <stdint.h>
#include "hal/rmt_types.h"
#include "hal/rmt_ll.h"
#include "esp_intr_alloc.h"
#include "soc/dport_reg.h"
#include "soc/interrupts.h"

#ifdef __cplusplus
extern "C" {
#endif

/* rmt_item32_t was renamed to rmt_symbol_word_t in new ESP-IDF */
typedef rmt_symbol_word_t rmt_item32_t;

/* rmt_isr_handle_t was just intr_handle_t */
typedef intr_handle_t rmt_isr_handle_t;

/* Legacy RMT data mode - use memory access instead of FIFO */
#define RMT_DATA_MODE_MEM  1

/* SOC_RMT_CHANNELS_PER_GROUP removed in new ESP-IDF, ESP32 has 8 channels */
#ifndef SOC_RMT_CHANNELS_PER_GROUP
#define SOC_RMT_CHANNELS_PER_GROUP  8
#endif

/* Legacy RMT memory owner defines */
#define RMT_MEM_OWNER_TX  0
#define RMT_MEM_OWNER_RX  1

/* Legacy RMT idle level defines */
#define RMT_IDLE_LEVEL_LOW   0
#define RMT_IDLE_LEVEL_HIGH  1

/* Legacy gpio_matrix_out/in - now use esp_rom_gpio functions */
#include "esp_rom_gpio.h"
#define gpio_matrix_out(gpio, signal, out_inv, oen_inv) \
    esp_rom_gpio_connect_out_signal((gpio), (signal), (out_inv), (oen_inv))
#define gpio_matrix_in(gpio, signal, inv) \
    esp_rom_gpio_connect_in_signal((gpio), (signal), (inv))

/* Legacy RMT channel enum */
typedef enum {
    RMT_CHANNEL_0 = 0,
    RMT_CHANNEL_1,
    RMT_CHANNEL_2,
    RMT_CHANNEL_3,
    RMT_CHANNEL_4,
    RMT_CHANNEL_5,
    RMT_CHANNEL_6,
    RMT_CHANNEL_7,
    RMT_CHANNEL_MAX,
} rmt_channel_t;

/* Legacy PERIPH_RMT_MODULE shim - enable/disable RMT bus clock directly */
#define PERIPH_RMT_MODULE  0xFFu  /* Dummy value, only used as a tag */

#include "hal/clk_gate_ll.h"

/* Override periph_ll_enable_clk_clear_rst when called with PERIPH_RMT_MODULE */
static inline void _rmt_compat_enable_clk(void)
{
    /* Use the RMT LL function to enable the bus clock and clear reset */
    uint32_t reg_val = DPORT_READ_PERI_REG(DPORT_PERIP_CLK_EN_REG);
    reg_val |= DPORT_RMT_CLK_EN;
    DPORT_WRITE_PERI_REG(DPORT_PERIP_CLK_EN_REG, reg_val);

    /* Clear the reset bit */
    reg_val = DPORT_READ_PERI_REG(DPORT_PERIP_RST_EN_REG);
    reg_val &= ~DPORT_RMT_RST;
    DPORT_WRITE_PERI_REG(DPORT_PERIP_RST_EN_REG, reg_val);
}

/* Legacy rmt_isr_register: wraps esp_intr_alloc for RMT interrupt */
static inline esp_err_t rmt_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, intr_handle_t *handle)
{
    return esp_intr_alloc(ETS_RMT_INTR_SOURCE, intr_alloc_flags, fn, arg, handle);
}

/* Legacy rmt_isr_deregister: wraps esp_intr_free */
static inline esp_err_t rmt_isr_deregister(intr_handle_t handle)
{
    return esp_intr_free(handle);
}

#ifdef __cplusplus
}
#endif

#endif /* RMT_LEGACY_COMPAT_H */

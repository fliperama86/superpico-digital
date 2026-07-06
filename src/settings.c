#include "settings.h"

#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico.h"

#include <string.h>

#define SETTINGS_FLASH_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#define SETTINGS_MAGIC 0x53504753u // "SPGS"
#define SETTINGS_VERSION 1u

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t payload_size;
    superpico_settings_t payload;
    uint32_t crc;
} settings_record_t;

_Static_assert(sizeof(settings_record_t) <= FLASH_PAGE_SIZE, "settings record must fit one flash page");

static uint32_t settings_crc32(const void *data, size_t len)
{
    const uint8_t *p = (const uint8_t *)data;
    uint32_t crc = 0xFFFFFFFFU;
    for (size_t i = 0; i < len; i++) {
        crc ^= p[i];
        for (int b = 0; b < 8; b++) {
            crc = (crc >> 1) ^ (0xEDB88320U & (uint32_t)(-(int32_t)(crc & 1U)));
        }
    }
    return ~crc;
}

static void settings_defaults(superpico_settings_t *out)
{
    memset(out, 0, sizeof(*out));
    out->resolution = 0; // 480p
}

bool settings_load(superpico_settings_t *out)
{
    const settings_record_t *rec = (const settings_record_t *)(XIP_BASE + SETTINGS_FLASH_OFFSET);
    if (rec->magic == SETTINGS_MAGIC && rec->version == SETTINGS_VERSION &&
        rec->payload_size == (uint16_t)sizeof(superpico_settings_t) &&
        rec->crc == settings_crc32(&rec->payload, sizeof(superpico_settings_t))) {
        *out = rec->payload;
        return true;
    }
    settings_defaults(out);
    return false;
}

void __no_inline_not_in_flash_func(settings_save)(const superpico_settings_t *s)
{
    static uint8_t page[FLASH_PAGE_SIZE] __attribute__((aligned(4)));
    memset(page, 0xFF, sizeof(page));

    settings_record_t *rec = (settings_record_t *)page;
    rec->magic = SETTINGS_MAGIC;
    rec->version = SETTINGS_VERSION;
    rec->payload_size = (uint16_t)sizeof(superpico_settings_t);
    rec->payload = *s;
    rec->crc = settings_crc32(&rec->payload, sizeof(superpico_settings_t));

    const uint32_t saved = save_and_disable_interrupts();
    flash_range_erase(SETTINGS_FLASH_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(SETTINGS_FLASH_OFFSET, page, FLASH_PAGE_SIZE);
    restore_interrupts(saved);
}

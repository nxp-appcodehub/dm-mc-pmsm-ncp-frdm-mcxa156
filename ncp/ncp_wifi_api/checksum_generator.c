/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "checksum_generator.h"

static uint32_t g_crc32_table[256] = {0};

void ncp_tlv_chksum_init(void)
{
    int i, j;
    unsigned int c;
    for (i = 0; i < 256; ++i)
    {
        for (c = i << 24, j = 8; j > 0; --j)
            c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
        g_crc32_table[i] = c;
    }
}

uint32_t checksum_generator(uint8_t *buf, uint32_t len)
{
    uint8_t *p;
    uint32_t crc;
    crc = 0xffffffff;
    for (p = buf; len > 0; ++p, --len)
        crc = (crc << 8) ^ (g_crc32_table[(crc >> 24) ^ *p]);
    return ~crc;
}

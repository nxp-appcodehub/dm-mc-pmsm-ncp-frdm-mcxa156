/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CHECKSUM_GENERATOR_H_
#define CHECKSUM_GENERATOR_H_

#include <stdint.h>

#define CRC32_POLY   0x04c11db7
#define CHECKSUM_LEN 4

void ncp_tlv_chksum_init(void);

uint32_t checksum_generator(uint8_t *buf, uint32_t len);

#endif /* CHECKSUM_GENERATOR_H_ */

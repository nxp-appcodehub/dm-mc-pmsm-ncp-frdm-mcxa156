/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef NCP_WLAN_DATA_TYPES_H_
#define NCP_WLAN_DATA_TYPES_H_

#include <stdint.h>

#define RECEIVE_BUFF_SIZE	(10U*1024U)
#define SEND_BUFF_SIZE 		(2U*1024U)

#define NCP_TLV_PACK_START
#define NCP_TLV_PACK_END __attribute__((packed))

#define WM_SUCCESS 0

#define IEEEtypes_SSID_SIZE 32
/** Maximum length of firmware software version string. */
#define MLAN_MAX_VER_STR_LEN         128
/** MAC address length. */
#define MLAN_MAC_ADDR_LENGTH         6
/** Maximum length of network name. */
#define WLAN_NETWORK_NAME_MAX_LENGTH 32
/** Maximum length of identity string . */
#define IDENTITY_MAX_LENGTH          64
/** Maximum length of password string . */
#define PASSWORD_MAX_LENGTH          128
/** Maximum number of STAs are connected to the UAP. */
#define MAX_NUM_CLIENTS              16

#define MAX_MONIT_MAC_FILTER_NUM     3
/** Maximum CSI filter count */
#define CSI_FILTER_MAX               16
/** Size of iperf end token. */
#define NCP_IPERF_END_TOKEN_SIZE 11

/** header structure for NCP command, NCP command response and NCP event. */
typedef struct _NCP_CMD_HEADER_t
{
    /** class: bit 28 ~ 31, subclass: bit 20 ~27, msg type: bit 16 ~ 19, cmd/cmd resp/event id: bit 0 ~ 15 */
    uint32_t cmd;
    /** NCP command total length include header length and body length */
    uint16_t size;
    /** NCP command sequence number */
    uint16_t seqnum;
    /** default OK(0) for NCP command, operation result for NCP command response */
    uint16_t result;
    /** Reserved fields */
    uint16_t rsvd;
}  __attribute__((packed)) NCP_COMMAND;

/** NCP tlv header */
typedef  struct _TLVTypeHeader_t
{
    /** tlv type */
    uint16_t type;
	/** size */
    uint16_t size;
}  TypeHeader_t;

void ncp_api_init(void);

void ncp_rx_gpio_callback(void);

void ncp_tx_gpio_callback(void);

void ncp_wait_ncp_device_ready_for_reception(void);

void ncp_wait_ncp_device_ready_for_transmision(void);

void ncp_send(uint8_t *buff, uint32_t buff_size);

void ncp_receive(uint8_t *buff, uint32_t *buff_size);


#endif /* NCP_COMMON_H_ */

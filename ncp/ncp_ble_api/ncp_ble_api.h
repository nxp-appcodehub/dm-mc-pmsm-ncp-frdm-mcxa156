/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef NCP_BLE_API_NCP_BLE_API_H_
#define NCP_BLE_API_NCP_BLE_API_H_

#include <stdint.h>
#include "ncp_cmd_ble.h"


typedef enum _ble_scan_type_t
{
	k_ble_passive_scan,
	k_ble_active_scan
} ble_scan_type_t;

typedef enum _ble_address_type_t
{
	k_address_type_public,
	k_address_type_random
} ble_address_type_t;

typedef enum _ble_connection_phy_t
{
	k_phy_opt_none		= 0x00,
	k_phy_opt_coded_s2	= 0x01,
	k_phy_opt_coded_s8	= 0x02
} ble_connection_phy_t;

typedef enum _ble_transfer_phy_t
{
	k_phy_none	= 0x00,
	k_phy_1m	= 0x01,
	k_phy_2m	= 0x02,
	k_phy_coded	= 0x04
} ble_transfer_phy_t;

typedef enum _ble_power_mode_t
{
	k_ble_auto_sleep_disable = 0x02,
	k_ble_auto_sleep_enable = 0x03
} ble_power_mode_t;

typedef enum _ble_subscribe_type_t
{
	k_subscribe_indicate,
	k_subscribe_notify,
} ble_subscribe_type_t;

typedef enum _ble_l2cap_security_t
{
	k_no_encryption_no_authentication  = 0x01,
	k_encryption_no_authentication	   = 0x02,
	k_encryption_authentication  	   = 0x03
} ble_l2cap_security_t;

typedef enum _ble_scan_param_options_t
{
	k_ble_scan_opt_none,
	k_ble_scan_filtter_duplicate,
	k_ble_scan_filtter_accept_list
} ble_scan_param_options_t;


/*
 * @brief  This function set advertise array for start advertise.
 * @param  advertise_array -> data array to going to be set.
 * @param  adv_length      -> length of data array.
 * @return None
 */
void ncp_ble_set_adv_data(uint8_t *advertise_array, uint32_t adv_length);


/*
 * @brief  This function start advertise.
 * @return None
 */
void ncp_ble_start_adv(void);

/*
 * @brief  This function stop advertise.
 * @return None
 */
void ncp_ble_stop_adv(void);

/*
* @brief  This function set scanning parameters (Scan filters, scan interval and scan window)
* @param  option   -> Filter options.
* @param  interval -> Range from 4 to 16384 (Decimal value) real_time =  N * 0.625 ms.
* @param  window   ->  Range: 4 to 16384 (Decimal value, window value shall be less than or equal to interval value) real_time =  N * 0.625 ms.
* @return None
*/
void ncp_ble_set_scan_param(ble_scan_param_options_t option, uint16_t interval, uint16_t window);

/*
 * @brief  This function start scanning.
 * @param  scan_type -> Selects between active or passive scan.
 * @return None
 */
void ncp_ble_start_scan(ble_scan_type_t scan_type);

/*
 * @brief  This function stop scanning.
 * @return None
 */
void ncp_ble_stop_scan(void);

/*
* @brief  This function will try to connect to peripheral.
* @param  addr -> Address of peripheral.
* @param  type -> Type of address (Public or random).
* @return None
*/
void ncp_ble_connect(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type);

/*
* @brief  This function disconnects device.
* @param  addr -> Address.
* @param  type -> Type of address (Public or random).
* @return None
*/
void ncp_ble_disconnect(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type);

/*
* @brief  This function changes data length.
* @param  addr        -> Address.
* @param  type        -> Type of address (Public or random).
* @param  tx_max_len  -> Max data length.
* @param  tx_max_time -> Max transmit time.
* @return None
*/
void ncp_ble_set_data_len(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, uint16_t tx_max_len, uint16_t tx_max_time);

/*
* @brief  This function sets ble phy.
* @param  addr         -> Address.
* @param  type         -> Type of address (Public or random).
* @param  connect_phy  -> Phy mode for connection.
* @param  tx_phy       -> Phy mode for transmit.
* @param  rx_phy 	   -> Phy mdoe for receive.
* @return None
*/
void ncp_ble_set_phy(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, ble_connection_phy_t connect_phy, ble_transfer_phy_t tx_phy, ble_transfer_phy_t rx_phy);

/*
* @brief  This function updates connection parameters.
* @param  addr         -> Address.
* @param  type         -> Type of address (Public or random).
* @param  interval_min -> Minimum interval for connection real_time =  N * 1.25ms.
* @param  interval_max -> Maximum interval for connection real_time =  N * 1.25ms.
* @param  latency      -> Number of connection events can peripheral safely skip.
* @param  timeout 	   -> Connection timeout real_time =  N * 10ms.
* @return None
*/
void ncp_ble_connect_param_update(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, uint16_t interval_min, uint16_t interval_max, uint16_t latency, uint16_t timeout);

/*
* @brief  This function starts encryption.
* @param  addr        -> Address.
* @param  type        -> Type of address (Public or random).
* @return None
*/
void ncp_ble_start_encryption(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type);

/*
* @brief  This writes into characteristic (Peripheral mode).
* @param  uuid      -> UUID pointer of characteristic.
* @param  uuid_len  -> UUID len.
* @param  data  	-> Data pointer to be written.
* @param  data_len 	-> Size of data.
* @return None
*/
void ncp_ble_write_characteristic(uint8_t *uuid, uint8_t uuid_len, uint8_t *data, uint16_t data_len);

/*
* @brief  This function reads characteristic (Central mode).
* @param  addr    -> Address.
* @param  type    -> Type of address (Public or random).
* @param  handle  -> Handler of characteristic.
* @return None
*/
void ncp_ble_read_characteristic(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, uint16_t handle);

/*
* @brief  This function sets power mode.
* @param  power_mode -> Enable or disable auto sleep.
* @return None
*/
void ncp_ble_set_power_mode(ble_power_mode_t power_mode);

// IN TEST ----------------------------------------------------------------------------
/*
* @brief  This function register a service.
* @param  service_id  -> Custom service ID.
*						 1: Peripheral_HTS
*						 2: Peripheral HRS
*						 3: BAS
*						 4: Central_HTS
*						 5: Central_HRS
* @param  service_len -> Service id len.
* @return None
*/
void ncp_ble_register_service(uint8_t *service_id, uint8_t service_len);

// IN TEST ----------------------------------------------------------------------------
/*
* @brief  This function register a service.
* @param  service_id  -> Custom service ID.
*						 1: Peripheral_HTS
*						 2: Peripheral HRS
*						 3: BAS
*						 4: Central_HTS
*						 5: Central_HRS
* @return None
*/
void ncp_ble_start_service(uint8_t service_id);

// IN TEST ----------------------------------------------------------------------------
/*
* @brief  This function register a service.
* @param  addr      -> Address device.
* @param  type      -> Type of address (Public or random).
* @param  subs_type -> Subscribe type (Indicate or Notify).
* @param  handle    -> Handler.
* @param  enable    -> Enable.
* @return None
*/
void ncp_ble_subscribe_service(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, ble_subscribe_type_t subs_type, uint16_t handle, uint8_t enable);

/*
* @brief  This function do a l2cap connection.
* @param  addr        					-> Address of device.
* @param  type        					-> Type of address (Public or random).
* @param  protocol_service_multiplexer  -> Desired protocol service multiplexer (ATT = 0x001F).
* @param  security 						-> Desired security.
* @return None
*/
void ncp_ble_l2cap_connect(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, uint16_t protocol_service_multiplexer, ble_l2cap_security_t security);

/*
* @brief  This function do a l2cap disconnection.
* @param  addr        -> Address of device.
* @param  type        -> Type of address (Public or random).
* @return None
*/
void ncp_ble_l2cap_disconnect(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type);

// IN TEST ----------------------------------------------------------------------------
/*
* @brief  This function sends data in l2cap layer.
* @param  addr   -> Address of peripheral.
* @param  type   -> Type of address (Public or random).
* @param  times  -> Times of loop.
* @return None
*/
void ncp_ble_l2cap_send(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, uint16_t times);

// IN TEST ----------------------------------------------------------------------------
/*
* @brief  This function changes data length.
* @param  protocol_service_multiplexer  -> Desired protocol service multiplexer (ATT = 0x001F).
* @param  security  					-> Desired security.
* @param  policy 						-> Use or no use allowlist.
* @return None
*/
void ncp_ble_l2cap_register(uint16_t protocol_service_multiplexer, ble_l2cap_security_t security, uint8_t policy);

#endif /* NCP_BLE_API_NCP_BLE_API_H_ */
